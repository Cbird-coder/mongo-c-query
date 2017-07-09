/** ******************** *******
**auther:caiyunlong.2017.06.29
**email:ylcai@miivii.com
*******************************/
#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <opencv2/highgui/highgui.hpp>
using namespace cv;

int get_facefeature_size(Mat img,int &img_size);
int get_facefeature(Mat image,uint8_t *data_out);
int restore_facefeature(uint8_t *feature,Mat &img);
#endif //_IMAGE_H_