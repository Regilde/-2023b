#include "fun.h"
#include "main.h"

extern double direct_v;
double gain_list[20]={0};
double Rin_list[20]={0};
double Rout_list[20]={0};
double direct_list[20]={0};
double gain_v = 0;//电压增益
double Rin = 0;//输入阻抗
double Rout = 0;//输出电阻
double direct_v_out = 0;
double last_gain_v =0;//电压增益
double last_Rin =0;//输入阻抗
double last_Rout = 0;//输出电阻
double real_gain=0,real_Rin=0,real_Rout=0;
 uint8_t rec_data;
int view = 0,cha=10;
int av_flag=0;
double ave_vol(uint16_t *adc_data)
{
	int dif[18]={0};
	double sum =0;
	
	for (int i =0;i<18;i++)
	{
		int start = i*400+10;
		uint16_t max_value1 = adc_data[start];
	uint16_t min_value1 = adc_data[start];
		for(int j=start;j<start+400;j++)
		{
			if (adc_data[j] > max_value1) 
			{
				max_value1 = adc_data[j];
			}
			if (adc_data[j] < min_value1 ) 
			{
				min_value1 = adc_data[j];
			}
		}
		dif[i] = max_value1 -min_value1;
	}
	for(int k=0;k<18;k++)
	{
		sum+=dif[k];
	}
	
	return (sum/18)*3.3/65535;
	
}


void Compute()
{	

	TIM15->PSC = 300;
	adc1_init();
	adc2_init();
	double Rs1 = 2.3;//需要改至0欧以上
	double Rs2 = 1.994;
	limit = 10;
	double v1=FFT_Init(adc1_data);//Ri_v1
	double v2=FFT_Init(adc2_data);//Ri_v2
	HAL_GPIO_WritePin (GPIOD,GPIO_PIN_8,GPIO_PIN_RESET);//闭合继电器
	HAL_Delay(50);//继电器开启延时
	adc2_init();//测增益的采样
	HAL_Delay(10);
	adc3_init();	
	limit = 10;
	flag = 1;
	double v_in = FFT_Init(adc2_data);
	flag = 0;
	limit = 100;//得试试
	double v_out=FFT_Init(adc3_data)*6;//vo

	//HAL_Delay (500);
	
	direct_v_out = direct_v*6;//真实值可能需要加上偏置电压
	HAL_GPIO_WritePin (GPIOD,GPIO_PIN_9,GPIO_PIN_RESET);//闭合继电器
	HAL_Delay(50);//继电器开启延时
	adc3_init();//vol
	limit = 50;
	double vol=FFT_Init(adc3_data)*6;//vol
	
	gain_v = v_out/v_in;//电压增益/////////////////////////////////////////////////////////////////////////////////////////////
	if(av_flag == 0){
	if(gain_v<139&&gain_v>130) gain_v+=15;
	else if(gain_v<=130&&gain_v>120) gain_v+=25;
	else if(gain_v<=120&&gain_v>110) gain_v+=35;
	}
	Rin = v2*Rs1/(v1-v2);//输入阻抗
	Rout = (v_out-vol)*Rs2/vol;//输出电阻

	HAL_GPIO_WritePin (GPIOD,GPIO_PIN_9,GPIO_PIN_SET);//断开继电器
	HAL_GPIO_WritePin (GPIOD,GPIO_PIN_8,GPIO_PIN_SET);//断开继电器
	HAL_Delay(50);//继电器关闭延时
	limit = 50;
		
	if(direct_v_out>0 && direct_v_out <0.02) gain_v = 1;//BJT输入 输出直接相连	
		
	//测试代码
	/*printf("v1=%f\r\n",v1);
	printf("v2=%f\r\n",v2);
	printf("v_in=%f\r\n",v_in);
	printf("v_out=%f\r\n",v_out);
	printf("Av=%f\r\n",gain_v);
	printf("Rin=%f\r\n",Rin);
	printf("Rout=%f\r\n",Rout);
	printf("direct_v_out=%f\r\n",direct_v_out);
	printf("v_ol=%f\r\n",vol);
	printf("/////////////////////////////////\r\n");
	*/
}

void Result_Dis(void)
{
	for (int i = 0; i < 3; i++) putchar(0xFF);      
	stringToHexWithSuffix(3,gain_v);
	stringToHexWithSuffix(4,Rin);
	stringToHexWithSuffix(5,Rout);
	stringToHexWithSuffix(9,direct_v_out);
}

void ChangeFreq()
{
	HAL_GPIO_WritePin (GPIOD,GPIO_PIN_8,GPIO_PIN_RESET);//闭合继电器,测量增益
	HAL_GPIO_WritePin (GPIOD,GPIO_PIN_9,GPIO_PIN_SET);//继电器,测量增益
	HAL_Delay(10);
	char text[20];
	double Av_pic[300]={0};
	double Av = 0;
	double Av_max = 0;
	double Av_min = 200;
	double v4=0,v3=0;
	int k = 0;
	int picture;
	char buff[20];
	av_flag=1;
	for (int i=1;i<300;i+=2)
	{	
		if(i<20) TIM15->PSC = 500;//40kHz-500
		else if(i>=20&&i<120) TIM15->PSC = 40;//250kHz-80
		else if(i>=120&&i<200) TIM15->PSC = 20;//500kHz-40
		else TIM15->PSC = 10;//1Mhz-20//TIM15->PSC = 4000/i;
		HAL_Delay(10);
		
		sprintf(text,"%d\r\n",i*1000);
		HAL_UART_Transmit(&huart7,(uint8_t *)text,sizeof(text),50);
		HAL_Delay(20);
		
		adc2_init();
		adc3_init();
		limit = 50;
		v4=6*FFT_Init(adc3_data);
		limit = 10;
		v3=FFT_Init(adc2_data);
		Av = v4/v3;
		
		if(Av > 10&& Av<200)				
		{
			Av_pic[i-1] = v4/v3;		
			//绘制波形
			//为了在屏幕上更满，每个值画五次
			for(int k =0;k<3;k++){
			picture = snprintf(buff, sizeof(buff), "add 10,0,%d",(int)Av);
			printf("%s",buff);
			for (int i = 0; i < 3; i++)
      putchar(0xFF);}
			
			if(i<21) 
			{
				Av_max +=Av_pic[i-1];
				k++;
			}
		}
			else Av_pic[i-1] = Av_pic[i-3];	
		//
		
	}
	//printf("max=%f\r\n",Av_max/k);
	for(int i =1;i<300;i+=2)
	{		
		if(Av_pic[i-1]>10&&Av_pic[i-1]<200) {
		if((Av_pic[i-1]/(Av_max/k))<0.707)
		{		
			stringToHexWithSuffix(13,i);
			break;
		}
		}
	}
	TIM15->PSC = 500;//还原初始采样频率
	HAL_GPIO_WritePin (GPIOD,GPIO_PIN_8,GPIO_PIN_SET);//断开继电器,测量增益
	av_flag=0;
}

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

void Debug()
{	
	char f1[20];
	sprintf(f1,"%d\r\n",1000);
	HAL_UART_Transmit(&huart7,(uint8_t *)f1,sizeof(f1),50);
	char f200[20];
	int error_mode = 20;
	int error_text;
	char error_buff[25];
	//TIM15->PSC = 200;//100kHz，以便2s内完成
	Compute();//Av,Rin,Rout,direct_v
		Result_Dis();
	last_Rin = Rin;
	last_Rout = Rout;
	
	if((direct_v_out<8)&&direct_v_out>6.5)//直流正常
	{
		if(Rin<0) error_mode = 0;
		else if (Rin >10 && Rin <12) error_mode = 1;
		else if((Rin>1.8)&&(Rin<2.4)&&((Rout>1.85)&&(Rout)<2.2)&&((gain_v<170.5)&&(gain_v>139.5)))
		{
			sprintf(f200,"%d\r\n",200000);
			HAL_UART_Transmit(&huart7,(uint8_t *)f200,sizeof(f200),50);
			HAL_Delay(50);
			Compute();
			
			sprintf(f200,"%d\r\n",1000);
			HAL_UART_Transmit(&huart7,(uint8_t *)f200,sizeof(f200),50);
			HAL_Delay(50);
			if(gain_v >125) error_mode = 2;
			else if(gain_v<80) error_mode = 3;
			else
			{
				if(last_Rin >=2.2 && last_Rin < 2.4) 
				{
				error_mode = 4;
			
				}//加功能--变化不大则保留上次显示结果。
				else if(last_Rin <2.15 && last_Rin > 1.8) error_mode = 5;
			}
			
		}
		
		else error_mode = 20;
	}
	else if(direct_v_out>10)//直流大于10
	{
		if(Rin==0)
		{
			if(direct_v_out<11.5) error_mode = 6;//
			else if (direct_v_out>11.5) error_mode = 7;
		}
		else 
		{
			if(Rin >= 14 && Rin <20) error_mode = 8;
			else if(Rin >= 1.8 && Rin <3) error_mode = 9;
			else if(Rin >= 10 && Rin <13) error_mode = 10;//
			else error_mode = 20;
		}
	}
	else if(direct_v_out<5)//直流小于5
	{
		if(direct_v_out < 2)
		{
			if (Rin <= 0.1) error_mode = 11;
			else error_mode = 12;
		}
		else if (direct_v_out>2) error_mode = 13;
		else error_mode = 20;
	}
	else error_mode = 20;
	
	switch(error_mode)
	{
		case 0: error_text = snprintf(error_buff, sizeof(error_buff), "t11.txt=\"C1 Open\"");break;
		case 1: error_text = snprintf(error_buff, sizeof(error_buff), "t11.txt=\"C2 Open\"");break;
		case 2: error_text = snprintf(error_buff, sizeof(error_buff), "t11.txt=\"C3 Open\"");break;
		case 3: error_text = snprintf(error_buff, sizeof(error_buff), "t11.txt=\"C3Double\"");break;
		case 4: error_text = snprintf(error_buff, sizeof(error_buff), "t11.txt=\"none\"");break;
		case 5: error_text = snprintf(error_buff, sizeof(error_buff), "t11.txt=\"C2Double\"");break;
		case 6: error_text = snprintf(error_buff, sizeof(error_buff), "t11.txt=\"R1 Short\"");break;
		case 7: error_text = snprintf(error_buff, sizeof(error_buff), "t11.txt=\"R2 Short\"");break;
		case 8: error_text = snprintf(error_buff, sizeof(error_buff), "t11.txt=\"R1 Open\"");break;
		case 9: error_text = snprintf(error_buff, sizeof(error_buff), "t11.txt=\"R3 Short\"");break;
		case 10: error_text = snprintf(error_buff, sizeof(error_buff), "t11.txt=\"R4 Open\"");break;
		case 11: error_text = snprintf(error_buff, sizeof(error_buff), "t11.txt=\"R4 Short\"");break;
		case 12: error_text = snprintf(error_buff, sizeof(error_buff), "t11.txt=\"R3 Open\"");break;
		case 13: error_text = snprintf(error_buff, sizeof(error_buff), "t11.txt=\"R2 Open\"");break;		
		case 20: error_text = snprintf(error_buff, sizeof(error_buff), "t11.txt=\"measuring\"");break;
	}
	
	printf("%s",error_buff);
	for (int i = 0; i < 3; i++)
        putchar(0xFF);
}

 void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart ->Instance == USART1)
	{
		HAL_UART_Transmit(huart,&rec_data,1,50);
		HAL_UART_Receive_IT(huart,&rec_data,1);
		
		
		//printf("%d\r\n",view);
	}
  		
	
}