#include<ap_int.h>
#include<hls_stream.h>
#include<iostream>
#define K 3
#include"rgb2gray.h"
using namespace std;

void linebuffer_sobel(hls::stream<pix_t> &FeatureIn,hls::stream<pix_t> &FeatureOut,int N,int P);
