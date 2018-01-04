/** ******************** *******
**auther:caiyunlong.2017
*******************************/

#ifndef MONGODB_H_
#define MONGODB_H_
#include "db_common.h"
void init();
void clean_up();

/* add bson document to database */
int insert_iterm_info(const char* name,const char* gender,int age,vector<string> feature,int id_seq);
int insert_binary_info(uint8_t* feature,int datalen,int index,char *strOid);
/* delete bson document from database*/
int delete_feature(const char* name,int id,std::vector<feat_datasheet> &featurelist);
int delete_all_match_iterm(const char* name,int id,std::vector<databaseinfo> &peosoninfolist);
int clear_database();
int drop_database();//delete database from mongodb
/*update some iterm in database*/
int update_db_name(const char* in_name,int id,int age);
int update_db_feature(uint8_t* feature,int datalen,int feature_index,int update_age,const char* name,int id,string &Oidstrtmp);
/*look up database,and find out the iterm*/
int query_fearture(std::vector<feat_datasheet> &featurelist);
int query_pesoninfo(std::vector<databaseinfo> &peosoninfolist);
int lookup_db_name_exist(const char *name,int id,int &min_age);
int lookup_db_person_count(int &max_id);
int lookup_db_name_repeat(const char *name,int id);
int lookup_db_by_id(int id,databaseinfo &person);
#endif
