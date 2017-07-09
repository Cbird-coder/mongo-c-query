/**
*Copyright (C) 2017
*create by Caiyunlong
*email:caiyunlong2012@gmail.com
**/

#ifndef _MATTOSTR_H_H
#define _MATTOSTR_H_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;
int Mat2binary(cv::Mat image,uint8_t *data_out);
int binary2Mat(uint8_t *feature,cv::Mat &mat_out);
    
#endif //_MATTOSTR_H_H
