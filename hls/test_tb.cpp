#include<iostream>
#include"linebuffer_sobel.h"
#include"top.h"
#define H 36
#define W 36
#define N H
#define MAX_LEN 10000
using namespace std;


ap_uint<16> baseline_abs(ap_int<16> x){
    if(x>=0)
    	return (ap_uint<16>)x;
    else
    	return (ap_uint<16>)(-x);
}

void sobel_baseline(ap_uint<8>* in,int h,int w,ap_uint<8>* out){
	  const ap_int<8> Gx[K][K]={{-1,0,1},{-2,0,2},{-1,0,1}};
	  const ap_int<8> Gy[K][K]={{1,2,1},{0,0,0},{-1,-2,-1}};
      int h_o=h-K+1;
      int w_o=w-K+1;
      for(int i=0;i<h_o;i++)
    	  for(int j=0;j<w_o;j++){
              ap_int<16> tmp1=0;
              ap_int<16> tmp2=0;
              for(int r=0;r<K;r++)
            	  for(int c=0;c<K;c++){
            		   tmp1+=in[(i+r)*w+(j+c)]*Gx[r][c];
            		   tmp2+=in[(i+r)*w+(j+c)]*Gy[r][c];
            	  }
              out[i*w_o+j]=baseline_abs(tmp1)+baseline_abs(tmp2);
    	  }
}

void rgb2gray_baseline(ap_uint<8>* in,ap_uint<8>* out,int h,int w){
	for(int i=0;i<h*w;i++){
		fdata_t R=(fdata_t)(*(in+i));
		fdata_t G=(fdata_t)(*(in+i+h*w));
		fdata_t B=(fdata_t)(*(in+i+2*h*w));
		fdata_t Gray=(fdata_t)0.3*R+(fdata_t)0.59*G+(fdata_t)0.11*B;
		*(out+i)=(ap_uint<8>)Gray;
	}
}

void top_baseline(ap_uint<8>* in,ap_uint<8>* out,int h,int w,int color){
	ap_uint<8> gray[MAX_LEN];
	if(color==1){
		//三通道彩色图像
		rgb2gray_baseline(in,gray,h,w);
		sobel_baseline(gray,h,w,out);
	}
	else{
		  sobel_baseline(in,h,w,out);
	}
}

int main(){
	ap_uint<8> color_img[3][H][W];
	ap_uint<8> gray_img[H][W];
	ap_uint<8> grad1[H-K+1][W-K+1];
	ap_uint<8> grad2[H-K+1][W-K+1];
	//init
	for(int i=0;i<H;i++)
		for(int j=0;j<W;j++)
			for(int n=0;n<3;n++){
				color_img[n][i][j]=rand()%256;
			}
	for(int i=0;i<H;i++){
		for(int j=0;j<W;j++){
			gray_img[i][j]=rand()%256;
		}
	}
	//running
	int color=1;
	if(color==1){
		top_baseline((ap_uint<8>*)color_img,(ap_uint<8>*)grad1,H,W,color);
		top_sobel((ap_uint<8>*)color_img,(ap_uint<8>*)color_img,(ap_uint<8>*)color_img,
				         H,W,0,color,(ap_uint<8>*)grad2);
	}
	else{
		top_baseline((ap_uint<8>*)gray_img,(ap_uint<8>*)grad1,H,W,color);
	    top_sobel((ap_uint<8>*)gray_img,(ap_uint<8>*)gray_img,(ap_uint<8>*)gray_img,
	    		         H,W,0,color,(ap_uint<8>*)grad2);
	}
	//check result
	for(int i=0;i<H-K+1;i++)
		for(int j=0;j<W-K+1;j++){
			if(grad1[i][j]!=grad2[i][j])
				return -1;
			cout<<grad1[i][j]<<","<<grad2[i][j]<<endl;
		}
	return 0;
}
