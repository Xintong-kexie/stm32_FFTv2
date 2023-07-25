#include "FFTv2.h"
#include "math.h"
#include "stdio.h"

float sin_table[NPT_max / 4];
uint8_t use_table = 0;

void FFT_Init(void)
{
	use_table = 1;
	for (int i = 0; i < NPT_max / 4; i++)
	{
		sin_table[i] = sin(2 * pi * i / NPT_max);
	}
}

/*
 *FFT函数主体
 *length: FFT点数，必须为2的整数次方
 *ch: FFTresult存放结果的结构体
 */
static void FFT(int length, FFTresult* ch)
{
	DFT(&ch->result[0], length);
	for (int i = 0; i < length; i++)
	{
		ch->Amp[i] = Complex_Modular(ch->result[i]);
		ch->Amp[i] = ch->Amp[i] / length * 2;
		if (i == 0)
		{
			ch->Amp[i] /= 2;
		}
		ch->phase[i] = Complex_arg(ch->result[i]);
	}
}

/*
 *输入数据的类型为uint32_t的FFT
 *input: 输入数据
 *length: FFT点数，必须为2的整数次方
 *ch: FFTresult存放结果的结构体
 */
void FFT_u32(uint32_t* input, int length, FFTresult* ch)
{
    //通过比特反转法来对数组重新排序
	for (int i = 0; i < length; i++)
	{
		int temp = BitReverse(i, length);
		ch->result[i].real = (float)input[temp];
		ch->result[i].imag = 0;
	}
	FFT(length, ch);
	ch->dc = ch->Amp[0];
}

/*
 *输入数据的类型为float的FFT
 *input: 输入数据
 *length: FFT点数，必须为2的整数次方
 *ch: FFTresult存放结果的结构体
 */
void FFT_f32(float* input, int length, FFTresult* ch)
{
	for (int i = 0; i < length; i++)
	{
		int temp = BitReverse(i, length);
		ch->result[i].real = input[temp];
		ch->result[i].imag = 0;
	}
	FFT(length, ch);
	ch->dc = ch->Amp[0];
}

/*
 *输入数据的类型为int的FFT
 *input: 输入数据
 *length: FFT点数，必须为2的整数次方
 *ch: FFTresult存放结果的结构体
 */
void FFT_int32(int* input, int length, FFTresult* ch)
{
	for (int i = 0; i < length; i++)
	{
		int temp = BitReverse(i, length);
		ch->result[i].real = (float)input[temp];
		ch->result[i].imag = 0;
	}
	FFT(length, ch);
	ch->dc = ch->Amp[0];
}

/*
 *输入数据的类型为int16_t的FFT
 *input: 输入数据
 *length: FFT点数，必须为2的整数次方
 *ch: FFTresult存放结果的结构体
 */
void FFT_int16(int16_t* input, int length, FFTresult* ch)
{
	for (int i = 0; i < length; i++)
	{
		int temp = BitReverse(i, length);
		ch->result[i].real = (float)input[temp];
		ch->result[i].imag = 0;
	}
	FFT(length, ch);
	ch->dc = ch->Amp[0];
}

/*
 *输入数据的类型为uint32_t的加窗FFT
 *input: 输入数据
 *length: FFT点数，必须为2的整数次方
 *win: 窗函数
 *ch: FFTresult存放结果的结构体
 */
void FFT_win_u32(uint32_t* input, int length, float* win, FFTresult* ch)
{
	float sum = 0;
	for (int i = 0; i < length; i++)
	{
		sum += input[i];
	}
	ch->dc = sum / length;
	for (int i = 0; i < length; i++)
	{
		int temp = BitReverse(i, length);
		ch->result[i].real = ((float)input[temp] - ch->dc) * win[temp];
		ch->result[i].imag = 0;
	}
	FFT(length, ch);
}

/*
 *输入数据的类型为float的加窗FFT
 *input: 输入数据
 *length: FFT点数，必须为2的整数次方
 *win: 窗函数
 *ch: FFTresult存放结果的结构体
 */
void FFT_win_f32(float* input, int length, float* win, FFTresult* ch)
{
	float sum = 0;
	for (int i = 0; i < length; i++)
	{
		sum += input[i];
	}
	ch->dc = sum / length;
	for (int i = 0; i < length; i++)
	{
		int temp = BitReverse(i, length);
		ch->result[i].real = (input[temp] - ch->dc) * win[temp];
		ch->result[i].imag = 0;
	}
	FFT(length, ch);
}

/*
 *输入数据的类型为int的加窗FFT
 *input: 输入数据
 *length: FFT点数，必须为2的整数次方
 *win: 窗函数
 *ch: FFTresult存放结果的结构体
 */
void FFT_win_int32(int* input, int length, float* win, FFTresult* ch)
{
	float sum = 0;
	for (int i = 0; i < length; i++)
	{
		sum += input[i];
	}
	ch->dc = sum / length;
	for (int i = 0; i < length; i++)
	{
		int temp = BitReverse(i, length);
		ch->result[i].real = ((float)input[temp] - ch->dc) * win[temp];
		ch->result[i].imag = 0;
	}
	FFT(length, ch);
}

void FFT_win_int16(int16_t* input, int length, float* win, FFTresult* ch)
{
	float sum = 0;
	for (int i = 0; i < length; i++)
	{
		sum += input[i];
	}
	ch->dc = sum / length;
	for (int i = 0; i < length; i++)
	{
		int temp = BitReverse(i, length);
		ch->result[i].real = ((float)input[temp] - ch->dc) * win[temp];
		ch->result[i].imag = 0;
	}
	FFT(length, ch);
}

/*
 *恢复加窗后带来的幅值衰减
 *ch: 待恢复幅值的FFT结果
 *length: FFT点数
 *k: 恢复系数，生成窗函数时的返回值就是恢复系数
 */
void AmpRecovery(FFTresult* ch, int length, float k)
{
	for (int i = 0; i < length; i++)
	{
		ch->Amp[i] *= k;
	}
}

/*
 *比特反转，为了做蝶形运算需要将数组重新排序
 *采用比特反转法可以生成新的数组下标
 *input: 原始的下标
 *N: FFT点数，必须是2的整数次方
 *return: 生成的新数组下标
 */
static int BitReverse(int input, int N)
{
	int output = 0;

	int times = (int)ceil(log2f((float)N));
	for (int i = 0; i < times; i++)
	{
		output <<= 1;
		output += (input % 2);
		input /= 2;
	}

	return output;
}

/*
 *递归用DFT
 */
static void DFT(Complex* p, int len)
{
	if (len == 1)
	{
		return;
	}
	else
	{
		DFT(p, len / 2);
		DFT(p + len / 2, len / 2);
		for (int i = 0; i < len / 2; i++)
		{
			Complex c1 = *p;
			Complex c2 = *(p + len / 2);
			Complex w;//旋转因子
			if (use_table == 0)
			{
				w.real = (float)cos(2 * pi / len * i);
				w.imag = -(float)sin(2 * pi / len * i);
			}
			else
			{
				w.real = Mycos(len, i);
				w.imag = -Mysin(len, i);
			}
			*p = Complex_Add(c1, Complex_Multi(c2, w));
			*(p + len / 2) = Complex_Minus(c1, Complex_Multi(c2, w));
			p++;
		}
		return;
	}
}

// 复数乘法，返回值等于c1 * c2
Complex Complex_Multi(Complex c1, Complex c2)
{
	Complex result;
	result.real = c1.real * c2.real - c1.imag * c2.imag;
	result.imag = c1.real * c2.imag + c1.imag * c2.real;
	return result;
}

// 复数加法，返回值等于c1 + c2
Complex Complex_Add(Complex c1, Complex c2)
{
	Complex result;
	result.real = c1.real + c2.real;
	result.imag = c1.imag + c2.imag;
	return result;
}

// 复数减法，返回值等于c1 - c2
Complex Complex_Minus(Complex c1, Complex c2)
{
	Complex result;
	result.real = c1.real - c2.real;
	result.imag = c1.imag - c2.imag;
	return result;
}

// 复数求模
float Complex_Modular(Complex c)
{
	return (float)sqrt(c.real * c.real + c.imag * c.imag);
}

// 复数求辐角
float Complex_arg(Complex c)
{
	return atan2f(c.imag, c.real);
}

/*-------阶乘-------*/
static int Factorial(int num)
{
	int a = 1;
	for (int i = num; i >= 1; i--)
	{
		a *= i;
	}
	return a;
}
/*--------0阶贝塞尔--------*/
static float Besseli(float x)
{
	float a = 1.0;
	for (int i = 1; i < 20; i++)
	{
		a += pow((pow((x / 2), i) * 1.0 / Factorial(i)), 2);
	}
	return a;
}
/*--------Kaiser窗--------*/
float Kaiser(int n, float beta, float* win)
{
	for (int i = 0; i < n; i++)
	{
		win[i] = Besseli(beta * sqrt(1 - pow(2.0 * i / (n - 1) - 1, 2))) / Besseli(beta);
	}
	return 1;
}
/*--------Flattop窗--------*/
float FlattopWin(int n, float* win)
{
	float a0 = 0.21557895, a1 = 0.41663158, a2 = 0.277263158, a3 = 0.083578947, a4 = 0.006947368;
    
	for (int i = 0; i < n; i++)
	{
		win[i] = a0 - a1 * cos(2 * pi * i / (n - 1)) + a2 * cos(4 * pi * i / (n - 1))
			- a3 * cos(6 * pi * i / (n - 1)) + a4 * cos(8 * pi * i / (n - 1));
	}
	return 4.639;
}
/*--------Hanning--------*/
float Hanning(int n, float* win)
{
	for (int i = 0; i < n; i++)
	{
		win[i] = 0.5 * (1 - cos(2 * pi * i / (n - 1)));
	}
	return 2;
}
/*--------Hamming--------*/
float Hamming(int n, float* win)
{
	for (int i = 0; i < n; i++)
	{
		win[i] = 0.54 - 0.46 * cos(2 * pi * i / (n - 1));
	}
	return 1.852;
}
/*--------rectangle--------*/
float rectangle(int n, float* win)
{
	for (int i = 0; i < n; i++)
	{
		win[i] = 1;
	}
	return 1;
}

// sin打表
static float Mysin(int len, int i)
{
	if (len == 2)
	{
		return 0.0f;
	}
	else
	{
		if (i < len / 4)
		{
			return sin_table[NPT_max / len * i];
		}
		else if (i > len / 4)
		{
			i = len / 2 - i;
			return sin_table[NPT_max / len * i];
		}
		else
		{
			return 1.0f;
		}
	}
}
// cos打表
static float Mycos(int len, int i)
{
	if (len == 2)
	{
		return 1.0f;
	}
	else
	{
		i = (i + len / 4) % (len / 2);
		if (i < len / 4)
		{
			return -sin_table[NPT_max / len * i];
		}
		else if (i > len / 4)
		{
			i = len / 2 - i;
			return sin_table[NPT_max / len * i];
		}
		else
		{
			return 1.0f;
		}
	}
}
