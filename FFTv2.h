#define __FPU_PRESENT 1U    // 开启FPU

#ifndef __FFTV2_H
#define __FFTV2_H

#include "stdint.h"
#define NPT_max  16384           //最大FFT点数，理论上能开很大，但是要考虑单片机RAM的大小
#define pi 3.1415926

typedef struct Complex
{
	float real; // 实部
	float imag; // 虚部
}Complex; // 复数

typedef struct FFTresult
{
	Complex result[NPT_max];    //FFT的结果，是复数形式
	float	Amp[NPT_max];       //幅值
	float	phase[NPT_max];     //相位
    float   dc;                 //直流分量
}FFTresult; //存放FFT运算结果的结构体


static void FFT(int length, FFTresult* ch);


// 以下函数是用户使用的FFT函数

//FFT初始化，主要是FFT前打表sin，实现加速
void FFT_Init(void);

void FFT_u32(uint32_t* input, int length, FFTresult* ch);
void FFT_f32(float* input, int length, FFTresult* ch);
void FFT_int32(int* input, int length, FFTresult* ch);
void FFT_int16(int16_t* input, int length, FFTresult* ch);
void FFT_win_u32(uint32_t* input, int length, float* win, FFTresult* ch);
void FFT_win_f32(float* input, int length, float* win, FFTresult* ch);
void FFT_win_int32(int* input, int length, float* win, FFTresult* ch);
void FFT_win_int16(int16_t* input, int length, float* win, FFTresult* ch);

void AmpRecovery(FFTresult* ch, int length, float k);


//以下为加窗函数

static int Factorial(int num);//阶乘
static float Besseli(float x);//0阶贝塞尔
float Kaiser(int n, float beta, float* win);//Kaiser窗
float FlattopWin(int n, float* win);//Flattop窗
float Hanning(int n, float* win);//Hanning窗
float Hamming(int n, float* win);//Hamming窗
float rectangle(int n, float* win);//矩形窗

//以下为实现FFT算法用到的函数

static int BitReverse(int input, int N);        // 比特反转
static void DFT(Complex* p, int len);           // DFT，用于递归
Complex Complex_Multi(Complex c1, Complex c2);  // 复数乘法，返回值等于c1 * c2
Complex Complex_Add(Complex c1, Complex c2);    // 复数加法，返回值等于c1 + c2
Complex Complex_Minus(Complex c1, Complex c2);  // 复数减法，返回值等于c1 - c2
float Complex_Modular(Complex c);               // 复数求模
float Complex_arg(Complex c);                   // 复数求辐角

//sin和cos查表

static float Mysin(int len, int i);
static float Mycos(int len, int i);

#endif

