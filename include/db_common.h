/**
*Copyright (C) 2017
*create by Caiyunlong
*email:caiyunlong2012@gmail.com
**/
#ifndef _DB_COMMON_
#define _DB_COMMON_
//CXX STL
#include <iostream>
#include <vector>
#include <string>
//mongodb
#include <bson.h>
#include <bcon.h>
#include <mongoc.h>
//C
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;

typedef struct databaseinfo
{
    string gender;
    int id_p;
    int age;
    int datalen;
    vector<Mat> feature;
} databaseinfo;
#endif //end _DB_COMMON_
