/**
*Copyright (C) 2017
*create by Caiyunlong
*email:caiyunlong2012@gmail.com
**/
#ifndef FACEFEATURE_
#define FACEFEATURE_
#include "mongdb_face.h"
#include "image_process.h"

#define MAX_SAME_FEATURE 20

typedef struct personinput
{
  int id_seq;
  string name;
  string gender;
  int age;
} personinput;

class featureDB
{
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
  int face_feature_get_person_count();
	/*
       * get all the people's names in the database.
       * the result is saved in namelist vector 
    */
	void face_feature_getnamelist_db(std::vector<std::string> &namelist);
	/*
      * add people's information to database.
      * the input :id_seq,name,gender,age,feature
    */
	int face_feature_add_db(personinput input,Mat feature_input);
	/*
      * use name,query people's information from database
      * the input : name,for query.
      * the output : age,gender,feature_output
    */
	int face_feature_query_db(personinput input,databaseinfo &result);
  /*
  *update database
  */
  int face_feature_update_age_db(personinput input,int age);
  int face_feature_update_feature(personinput person,Mat feature);
	/*
      * delete people's information  by people'name.
      *input:name
    */
	void face_feature_delete_db(personinput input);
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