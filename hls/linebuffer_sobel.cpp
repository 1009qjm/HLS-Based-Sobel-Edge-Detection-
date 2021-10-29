#include"linebuffer_sobel.h"

ap_uint<16> abs(ap_int<16> x){
	if(x>=0)
		return (ap_uint<16>)x;
	else{
		return (ap_uint<16>)(-x);
	}
}

void linebuffer_sobel(hls::stream<pix_t> &FeatureIn,hls::stream<pix_t> &FeatureOut,int N,int P){
#pragma HLS DATAFLOW
	//
	const ap_int<8> Gx[K][K]={{-1,0,1},{-2,0,2},{-1,0,1}};
	const ap_int<8> Gy[K][K]={{1,2,1},{0,0,0},{-1,-2,-1}};
	//
	hls::stream<pix_t> Line1;
#pragma HLS STREAM variable=Line1 depth=2000 dim=1
	hls::stream<pix_t> Line2;
#pragma HLS STREAM variable=Line2 depth=2000 dim=1
	ap_uint<8> Window[K][K];
#pragma HLS ARRAY_PARTITION variable=Window complete dim=0
	ap_uint<8> pixIn,Line1Rd,Line2Rd;
	int LEN1,LEN2;
	LEN1=0;
	LEN2=0;
	int start_index=2*N+4*P+K-1;
	for(int i=0;i<(N+2*P)*(N+2*P);i++){
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=900 max=900 avg=900
		//����PADDING����
		int row=i/(N+2*P);
		int col=i%(N+2*P);
		if(row>=P&&row<N+P&&col>=P&&col<N+P)
			FeatureIn>>pixIn;
		else
			pixIn=0;
		//LineBuffer
		if(LEN1==(N+2*P)){
			Line1>>Line1Rd;
		    LEN1-=1;
		}
		if(LEN2==(N+2*P)){
			Line2>>Line2Rd;
		    LEN2-=1;
		}
		Line1<<pixIn;
		Line2<<Line1Rd;
		LEN1+=1;
		LEN2+=1;
		//Window
		Window[0][0]=Window[0][1];
		Window[0][1]=Window[0][2];
		Window[0][2]=Line2Rd;
		Window[1][0]=Window[1][1];
		Window[1][1]=Window[1][2];
		Window[1][2]=Line1Rd;
		Window[2][0]=Window[2][1];
		Window[2][1]=Window[2][2];
		Window[2][2]=pixIn;
		//sobel计算
		ap_int<16> sumx;
		ap_int<16> sumy;
		ap_uint<8> out;
		sumx=0;
		sumy=0;
		if(i>=start_index&&(i-start_index)%N<(N-K+1)){
			for(int i=0;i<K;i++)
#pragma HLS UNROLL
				for(int j=0;j<K;j++){
#pragma HLS UNROLL
					sumx+=Window[i][j]*Gx[i][j];
					sumy+=Window[i][j]*Gy[i][j];
				}
			out=abs(sumx)+abs(sumy);
			FeatureOut<<out;
		}
		//print
		if(i>=start_index&&(i-start_index)%(N+2*P)<N){
			cout<<"i="<<i<<endl;
			for(int kx=0;kx<K;kx++){
				for(int ky=0;ky<K;ky++)
					cout<<Window[kx][ky]<<",";
				cout<<endl;
			}
		}
	}
}
