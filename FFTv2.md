
[TOC]

# FFT函数的C语言实现和封装

## 一、序言

&emsp;&emsp;本次封装脱离了ARM官方的DSP库（因为麻烦而且没有高点数的FFT）
&emsp;&emsp;我自己用C语言搓了一个FFT函数，为了实现更方便，空间复杂度更小的FFT。所以本驱动是适用于所有单片机型号的，唯一需要考虑的就是RAM的大小，RAM太小会导致数组开不大

## 二、定义说明

### 1.FPU

```c
#define __FPU_PRESENT 1U    // 开启FPU
```

据说是开启FPU用的，可以提速，但根据实测好像并没有什么提升

### 2.NPT_max

```c
#define NPT_max  16384
```

定义在FFTv2.h中，影响空间大小的主要因素。RAM不够了就把这个改小，但必须是2的整数次方

### 3.复数

```c
typedef struct Complex
{
	float real; // 实部
	float imag; // 虚部
}Complex; // 复数
```

FFT要用到很多复数运算，为了便于组织，以结构体的形式存储复数

### 4.FFT运算结果

```c
typedef struct FFTresult
{
	Complex result[NPT_max];    //FFT的结果，是复数形式
	float	Amp[NPT_max];       //幅值
	float	phase[NPT_max];     //相位
    float   dc;                 //直流分量
}FFTresult; //存放FFT运算结果的结构体
```
FFT的运算结果以复数的形式存储于result数组中
Amp存放result数组的模值
phase存放result数组的相位
dc存放的是直流分量（因为加窗之前要去掉直流，所以在这里记录下直流的大小）

### 5.函数

#### (1)初始化

```c
void FFT_Init(void);
```
初始化函数，实际上只用来sin函数的打表，三角函数的计算很耗时间，查表可以节省很多时间（本驱动的运算时间与ARM官方的FFT运算时间相当）。在单片机上电之后调用一次就好

#### (2)FFT函数

```c
/*
 *输入数据的类型为XXX的FFT
 *input: 输入数据
 *length: FFT点数，必须为2的整数次方
 *ch: FFTresult存放结果的结构体
 */
void FFT_u32(uint32_t* input, int length, FFTresult* ch);
void FFT_f32(float* input, int length, FFTresult* ch);
void FFT_int32(int* input, int length, FFTresult* ch);
void FFT_int16(int16_t* input, int length, FFTresult* ch);
```

FFT函数，为了兼容不同数据类型的输入（比如片上ADC是uint32_t类型，AD7606是int16_t类型），定义了4种函数（实际上这4个函数的内部代码是一样的，只不过我没有想到更好的兼容方法）

#### (3)窗函数生成

```c
float Kaiser(int n, float beta, float* win);//Kaiser窗
float FlattopWin(int n, float* win);//Flattop窗
float Hanning(int n, float* win);//Hanning窗
float Hamming(int n, float* win);//Hamming窗
float rectangle(int n, float* win);//矩形窗
```

生成窗函数到win的地址，返回值是窗函数的恢复系数

#### (4)加窗FFT

```c
/*
 *输入数据的类型为uint32_t的加窗FFT
 *input: 输入数据
 *length: FFT点数，必须为2的整数次方
 *win: 窗函数
 *ch: FFTresult存放结果的结构体
 */
void FFT_win_u32(uint32_t* input, int length, float* win, FFTresult* ch);
void FFT_win_f32(float* input, int length, float* win, FFTresult* ch);
void FFT_win_int32(int* input, int length, float* win, FFTresult* ch);
void FFT_win_int16(int16_t* input, int length, float* win, FFTresult* ch);
```

加窗FFT，win输入窗函数，无幅值恢复，需要额外调用幅值恢复函数

#### (5)幅值恢复

```c
/*
 *恢复加窗后带来的幅值衰减
 *ch: 待恢复幅值的FFT结果
 *length: FFT点数
 *k: 恢复系数，生成窗函数时的返回值就是恢复系数
 */
void AmpRecovery(FFTresult* ch, int length, float k);
```

需要分析幅值的时候调用一下，如果只分析频率就不用管这个

#### (6)复数运算

```c
Complex Complex_Multi(Complex c1, Complex c2);  // 复数乘法，返回值等于c1 * c2
Complex Complex_Add(Complex c1, Complex c2);    // 复数加法，返回值等于c1 + c2
Complex Complex_Minus(Complex c1, Complex c2);  // 复数减法，返回值等于c1 - c2
float Complex_Modular(Complex c);               // 复数求模
float Complex_arg(Complex c);                   // 复数求辐角
```

尽管可能用不到这些运算，为了保证灵活性，还是开放了这些函数的接口

## 三、调用步骤

### 1.不加窗的FFT

#### (1)引用头文件

```c
#include "FFTv2.h"
```

#### (2)定义变量

```c
uint32_t adc_buf[NPT_max];//这里的NPT_max也可以是比它小的常量
FFTresult ch1;//定义FFT结果变量
```

#### (3)初始化（打表提速）

```c
FFT_Init();
```

#### (4)调用FFT

```c
//根据你输入的数据类型选择不同的函数
FFT_u32(adc_buf, NPT, &ch1);
```
### 2.加窗FFT

#### (1)引用头文件

```c
#include "FFTv2.h"
```

#### (2)定义变量

```c
uint32_t adc_buf[NPT_max];//这里的NPT_max也可以是比它小的常量
float win[NPT_max];//定义窗函数，可扩展性体现在这里，你可以自己写窗函数，也可以用我提供的窗函数
FFTresult ch1;//定义FFT结果变量
```

#### (3)初始化（打表提速）

```c
FFT_Init();
```

#### (4*)生成窗函数（不必要）

```c
float k = FlattopWin(NPT, win);
```

#### (5)调用FFT

```c
//根据你输入的数据类型选择不同的函数
FFT_win_u32(adc_buf, NPT, win, &ch1);
```

#### (6*)恢复幅值（不必要）

```c
AmpRecovery(&ch1, NPT, k);
```

如果你不关注幅值的话，可以不调用这个

## 四、空间性能

主要占空间的是以下变量

```c
typedef struct FFTresult
{
	Complex result[NPT_max];    //FFT的结果，是复数形式
	float	Amp[NPT_max];       //幅值
	float	phase[NPT_max];     //相位
    float   dc;                 //直流分量
}FFTresult; //存放FFT运算结果的结构体
```

```c
uint32_t adc_buf[NPT_max];
float win[NPT_max];
FFTresult ch1;//定义FFT结果变量
```

```c
float sin_table[NPT_max / 4];//sin打表
```

* result的大小：2 * NPT_max * 4 Byte
* Amp的大小：NPT_max * 4Byte
* phase的大小: NPT_max * 4Byte

* adc_buf的大小：NPT_max * 4 Byte
* win的大小：NPT_max * 4 Byte
* sin_table的大小：NPT_max Byte


不加窗的情况下占空间：21 * NPT_max Byte
加窗的情况下占空间：25 * NPT_max Byte

