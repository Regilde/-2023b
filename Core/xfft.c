#include "xFFT.h"

ALIGN_32BYTES (Complex q[FFT_LENGTH])   __attribute__((section(".ARM.__at_0x24000000")) );
ALIGN_32BYTES (Complex q1[FFT_LENGTH])  __attribute__((section(".ARM.__at_0x24040000")) );

 double v1[5]={0};
 double f2[5]={0};//
 	double limit = 10;
 int window_mode = 0;
 double direct_v = 0;
void FFT_DIS(void)
{
//		for(uint32_t i=1550; i<6750; i++)
//		for(uint32_t i = 0; i < FFT_LENGTH; i++) 
		for(uint32_t i=0; i<FFT_LENGTH; i++)
		{
			printf("%f\r\n", q1[i].real);
//			printf("%d:\t%f\r\n", i,  q1[i].real);	
      //printf("%f\r\n", (adc1_data[i] *3.3/65536));			
		}
	
}
void FFT_Init(uint16_t *adc_data)
{
		double a0 = 0.21557895;
    double a1 = 0.41663158;
    double a2 = 0.277263158;
    double a3 = 0.083578947;
    double a4 = 0.006947368;
		double v_max = 0;

		double flag_max = 0;
	
		for(uint32_t i=0; i<FFT_LENGTH; i++)
		{
			/* *(0.54f-0.46f*cosf(6.283185307f*i/FFT_LENGTH)) */
//			q1[i].real = adc1_data[i] *(0.5-0.5*cos(6.283185307*i/FFT_LENGTH))*3.3/65536;	
			double window = a0
                        - a1 * cos(2 * PI * i / (FFT_LENGTH - 1))
                        + a2 * cos(4 * PI * i / (FFT_LENGTH - 1))
                        - a3 * cos(6 * PI * i / (FFT_LENGTH - 1))
                        + a4 * cos(8 * PI * i / (FFT_LENGTH - 1));
			q1[i].real = adc_data[i] *window*3.3/(65536*0.22);		
			q1[i].imag = 0;
		}
		FFT(q1, 13);
		for(uint32_t i=0; i<FFT_LENGTH; i++)
		{
			arm_sqrt_f32((float)(q1[i].real *q1[i].real+ q1[i].imag*q1[i].imag), &q1[i].real); 
		}
		
		 int v_index = 0;
		 for(uint32_t k=0; k<=5; k++)
		{
		  v1[k]=0;
		  f2[k]=0;
		}
		for (int i=5;i<4096;i++)
		{
			v1[0] = complex_abs(q1[0]);
			if (complex_abs(q1[i])>200)
			{
				v1[1] = complex_abs(q1[i]);
				for (int j=i;j<i+20;j++)
				{
					if ( complex_abs(q1[j+1]) > v1[1] )
					v1[1] = complex_abs(q1[j+1]);
					
				}
				break;
			}
		}
		
		/*for (int i = 10; i < 8192/2 - 50 && v_index < 5; i ++) {
        double q1_i = complex_abs(q1[i]);
        double q1_i_10 = complex_abs(q1[i+10])	;							     
       if (((q1_i_10 / q1_i)  >= 15)&& (q1_i_10 >=limit)) {
            double max = q1_i_10;
            int max_index = i+25;
            for (int j = i+25; j <= i+60; j++) {
                double abs_value = complex_abs(q1[j]);
                if (abs_value > max) {
                    max = abs_value;
                    max_index = j;
                }
            }			
            v1[v_index] = max;
            f2[v_index] = max_index;
						//else if (flag == 1)v3[v_index] = max_index;
            v_index++;
						i=i+20;
        }
    }*/
		direct_v = complex_abs(q1[0])/8192;
		
}

double complex_abs(Complex c) 
	{
    //return sqrt(c.real * c.real + c.imag * c.imag);
		return c.real;
}
	
unsigned char FFT(Complex *d,int m)
{
#ifndef __EXTERN_W__
	static Complex *w;
	static int mw = 0;
	float arg, w_real, w_imag, wr_real, wr_imag, wtemp;
#endif
	static int n = 1;
	Complex temp, tmp1, tmp2;
	Complex *di, *dip, *dj, *wp;
	int i, j, k, l, le, wi;
#ifndef __EXTERN_W__
	if(m != mw)
	{
		if(mw != 0)
			free(w);
		mw = m;
		if(m == 0)
			return 0;
		n = 1 << m;
		le = n >> 1;
		w =q;
		if(!w)
			return 0;
		arg = 4.0 * atan(1.0) / le;
		wr_real = w_real = cos(arg);
		wr_imag = w_imag = -sin(arg);
		dj = w;
		for(j = 1; j < le; j++)
		{
			dj->real = (float)wr_real;
			dj->imag = (float)wr_imag;
			dj++;
			wtemp = wr_real * w_real - wr_imag * w_imag;
			wr_imag = wr_real * w_imag + wr_imag * w_real;
			wr_real = wtemp;
		}
	}
#else
	n = 1 << m;
#endif
	le = n;
	wi = 1;
	for(l = 0; l < m; l++)
	{
		le >>= 1;
		for(i = 0; i < n; i += (le << 1))
		{
			di = d + i;
			dip = di + le;
			temp.real = (di->real + dip->real);
			temp.imag = (di->imag + dip->imag);
			dip->real = (di->real - dip->real);
			dip->imag = (di->imag - dip->imag);
			*di = temp;
		}
		wp = (Complex*)w + wi - 1;
		for(j = 1; j < le; j++)
		{
			tmp1 = *wp;
			for(i = j; i < n; i += (le << 1))
			{
				di = d + i;
				dip = di + le;
				temp.real = (di->real + dip->real);
				temp.imag = (di->imag + dip->imag);
				tmp2.real = (di->real - dip->real);
				tmp2.imag = (di->imag - dip->imag);
				dip->real = (tmp2.real * tmp1.real - tmp2.imag * tmp1.imag);
				dip->imag = (tmp2.real * tmp1.imag + tmp2.imag * tmp1.real);
				*di = temp;
			}
			wp += wi;
		}
		wi <<= 1;
	}

	for(i = 0; i < n; i++)
	{
		j = 0;
		for(k = 0; k < m; k++)
			j = (j << 1) | ((i >> k) & 1);
		if(i < j)
		{
			di = d + i;
			dj = d + j;
			temp = *dj;
			*dj = *di;
			*di = temp;
		}
	}
	return 1;
}

