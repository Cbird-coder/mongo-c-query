/**
*Copyright (C) 2017
*create by Caiyunlong
*email:caiyunlong2012@gmail.com
**/

#include "image_process.h"
#include "Mat2Binary.h"

int get_facefeature_size(Mat img,int &img_size){
    img_size = img.elemSize() * img.total() + 3*sizeof(int);
    if (img_size == 0){
    	return -1;
    }
    return 0;
}
int get_facefeature(Mat image,uint8_t *data_out){
   return Mat2binary(image,data_out);
}
int restore_facefeature(uint8_t *feature,Mat &img){
	return binary2Mat(feature,img);
}