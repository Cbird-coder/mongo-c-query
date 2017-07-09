/**
*Copyright (C) 2017
*create by Caiyunlong
*email:caiyunlong2012@gmail.com
**/
#include "face_feature.h"
int featureDB::face_feature_get_person_count()
{
    return lookup_db_person_count();
}
void featureDB::face_feature_getnamelist_db(std::vector<std::string> &namelist){
    lookup_db_get_name_list(namelist);
}
int featureDB::face_feature_add_db(personinput person,Mat feature_input){
    int namecounter = 0;
    int feature_size=0;
    char strOid[25];
    std::vector<string> Oid_str;

    if(lookup_db_name_exist(person.name.c_str(),person.id_seq)>0)
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
    if(lookup_db_name_exist(person.name.c_str(),person.id_seq) == 0)
    {
        cout<<"insert people"<<endl;
        insert_binary_info(feature,feature_size,strOid);
        string tmp;
        tmp = strOid;
        Oid_str.push_back(tmp);
        insert_iterm_info(person.name.c_str(),person.gender.c_str(),person.age,
                                         Oid_str,person.id_seq);
    }else{
        cout << "update people"<<endl;
        update_db_feature(feature,feature_size,person.name.c_str(),person.id_seq);
    }
    return 0;
}
int featureDB::face_feature_update_feature(personinput person,Mat feature){
    int feature_update_len = 0;
    if(get_facefeature_size(feature,feature_update_len)<0){
        cout<<"feature size is error"<<endl;
        return -1;
    }
    uint8_t *feature_bin = (uint8_t*)malloc(feature_update_len);
    if(get_facefeature(feature,feature_bin)<0){
        cout << "get feature error "<<endl;
        return -1;
    }
    update_db_feature(feature_bin,feature_update_len,person.name.c_str(),person.id_seq);
    free(feature_bin);
    return 0;
}
int featureDB::face_feature_update_age_db(personinput person,int age){
      update_db_name(person.name.c_str(),person.id_seq,age);
      return 0;
}
int featureDB::face_feature_query_db(personinput person,databaseinfo &result){
    lookup_db_by_name(person.name.c_str(),person.id_seq,result);
    return 0;
}
void featureDB::face_feature_delete_db(personinput person){
     delete_all_match_iterm(person.name.c_str(),person.id_seq);
}
void featureDB::face_feature_clear_db(){
    clear_database();
}
void featureDB::face_feature_drop_db(){
    drop_database();
}
featureDB::featureDB(){
    init();
}
featureDB::~featureDB(){
    clean_up();
}