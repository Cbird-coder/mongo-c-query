/** ******************** *******
**auther:caiyunlong.2017.06.29
**email:ylcai@miivii.com
*******************************/

#ifndef MONGODB_H_
#define MONGODB_H_
#include "db_common.h"
void init();
void clean_up();

/* add bson document to database */
int insert_iterm_info(const char* name,const char* gender,int age,vector<string> feature,int id_seq);
int insert_binary_info(uint8_t* feature,int datalen,char *strOid);
/* delete bson document from database*/
int delete_all_match_iterm(const char* name,int id);
int delete_one_match_iterm(const char* name,int id);
int clear_database();
int drop_database();//delete database from mongodb
/*update some iterm in database*/
int update_db_name(const char* in_name,int id,int age);
int update_db_feature(uint8_t* feature,int datalen,const char* name,int id);
/*look up database,and find out the iterm*/
int lookup_db_name_exist(const char *name,int id);
int lookup_db_person_count();
int lookup_db_get_name_list(std::vector<string> &names);
int lookup_db_name_repeat(const char *name,int id);
int lookup_db_by_name(const char* name,int id,databaseinfo &persons);
#endif
