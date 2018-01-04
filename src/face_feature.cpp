/** ******************** *******
**auther:caiyunlong.2017
*******************************/
#include "face_feature.h"
int featureDB::face_feature_get_person_count(int &max_id)
{
    return lookup_db_person_count(max_id);
}
int featureDB::face_feature_add_db(person_info person,
                                   Mat feature_input,
                                   std::vector<feat_datasheet> &featurelist,
                                   std::vector<databaseinfo> &personinfolist)
{
    int namecounter = 0;
    int feature_size=0;
    char strOid[25];
    std::vector<string> Oid_str;
    feat_datasheet add_datasheet;
    databaseinfo persontmp;
    string oid_tmp;

    if(lookup_db_name_exist(person.name.c_str(),person.id_seq,min_age)>0)
    {
        namecounter = lookup_db_name_repeat(person.name.c_str(),person.id_seq);
        if (namecounter >= MAX_SAME_FEATURE){
                return 2;
        }
    }
    if(get_facefeature_size(feature_input,feature_size)<0){
        cout<<"feature size is error"<<endl;
        return -1;
    }
    uint8_t *feature = (uint8_t*)malloc(feature_size);
    if(get_facefeature(feature_input,feature)<0){
        cout << "get feature error "<<endl;
        return -1;
    }
    if(lookup_db_name_exist(person.name.c_str(),person.id_seq,min_age) == 0)
    {
        cout<<"insert people"<<endl;
        persontmp.name = person.name.c_str();
        persontmp.gender = person.gender.c_str();
        persontmp.id_p = person.id_seq;
        persontmp.age = person.age;

        feature_index = person.id_seq;
        insert_binary_info(feature,feature_size,feature_index,strOid);
        oid_tmp = strOid;
        Oid_str.push_back(oid_tmp);
        insert_iterm_info(person.name.c_str(),person.gender.c_str(),person.age,
                                         Oid_str,person.id_seq);
        personinfolist.push_back(persontmp);
    }else{
        cout << "update people"<<endl;
        feature_index = person.id_seq;
        if (min_age > person.age){
            persontmp.name = person.name.c_str();
            persontmp.gender = person.gender.c_str();
            persontmp.id_p = person.id_seq;

            min_age = person.age;
            personinfolist.pop_back();
            persontmp.age = person.age;
            personinfolist.push_back(persontmp);
        }
        update_db_feature(feature,feature_size,feature_index,min_age,person.name.c_str(),person.id_seq,oid_tmp);
    }
    add_datasheet.Oid =  oid_tmp;
    add_datasheet.feature_index = feature_index;
    add_datasheet.feature = feature_input;
    featurelist.push_back(add_datasheet);

    free(feature);
    return 0;
}
int featureDB::face_feature_update_age_db(person_info person,int age){
      update_db_name(person.name.c_str(),person.id_seq,age);
      return 0;
}
int featureDB::face_feature_getPersonInfo(std::vector<databaseinfo> &personinfolist)
{
    query_pesoninfo(personinfolist);
    return 0;
}
int featureDB::face_feature_getFeature(std::vector<feat_datasheet> &featurelist)
{
    query_fearture(featurelist);
    return 0;
}
int featureDB::face_feature_query_db(int id,databaseinfo &result){
    lookup_db_by_id(id,result);
    return 0;
}
void featureDB::face_feature_delete_db( person_info person,
                                        std::vector<feat_datasheet> &featurelist,
                                        std::vector<databaseinfo> &personinfolist)
{
    delete_feature(person.name.c_str(),person.id_seq,featurelist);
    delete_all_match_iterm(person.name.c_str(),person.id_seq,personinfolist);
}
void featureDB::face_feature_clear_db(){
    clear_database();
}
void featureDB::face_feature_drop_db(){
    drop_database();
}
featureDB::featureDB(){
    init();
    feature_index = 0;
    min_age = 100;
}
featureDB::~featureDB(){
    clean_up();
}
