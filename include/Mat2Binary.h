/** ******************** *******
**auther:caiyunlong.2017.06.29
**email:ylcai@miivii.com
*******************************/

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
