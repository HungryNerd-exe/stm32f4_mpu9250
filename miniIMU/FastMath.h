/*
The MIT License (MIT)

Copyright (c) 2015-? suhetao

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _FASTMATH_H_
#define _FASTMATH_H_

//translate from the DSP instruction of a DSP Library.
#define PI (3.1415926535897932384626433832795f)
#define PI_2 (1.5707963267948966192313216916398f)
#define PI_3 (1.0471975511965977461542144610932f)
#define PI_4 (0.78539816339744830961566084581988f)
#define PI_6 (0.52359877559829887307710723054658f)
#define TWO_MINUS_ROOT3 (0.26794919243112270647255365849413f)
#define SQRT3_MINUS_1 (0.73205080756887729352744634150587f)
#define SQRT3 (1.7320508075688772935274463415059f)
#define EPS_FLOAT (+3.452669830012e-4f)
//Coefficients used for atan/atan2
#define ATANP_COEF0 (-1.44008344874f)
#define ATANP_COEF1 (-7.20026848898e-1f)
#define ATANQ_COEF0 (+4.32025038919f)
#define ATANQ_COEF1 (+4.75222584599f)
//Coefficients used for asin/acos
#define ASINP_COEF1 (-2.7516555290596f)
#define ASINP_COEF2 (+2.9058762374859f)
#define ASINP_COEF3 (-5.9450144193246e-1f)
#define ASINQ_COEF0 (-1.6509933202424e+1f)
#define ASINQ_COEF1 (+2.4864728969164e+1f)
#define ASINQ_COEF2 (-1.0333867072113e+1f)

typedef union {
	long i;
	float f;
}Long2Float;

//S10.22
int FP_FastSqrtI(int number)
{
	long x;
	unsigned int y_1;
	unsigned int f = 0x600000;
	Long2Float flt, flt2;
	flt.f = number;

	x = number << 15;
	flt.i = 0x5f3759df - ( flt.i >> 1 );

	flt2.f = flt.f + 2.0;
	flt2.i &= 0x7FFFFF;
	y_1 = (flt2.i >> 11) * (flt2.i >> 11);
	y_1 = (y_1 >> 8) * (x >> 8);
	y_1 = f - y_1;
	flt2.i = (flt2.i >> 11) * (y_1 >> 11);
	if (x < 171){
		y_1 = (flt2.i >> 11) * (flt2.i >> 11);
		y_1 = (y_1 >> 8) * (x >> 8);
		y_1 = f - y_1;
		flt2.i = (flt2.i >> 11) * (y_1 >> 11);
	}
	return (number * flt2.i + 0x1e3c68) >> 22;
}

__inline float FastInvSqrt(float x)
{
	Long2Float l2f;
	float r;
	float y = 0.5f * x;

	l2f.f = y;
	l2f.i = 0x5f3759df - (l2f.i >> 1);
	r = l2f.f;
	r = r * (1.5f - y * r * r);
	//r = r * (1.5f - y * r * r);
	return r;
}

__inline float FastSqrt(float x)
{
	return x * FastInvSqrt(x);
}

__inline float FastAsin(float x)
{
	float y, g;
	float num, den, result;
	long i;
	float sign = 1.0;

	y = x;
	if (y < (float)0.0){
		y = -y;
		sign = -sign;
	}

	if (y > (float)0.5){
		i = 1;
		if (y > (float)1.0){
			result = 0.0;
			return result;
		}    
		g = (1.0f - y) * 0.5f;
		y = -2.0f * FastSqrt(g);
	}
	else{
		i = 0;
		if (y < (float)EPS_FLOAT){
			result = y;
			if (sign < (float)0.0){
				result = -result;
			}
			return result;
		}
		g = y * y;
	}
	num = ((ASINP_COEF3 * g + ASINP_COEF2) * g + ASINP_COEF1) * g;
	den = ((g + ASINQ_COEF2) * g + ASINQ_COEF1) * g + ASINQ_COEF0;
	result = num / den;
	result = result * y + y;
	if (i == 1){
		result = result + (float)PI_2;
	}
	if (sign < (float)0.0){
		result = -result;
	}
	return result;
}

__inline float FastAtan2(float y, float x)
{
	float f, g;
	float num, den;
	float result;
	int n;

	static const float a[4] = {0, (float)PI_6, (float)PI_2, (float)PI_3};

	if (x == (float)0.0){
		if (y == (float)0.0){
			result = 0.0;
			return result;
		}

		result = (float)PI_2;
		if (y > (float)0.0){
			return result;
		}
		if (y < (float)0.0){
			result = -result;
			return result;
		}
	}
	n = 0;
	num = y;
	den = x;

	if (num < (float)0.0){
		num = -num;
	}
	if (den < (float)0.0){
		den = -den;
	}
	if (num > den){
		f = den;
		den = num;
		num = f;
		n = 2;
	}
	f = num / den;

	if (f > (float)TWO_MINUS_ROOT3){
		num = f * (float)SQRT3_MINUS_1 - 1.0f + f;
		den = (float)SQRT3 + f;
		f = num / den;
		n = n + 1;
	}

	g = f;
	if (g < (float)0.0){
		g = -g;
	}

	if (g < (float)EPS_FLOAT){
		result = f;
	}
	else{
		g = f * f;
		num = (ATANP_COEF1 * g + ATANP_COEF0) * g;
		den = (g + ATANQ_COEF1) * g + ATANQ_COEF0;
		result = num / den;
		result = result * f + f;
	}
	if (n > 1){
		result = -result;
	}
	result = result + a[n];

	if (x < (float)0.0){
		result = PI - result;
	}
	if (y < (float)0.0){
		result = -result;
	}
	return result;
}

#endif