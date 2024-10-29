#ifndef	_xFFT_H
#define _xFFT_H

#include "main.h"
//extern ALIGN_32BYTES (Complex q1[FFT_LENGTH])  __attribute__((section(".ARM.__at_0x24040000")) );
typedef struct
{
	float real;
	float imag;
} Complex;

extern double limit;
extern int flag;
extern double v1[5];
extern double f2[5];

void FFT_Init(uint16_t *adc_data);
unsigned char FFT(Complex *x,int m);
double complex_abs(Complex c) ;

void FFT_DIS(void);

#endif
