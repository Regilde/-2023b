#include "capture.h"

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

void stringToHexWithSuffix2(int n)
{
		
    char buffer[50];
    int len;
    // 组合字符串，n 可以是12-17
    len = snprintf(buffer, sizeof(buffer), "sys=%d", n);
	printf("%s",buffer);
	for (int i = 0; i < 3; i++)
        putchar(0xFF);
}

void length_cmp(void)
{
	double length = 0;
	adc3_init();
	FFT_Init(adc3_data);
	
	length = 5555555555;
	stringToHexWithSuffix(1,length);
}
double direct_volume(void)
{
	double sum =0;
	double ave_dir_v = 0;
	adc2_init();
	for (int i=0;i<8192;i++)
	{
		sum += (adc2_data[i]*3.3/65535);
	}
	ave_dir_v = sum/8192;
	
	return ave_dir_v;
}

void load_type (void)
{
	double DV = direct_volume();
	if( DV > 11111 )
	{
		stringToHexWithSuffix2(2);
	}
	else 
	{
		stringToHexWithSuffix2(1);
	}
}
