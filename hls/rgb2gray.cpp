#include"rgb2gray.h"

void rgb2gray(ap_uint<8>* in1,ap_uint<8>* in2,ap_uint<8>* in3,
		hls::stream<pix_t> &gray,int h,int w,int color){
    //GRAY==0.3*R+0.59*G+0.11*B
	if(color){
		    for(int i=0;i<h*w;i++){
#pragma HLS PIPELINE
			     fdata_t R=*(in1+i);
			     fdata_t G=*(in2+i+h*w);
			     fdata_t B=*(in3+i+2*h*w);
			     ap_uint<8> GRAY=(ap_uint<8>)((fdata_t)0.3*R+(fdata_t)0.59*G+(fdata_t)0.11*B);
			     gray<<GRAY;
			}
	 }
	else{
		    for(int i=0;i<h*w;i++){
#pragma HLS PIPELINE
			     ap_uint<8> pix=*(in1+i);
			     gray<<pix;
		    }
	 }
}
