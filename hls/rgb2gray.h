#include<ap_int.h>
#include<hls_stream.h>
#include<ap_fixed.h>
typedef ap_uint<8> pix_t;
typedef ap_fixed<32,16,AP_RND,AP_SAT> fdata_t;


void rgb2gray(ap_uint<8>* in1,ap_uint<8>* in2,ap_uint<8>* in3,
		hls::stream<pix_t> &gray,int h,int w,int color);
