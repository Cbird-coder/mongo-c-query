/** ******************** *******
**auther:caiyunlong.2017.06.29
**email:ylcai@miivii.com
*******************************/
#include "Mat2Binary.h"
//interface for other
int Mat2binary(Mat out_mat,uint8_t *mat_binary) {
    if( out_mat.empty() ) {
        cout << "load image wrong..."<<endl;    
        return -1;
    }
    int type = out_mat.type();
    memcpy(mat_binary,(uint8_t *)(&type),sizeof(int));
    memcpy(mat_binary+sizeof(int),(uint8_t *)(&out_mat.rows),sizeof(int));
    memcpy(mat_binary+2*sizeof(int),(uint8_t *)(&out_mat.cols),sizeof(int));
    memcpy(mat_binary+3*sizeof(int),(uint8_t*)(out_mat.data),out_mat.elemSize() * out_mat.total());
    return 0;
}
int binary2Mat(uint8_t *binary_mat,cv::Mat &mat_out)
{
    int rows=0,cols=0,type=0;

    memcpy(&type,binary_mat,sizeof(int));
    memcpy(&rows,binary_mat+sizeof(int),sizeof(int));
    memcpy(&cols,binary_mat+2*sizeof(int),sizeof(int));
    if (type ==0 || cols == 0 || rows==0){
        return -1;
    }
    mat_out.release();
    mat_out.create(rows,cols,type);
    memcpy(mat_out.data,binary_mat+3*sizeof(int),mat_out.elemSize() * mat_out.total());
    return 0;
}