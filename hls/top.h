#include"linebuffer_sobel.h"
#include"rgb2gray.h"
typedef ap_uint<8> pix_t;
void top_sobel(ap_uint<8>* in1,ap_uint<8>* in2,ap_uint<8>* in3,int h,int w,int p,int color,ap_uint<8>* out);
