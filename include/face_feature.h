/** ******************** *******
**auther:caiyunlong.2017
*******************************/
#ifndef FACEFEATURE_
#define FACEFEATURE_
#include "mongdb_face.h"
#include "image_process.h"

#define MAX_SAME_FEATURE 50
typedef struct person_info {
		string name;
		int age;
		string gender;
		int id_seq;
}person_info;

class featureDB
{
  int feature_index;
  int min_age;
public:
	/*
      * construction function.
    */
	featureDB();
	/*
      * release function.
    */
	~featureDB();
  /*
  *  get total person number
  */
  int face_feature_get_person_count(int &max_id);
	/*
      * add people's information to database.
      * the input :id_seq,name,gender,age,feature
    */
	int face_feature_add_db(person_info person,Mat feature_input,std::vector<feat_datasheet> &featurelist,
                                                                  std::vector<databaseinfo> &personinfolist);
  /*
  *****
  */
  int face_feature_getFeature(std::vector<feat_datasheet> &featurelist);
  int face_feature_getPersonInfo(std::vector<databaseinfo> &personinfolist);
	/*
      * use name,query people's information from database
    */
	int face_feature_query_db(int id,databaseinfo &result);
  /*
  *update database
  */
  int face_feature_update_age_db(person_info input,int age);
	/*
      * delete people's information  by people'name.
      *input:name
    */
	void face_feature_delete_db(person_info person,std::vector<feat_datasheet> &featurelist,
                                                    std::vector<databaseinfo> &personinfolist);
  /*
  *clear database
  */
  void face_feature_clear_db();
  /*
      * database delete
    */
  void face_feature_drop_db();
};
#endif
