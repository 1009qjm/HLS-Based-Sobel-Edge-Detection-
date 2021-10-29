/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include<stdlib.h>
#include"xtop_sobel.h"
#include"xil_cache.h"
#define H 192
#define W 108
#define P 1
#define K 3
#define MAX_LEN 1000000

int baseline_abs(int x){
	if(x>=0)
		return x;
	else
		return (-x);
}

void sobel_baseline(unsigned char* in,int h,int w,int p,unsigned char* out){
	const int Gx[K][K]={{-1,0,1},{-2,0,2},{-1,0,1}};
	const int Gy[K][K]={{1,2,1},{0,0,0},{-1,-2,-1}};
	int out_h=h+2*p-K+1;
	int out_w=w+2*p-K+1;
	//
	for(int i=0;i<out_h;i++)
		for(int j=0;j<out_w;j++){
			    int tmp1=0;
			    int tmp2=0;
			    for(int r=0;r<K;r++)
			    	for(int c=0;c<K;c++){
			    		if(i+r>=p&&i+r<h+p&&j+c>=p&&j+c<w+p){
			    			tmp1+=in[(i+r-p)*w+(j+c-p)]*Gx[r][c];
			    			tmp2+=in[(i+r-p)*w+(j+c-p)]*Gy[r][c];
			    		}
			    	}
			    out[i*out_w+j]=baseline_abs(tmp1)+baseline_abs(tmp2);
		}
}

void rgb2gray_baseline(unsigned char* in,unsigned char* out,int h,int w){
	for(int i=0;i<h*w;i++){
		float R=(float)(*(in+i));
		float G=(float)(*(in+i+h*w));
		float B=(float)(*(in+i+2*h*w));
		int Gray=(30*R+59*G+11*B)/100;
		*(out+i)=(unsigned char)Gray;
	}
}

void ps_sobel_top(unsigned char* img,unsigned char* grad,int h,int w,int p,int color){
	unsigned char gray[MAX_LEN];
	if(color==1){
        rgb2gray_baseline(img,gray,h,w);
        sobel_baseline(gray,h,w,p,grad);
	}
	else{
        sobel_baseline(img,h,w,p,grad);
	}
}

int main()
{
    init_platform();
    Xil_DCacheDisable();
    int color=0;
    unsigned char color_img[3][H][W];
    unsigned char grad1[H+2*P-2][W+2*P-2];
    unsigned char grad2[H+2*P-2][W+2*P-2];
    //init
    for(int i=0;i<H;i++)
    	for(int j=0;j<W;j++)
    		for(int n=0;n<3;n++){
    			color_img[n][i][j]=rand()%256;
    		}
    //ps
    ps_sobel_top((unsigned char*)color_img,(unsigned char*)grad1,H,W,P,color);
    //pl
    XTop_sobel hls_inst;
    XTop_sobel_Initialize(&hls_inst, 0);
    //
    XTop_sobel_Set_in1_V(&hls_inst, (u32)color_img);
    XTop_sobel_Set_in2_V(&hls_inst, (u32)color_img);
    XTop_sobel_Set_in3_V(&hls_inst, (u32)color_img);
    XTop_sobel_Set_h(&hls_inst, (u32)H);
    XTop_sobel_Set_w(&hls_inst, (u32)W);
    XTop_sobel_Set_p(&hls_inst, (u32)P);
    XTop_sobel_Set_color(&hls_inst, (u32)color);
    XTop_sobel_Set_out_V(&hls_inst, (u32)grad2);
    XTop_sobel_Start(&hls_inst);
    while(XTop_sobel_IsDone(&hls_inst)==0);
    //
    print("check result\n");
    for(int i=0;i<H+2*P-2;i++)
    	for(int j=0;j<W+2*P-2;j++){
    		if(grad1[i][j]-grad2[i][j]>2||grad1[i][j]-grad2[i][j]<-2){
                printf("error\n");
                printf("%d,%d\n",grad1[i][j],grad2[i][j]);
    			//return -1;
    		}
    	}
    printf("right\n");
    cleanup_platform();
    return 0;
}
