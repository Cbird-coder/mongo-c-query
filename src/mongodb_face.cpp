/** ******************** *******
**auther:caiyunlong.2017.06.29
**email:ylcai@miivii.com
*******************************/
#include "db_common.h"
#include "image_process.h"

void debug_bson(bson_t  *insert)
{    
    char *str;
    str = bson_as_json (insert, NULL);
    printf ("BSON :%s\n", str);
    bson_free (str);
 }


mongoc_database_t    *database;
mongoc_collection_t  *collection;
mongoc_client_t      *client;
//use in inner function
int GetBinary_list( bson_iter_t iter,std::vector<string> &feature_list)
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
            feature_list.push_back(feature_name);
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
        printf("%s|",bson_iter_utf8(&iter2,&length));
    }
    if(bson_iter_find_descendant(&iter,"gender",&iter2))
    {
      people.gender= bson_iter_utf8(&iter2,&length);
      printf("%s|",people.gender.c_str());
    }
    if(bson_iter_find(&iter, "age"))
    {
        people.age = bson_iter_int32(&iter);
        printf("%d|",people.age);
    }
    if(bson_iter_find(&iter, "id_p"))
    {
        people.id_p = bson_iter_int32(&iter);
        printf("%d|",people.id_p);
        printf("\n");
    }
   std::vector<string> feature_list;
   GetBinary_list(iter,feature_list);
   for (int i = 0; i < feature_list.size(); ++i)
   {
       bson_oid_t oid;
       const bson_t *doc_feat;
       mongoc_cursor_t *cursor;
       bson_t *query;
       bson_oid_init_from_string (&oid, feature_list[i].c_str());
       query = BCON_NEW ("_id", BCON_OID(&oid));
       cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
        while (mongoc_cursor_next (cursor, &doc_feat)) 
        {
            bson_iter_t iter_feat;
            bson_iter_init(&iter_feat,doc_feat);
            if (bson_iter_find(&iter_feat, "feature_Mat"))
            {
              const uint8_t *binary = NULL;
              Mat feature;
              bson_subtype_t subtype = BSON_SUBTYPE_BINARY;

              uint32_t binary_len = 0;
              bson_iter_binary(&iter_feat, &subtype, &binary_len, &binary);
              uint8_t *tempbuffer = (uint8_t*)malloc(binary_len);
              memcpy(tempbuffer,binary,binary_len);
              if(restore_facefeature(tempbuffer,feature) < 0){
                printf("wrong feature type");
                return -1;
              }
              people.feature.push_back(feature);
              free(tempbuffer);
            }
          }
        bson_destroy (query);
        mongoc_cursor_destroy (cursor);
    }
    return 0;
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
    insert = BCON_NEW ("name", BCON_UTF8 (name),
                       "gender",BCON_UTF8(gender),
                       "age",BCON_INT32(age),
                       "id_p",BCON_INT32(id_seq),
                       "feature_size",BCON_INT32(feature_size)
    );
    BSON_APPEND_ARRAY(insert,"feature", Array);
    if (!mongoc_collection_insert (collection, MONGOC_INSERT_NONE, insert, NULL, &error_insert)) {
      fprintf (stderr, "%s\n", error_insert.message);
   }
   bson_destroy (insert);
   bson_destroy (Array);
   return 0;
}
int insert_binary_info(uint8_t* feature,int datalen,char *strOid){
  bson_t    *insert;
  bson_oid_t oid;
  bson_iter_t iter;
  bson_error_t error_insert; 


  bson_oid_init (&oid, NULL);
  insert = BCON_NEW ("_id", BCON_OID (&oid));
  bson_oid_to_string(&oid,strOid);
  cout << "oid:"<<strOid<<endl;
  BSON_APPEND_BINARY(insert,"feature_Mat",BSON_SUBTYPE_BINARY,feature,datalen);
  if (!mongoc_collection_insert (collection, MONGOC_INSERT_NONE, insert, NULL, &error_insert)) {
      fprintf (stderr, "%s\n", error_insert.message);
   }
   bson_destroy (insert);
   return 0;
}
/////////////////////////////////////////////////////////////
///////////////////////look up database//////////////////////
int lookup_db_get_name_list(std::vector<string> &namelist)
{
  mongoc_cursor_t *cursor;
  const bson_t *doc;
  bson_t *query;
  int count = 0;

  query =bson_new ();
  cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
  while (mongoc_cursor_next (cursor, &doc)){
    bson_iter_t iter;
    bson_iter_t iter2;
    uint32_t length;
    bson_iter_init(&iter,doc);
    if(bson_iter_find_descendant(&iter,"name",&iter2))
    {
         string persons_name;
         persons_name=bson_iter_utf8(&iter2,&length);
         namelist.push_back(persons_name);
    }
  }
  //delete repeat name
  sort(namelist.begin(), namelist.end());  
  namelist.erase(unique(namelist.begin(),namelist.end()),namelist.end());
  bson_destroy (query);
  mongoc_cursor_destroy (cursor);
  return 0;
}

int lookup_db_person_count()
{
  mongoc_cursor_t *cursor;
  const bson_t *doc;
  bson_t *query;
  int count = 0;

  query =bson_new ();
  cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
  while (mongoc_cursor_next (cursor, &doc)){
      bson_iter_t iter;
      bson_iter_init(&iter,doc);
     if(bson_iter_find(&iter, "id_p")){
      ++count;
    }
  }
  bson_destroy (query);
  mongoc_cursor_destroy (cursor);
  return count;
}
int lookup_db_name_exist(const char *name,int id)
{
   bson_error_t error_lk;  
   bson_t *query;
   int64_t count;

   query = BCON_NEW("name", BCON_UTF8 (name),
                    "id_p",BCON_INT32(id));
   count = mongoc_collection_count (collection, MONGOC_QUERY_NONE, query, 0, 0, NULL, &error_lk);
   if (count < 0) {
      fprintf (stderr, "%s\n", error_lk.message);
   } 
   bson_destroy (query);
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
        GetBinary_list(iter,v_list);
    }
   bson_destroy (query);
   mongoc_cursor_destroy (cursor);

   return (int)(v_list.size()); 
}

 int lookup_db_by_name(const char* name,int id,databaseinfo &persons){
    mongoc_cursor_t *cursor;
    bson_t *query;
    const bson_t *doc;  
    char *str;
    query = bson_new ();
    BSON_APPEND_UTF8 (query, "name", name);
    BSON_APPEND_INT32(query,"id_p",id);
    cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
    while (mongoc_cursor_next (cursor, &doc)) {
        GetRecord(doc,persons);
    }
    bson_destroy (query);
    mongoc_cursor_destroy (cursor);

    return 0;
 }
 //delete
 int delete_all_match_iterm(const char* name,int id){
   bson_error_t error_del;
    bson_t *doc;
    doc = bson_new ();
    BSON_APPEND_UTF8 (doc, "name", name);
    BSON_APPEND_INT32 (doc, "id_p", id);  
    if (!mongoc_collection_remove (collection, MONGOC_REMOVE_NONE, doc, NULL, &error_del)){
        fprintf (stderr, "Delete failed: %s\n", error_del.message);
        return -1;
    }
    bson_destroy (doc);
    return 0;
 }
 int delete_one_match_iterm(const char* name,int id){
    bson_error_t error_del;
    bson_t *doc;
    doc = bson_new ();
    BSON_APPEND_UTF8 (doc, "name", name); 
    BSON_APPEND_INT32 (doc, "id_p", id); 
    if (!mongoc_collection_remove (collection, MONGOC_REMOVE_SINGLE_REMOVE, doc, NULL, &error_del)){
        fprintf (stderr, "Delete failed: %s\n", error_del.message);
        return -1;
    }
    bson_destroy (doc);
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
 int update_db_feature(uint8_t* feature,int datalen,const char* name,int id){
   bson_error_t error_update;
   bson_t *query;
   bson_t *update;
   mongoc_cursor_t *cursor;
   const bson_t *doc;
   bson_t *Array;
   char strOid[25];
   string Oidstrtmp;
   std::vector<string> v_list;
   Array = bson_new (); 
   query = BCON_NEW("name", BCON_UTF8 (name),
                    "id_p",BCON_INT32(id)); 
   cursor = mongoc_collection_find (collection, MONGOC_QUERY_NONE, 0, 0, 0, query, NULL, NULL);
   while (mongoc_cursor_next (cursor, &doc)) {
        bson_iter_t iter;
        bson_iter_init(&iter,doc);
        GetBinary_list(iter,v_list);
    }
    insert_binary_info(feature,datalen,strOid);
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