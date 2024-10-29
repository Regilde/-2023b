#include "capture.h"

double length = 0.0;

void stringToHexWithSuffix(int n, double value)
{		
    char buffer[50];
    int len;
    // 组合字符串，n 可以是12-17
    len = snprintf(buffer, sizeof(buffer), "t%d.txt=\"%.3f\"", n, value);
	printf("%s",buffer);
	for (int i = 0; i < 3; i++)
        putchar(0xFF);
}

void length_cmp(void)
{
	adc3_init();
	FFT_Init(adc3_data);
	
	length = 5555;////////////
}

void load_type (void)
{
	//测量压降
	double v_dir = direct_v();
	if( direct_v() < 1)//////数值要改
	{
		
	}
	else
	{
		
	}
}

double direct_v(void)
{
	double v_ave = 0;
	double v_sum = 0;
	adc3_init();
	for (int i=0;i<8192;i++)
	{
		v_sum += adc3_data[i];
	}
	v_ave = v_sum / 8192;
	return v_ave;
}