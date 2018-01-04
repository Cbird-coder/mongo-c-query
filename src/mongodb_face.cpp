/** ***************************
**auther:caiyunlong.2017
*******************************/
#include "db_common.h"
#include "image_process.h"
mongoc_database_t    *database;
mongoc_collection_t  *collection;
mongoc_client_t      *client;
//debug BSON content function
void debug_bson(bson_t  *insert)
{    
    char *str;
    str = bson_as_json (insert, NULL);
    printf ("BSON :%s\n", str);
    bson_free (str);
}
void debug_bson(const bson_t  *insert)
{    
    char *str;
    str = bson_as_json (insert, NULL);
    printf ("BSON :%s\n", str);
    bson_free (str);
 }

//use in inner function
int GetBinary_list( bson_iter_t iter,std::vector<string> &f_list,string input_str)
{
       bson_iter_t iter2;
       bson_iter_t fIter;
       bson_t * fSubArray;
       int feature_size = 0;
       uint32_t length;
       string feature_name;
       int count=0;
       char array_index[128];

       if(bson_iter_find(&iter, "feature_size"))
       {
           feature_size = bson_iter_int32(&iter);
       }
       if (bson_iter_find(&iter, "feature")){
          const uint8_t *array = NULL;
          uint32_t array_len = 0;
          bson_iter_array(&iter,&array_len,&array);
          fSubArray = bson_new_from_data(array, array_len);
          bson_iter_init(&fIter, fSubArray);
          while (feature_size!=0){
            sprintf(array_index,"%d",count);
            if(bson_iter_find_descendant(&fIter,array_index,&iter2)){
              feature_name=bson_iter_utf8(&iter2,&length);
            }
            if (feature_name == input_str){
                return 1;
            }
            f_list.push_back(feature_name);
            ++count;
            --feature_size;
          }
       }
       return 0;
}

int GetRecord(const bson_t *doc,databaseinfo &people)
{
    bson_iter_t iter;
    bson_iter_t iter2;
    uint32_t length;

    bson_iter_init(&iter,doc);
  
    if(bson_iter_find_descendant(&iter,"name",&iter2))
    {
      people.name = bson_iter_utf8(&iter2,&length);
      //printf("%s|",people.name.c_str());
    }
    if(bson_iter_find_descendant(&iter,"gender",&iter2))
    {
      people.gender= bson_iter_utf8(&iter2,&length);
      //printf("%s|",people.gender.c_str());
    }
    if(bson_iter_find(&iter, "age"))
    {
        people.age = bson_iter_int32(&iter);
        //printf("%d|",people.age);
    }
    if(bson_iter_find(&iter, "id_p"))
    {
        people.id_p = bson_iter_int32(&iter);
        //printf("%d\n",people.id_p );
    }
    return 0;
}
int GetFeature(const bson_t *doc,std::vector<feat_datasheet> &featurelist)
{
      feat_datasheet feat_temp;
      bson_iter_t iter_feat;
      bson_iter_init(&iter_feat,doc);

      if (bson_iter_find(&iter_feat, "feature_index")){
          feat_temp.feature_index  = bson_iter_int32(&iter_feat);
      }
      if (bson_iter_find(&iter_feat, "feature_Mat"))
      {
        const uint8_t *binary = NULL;
        bson_subtype_t subtype = BSON_SUBTYPE_BINARY;

        uint32_t binary_len = 0;
        bson_iter_binary(&iter_feat, &subtype, &binary_len, &binary);
        uint8_t *tempbuffer = (uint8_t*)malloc(binary_len);
        memcpy(tempbuffer,binary,binary_len);
        if(restore_facefeature(tempbuffer,feat_temp.feature) < 0){
          printf("wrong feature type");
          return -1;
        }
        featurelist.push_back(feat_temp);
        free(tempbuffer);
      }
}
//get all people infotmation
int query_pesoninfo(std::vector<databaseinfo> &peosoninfolist)
{
    mongoc_cursor_t *cursor;
    bson_t *filter;
    const bson_t *doc;
    filter =bson_new ();
    peosoninfolist.clear();
    cursor = mongoc_collection_find(collection, MONGOC_QUERY_NONE, 0, 0, 0, filter, NULL, NULL);
    while (mongoc_cursor_next (cursor, &doc)) {
        bson_iter_t iter;
        bson_iter_init (&iter, doc);
        while (bson_iter_next (&iter)){
            const char *key;
            key = bson_iter_key(&iter);
            string key_str = key;
            if (key_str == "name")
            {
              databaseinfo tmpperson;
              GetRecord(doc,tmpperson);
              peosoninfolist.push_back(tmpperson);
            }
          }
    }
    bson_destroy (filter);
    mongoc_cursor_destroy (cursor);
}
//get all feature
int query_fearture(std::vector<feat_datasheet> &featurelist){
    mongoc_cursor_t *cursor;
    bson_t *filter;
    const bson_t *doc;
    filter =bson_new ();
    featurelist.clear();
    cursor = mongoc_collection_find(collection, MONGOC_QUERY_NONE, 0, 0, 0, filter, NULL, NULL);
    while (mongoc_cursor_next (cursor, &doc)) {
       bson_iter_t iter;
       bson_iter_init (&iter, doc);
       while (bson_iter_next (&iter)) {
            const char *key;
            key = bson_iter_key(&iter);
            string key_str = key;
            if (key_str == "feature_index")
            {
              GetFeature(doc,featurelist);
            }
     }
   }
    bson_destroy (filter);
    mongoc_cursor_destroy (cursor);
 }
//insert 
int insert_iterm_info(const char* name,const char* gender,int age,vector<string> feature,int id_seq)
{
    bson_error_t error_insert;
    bson_t    *insert;
    bson_t *Array;
    int feature_size = 0;
    Array = bson_new ();
    for(int i=0;i<feature.size();i++){
      char array_index[128];
      sprintf(array_index,"%d",i);
      BSON_APPEND_UTF8(Array,array_index, feature[i].c_str());
     }
    feature_size=(int)(feature.size());
    insert = bson_new ();
    BSON_APPEND_INT32(insert,"feature_size",feature_size);
    BSON_APPEND_ARRAY(insert,"feature", Array);
    BSON_APPEND_UTF8(insert,"name",name);
    BSON_APPEND_UTF8(insert,"gender",gender);
    BSON_APPEND_INT32(insert,"age",age);
    BSON_APPEND_INT32(insert,"id_p",id_seq);

    if (!mongoc_collection_insert (collection, MONGOC_INSERT_NONE, insert, NULL, &error_insert)) {
      fprintf (stderr, "%s\n", error_insert.message);
   }
   bson_destroy (insert);
   bson_destroy (Array);
   return 0;
}
int insert_binary_info(uint8_t* feature,int datalen,int index,char *strOid){
  bson_t    *insert;
  bson_oid_t oid;
  bson_error_t error_insert; 

  bson_oid_init (&oid, NULL);
  insert = bson_new ();
  BSON_APPEND_OID(insert,"_id", &oid);
  BSON_APPEND_INT32(insert,"feature_index",index);
  BSON_APPEND_BINARY(insert,"feature_Mat",BSON_SUBTYPE_BINARY,feature,datalen);
  bson_oid_to_string(&oid,strOid);
  if (!mongoc_collection_insert (collection, MONGOC_INSERT_NONE, insert, NULL, &error_insert)) {
      fprintf (stderr, "%s\n", error_insert.message);
   }
   bson_destroy (insert);
   return 0;
}
/////////////////////////////////////////////////////////////
///////////////////////look up database//////////////////////
int lookup_db_person_count(int &max_id)
{
  mongoc_cursor_t *cursor;
  const bson_t *doc;
  bson_t *query;
  int count = 0;

  max_id = 0;

  query =bson_new ();
  cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
  while (mongoc_cursor_next (cursor, &doc)){
      bson_iter_t iter;
      bson_iter_init(&iter,doc);
     if(bson_iter_find(&iter, "id_p")){
      int id = bson_iter_int32(&iter);
      if (id > max_id){
        max_id = id;
      }
      ++count;
    }
  }
  bson_destroy (query);
  mongoc_cursor_destroy (cursor);
  return count;
}
int lookup_db_name_exist(const char *name,int id,int &min_age)
{
   mongoc_cursor_t *cursor;
   const bson_t *doc;
   bson_error_t error_lk;  
   bson_t *query;
   int64_t count;
   databaseinfo person;

   query = BCON_NEW("name", BCON_UTF8 (name),
                    "id_p",BCON_INT32(id));
   cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
   while (mongoc_cursor_next (cursor, &doc)){
          GetRecord(doc,person);
   }
   min_age = person.age;
   
   count = mongoc_collection_count (collection, MONGOC_QUERY_NONE, query, 0, 0, NULL, &error_lk);
   if (count < 0) {
      fprintf (stderr, "%s\n", error_lk.message);
   } 
   bson_destroy (query);
   mongoc_cursor_destroy (cursor);
   return int(count);
}
int lookup_db_name_repeat(const char *name,int id)
{
   bson_t *query;
   mongoc_cursor_t *cursor;
   const bson_t *doc;
   std::vector<string> v_list; 

   query = BCON_NEW("name", BCON_UTF8 (name),
                    "id_p",BCON_INT32(id)); 
   cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
   while (mongoc_cursor_next (cursor, &doc)) {
        bson_iter_t iter;
        bson_iter_init(&iter,doc);
        GetBinary_list(iter,v_list,"NULL");
    }
   bson_destroy (query);
   mongoc_cursor_destroy (cursor);

   return (int)(v_list.size()); 
}
int lookup_db_by_id(int id,databaseinfo &person)
{
    mongoc_cursor_t *cursor;
    bson_t *query;
    const bson_t *doc;

    query = bson_new ();
    BSON_APPEND_INT32(query,"id_p",id);
    cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
    while (mongoc_cursor_next (cursor, &doc)) {
        GetRecord(doc,person);
    }
    bson_destroy (query);
    mongoc_cursor_destroy (cursor);

    return 0;
 }
 //delete()
int sync_peopleinfolist(std::vector<databaseinfo> &personinfolist,int id)
{
    vector<databaseinfo>::iterator iter = personinfolist.begin();
       while(1)
       {
           if(!(iter != personinfolist.end())){
              break;
            }
            if (iter->id_p == id){
               iter=personinfolist.erase(iter);
               break;
            }else{
              ++iter;
            }
      }
     return 0;   
}
int sync_featurelist(std::vector<feat_datasheet> &featurelist,int feature_id)
{
       vector<feat_datasheet>::iterator iter = featurelist.begin();
       while(1)
       {
           if(!(iter != featurelist.end())){
              break;
            }
            if (iter->feature_index == feature_id){
               iter=featurelist.erase(iter);
            }else{
              ++iter;
            }
        }
     return 0;   
 }
 int delete_feature(const char* name,int id,std::vector<feat_datasheet> &featurelist)
 {
     bson_error_t error_del;
     bson_t *query;
    query = bson_new ();
    //BSON_APPEND_UTF8 (query, "name", name);
    BSON_APPEND_INT32 (query, "feature_index", id);
    if (!mongoc_collection_remove (collection, MONGOC_REMOVE_NONE, query, NULL, &error_del)){
                      fprintf (stderr, "Delete failed: %s\n", error_del.message);
                       return -1;
    }
    sync_featurelist(featurelist,id);
    bson_destroy (query);
    return 0;
 }
 int delete_all_match_iterm(const char* name,int id,std::vector<databaseinfo> &personinfolist){
    bson_error_t error_del;
    bson_t *query;
    query = bson_new ();
    //BSON_APPEND_UTF8 (query, "name", name);
    BSON_APPEND_INT32 (query, "id_p", id);
    sync_peopleinfolist(personinfolist,id);
    if (!mongoc_collection_remove (collection, MONGOC_REMOVE_NONE, query, NULL, &error_del)){
        fprintf (stderr, "Delete failed: %s\n", error_del.message);
        return -1;
    }
    bson_destroy (query);
    return 0;
 }
 int clear_database(){
    bson_error_t error_del;
    bson_t *doc;
    doc = bson_new ();
    if (!mongoc_collection_remove (collection, MONGOC_REMOVE_NONE, doc, NULL, &error_del)){
        fprintf (stderr, "Delete failed: %s\n", error_del.message);
        return -1;
    }
    bson_destroy (doc);
    return 0;
 }
 int drop_database(){
  bson_error_t *error_drop;
  if (!mongoc_collection_drop(collection,error_drop)){
       fprintf (stderr, "drop_database failed: %s\n", *error_drop->message);
        return -1;
    }
    return 0;
 }
//update
 int update_db_name(const char* in_name,int id,int age){
   bson_error_t error_update;
   bson_t *query;
   bson_t *update;

   query = BCON_NEW ("name", BCON_UTF8(in_name),
                     "id_p", BCON_INT32(id));

   update = BCON_NEW ("$set", "{","age", BCON_INT32(age), 
                                    "updated", BCON_BOOL (true),
                               "}");
    if (!mongoc_collection_update (collection, MONGOC_UPDATE_MULTI_UPDATE, query, update, NULL, &error_update)) {
        fprintf (stderr, "%s\n", error_update.message);
    }
   bson_destroy (query);
   bson_destroy (update);

  return 0;
 }
 int update_db_feature(uint8_t* feature,int datalen,int feature_index,int update_age,const char* name,int id,string &Oidstrtmp){
   bson_error_t error_update;
   bson_t *query;
   bson_t *update;
   mongoc_cursor_t *cursor;
   const bson_t *doc;
   bson_t *Array;
   char strOid[25];
   std::vector<string> v_list;
   Array = bson_new (); 
   query = BCON_NEW("name", BCON_UTF8 (name),
                    "id_p",BCON_INT32(id)); 
   cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
   while (mongoc_cursor_next (cursor, &doc)) {
        bson_iter_t iter;
        bson_iter_init(&iter,doc);
        GetBinary_list(iter,v_list,"NULL");
    }
    insert_binary_info(feature,datalen,feature_index,strOid);
    Oidstrtmp=strOid;
    v_list.push_back(Oidstrtmp);

    for(int i= 0;i<v_list.size();i++){
      char array_index[128];
      sprintf(array_index,"%d",i);
      BSON_APPEND_UTF8(Array,array_index, v_list[i].c_str());
     }
     int feature_size = (int)(v_list.size());
    update = BCON_NEW ("$set", "{","feature", BCON_ARRAY(Array),
                                   "feature_size",BCON_INT32(feature_size),
                                   "age",BCON_INT32(update_age), 
                                    "updated", BCON_BOOL (true),
                               "}");
    if (!mongoc_collection_update (collection, MONGOC_UPDATE_MULTI_UPDATE, query, update, NULL, &error_update)) {
        fprintf (stderr, "%s\n", error_update.message);
    }
   bson_destroy (query);
   bson_destroy (Array);
   bson_destroy (update);
   mongoc_cursor_destroy (cursor);

  return 0;
 }
 //init database  
void init(){
    mongoc_init();
    client = mongoc_client_new ("mongodb://localhost:27017");
    database = mongoc_client_get_database (client, "face_db");
    collection = mongoc_client_get_collection (client, "face_db", "coll");
}
void clean_up()
{
   mongoc_collection_destroy (collection);
   mongoc_database_destroy (database);
   mongoc_client_destroy (client);
   mongoc_cleanup ();
}
