#include"top.h"

void write_back(hls::stream<pix_t> &grad,ap_uint<8>* out,int h,int w,int p){
	int out_h=h+2*p-2;
	int out_w=w+2*p-2;
	ap_uint<8> g;
	for(int i=0;i<out_h*out_w;i++){
#pragma HLS PIPELINE
		   grad>>g;
		   *(out+i)=g;
	}
}

void top_sobel(ap_uint<8>* in1,ap_uint<8>* in2,ap_uint<8>* in3,int h,int w,int p,int color,ap_uint<8>* out){
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL
#pragma HLS INTERFACE m_axi depth=1296 port=out offset=slave bundle=OUT
#pragma HLS INTERFACE s_axilite port=h bundle=CTRL
#pragma HLS INTERFACE s_axilite port=color bundle=CTRL
#pragma HLS INTERFACE s_axilite port=w bundle=CTRL
#pragma HLS INTERFACE s_axilite port=p bundle=CTRL
#pragma HLS INTERFACE m_axi depth=3888 port=in3 offset=slave bundle=IN3
#pragma HLS INTERFACE m_axi depth=2592 port=in2 offset=slave bundle=IN2
#pragma HLS INTERFACE m_axi depth=1296 port=in1 offset=slave bundle=IN1
	hls::stream<pix_t> gray;
#pragma HLS STREAM variable=gray depth=256 dim=1
	hls::stream<pix_t> grad;
#pragma HLS STREAM variable=grad depth=256 dim=1
	//
#pragma HLS DATAFLOW
	rgb2gray(in1,in2,in3,gray,h,w,color);
	linebuffer_sobel(gray,grad,h,p);
	write_back(grad,out,h,w,p);
}
