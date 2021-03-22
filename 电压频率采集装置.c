#include <STC15F2K60S2.h>
#include <intrins.h>

#define control P2|=0xA0;P2&=0xBF;
#define wei_control P2|=0xC0;P2&=0xDF;
#define duan_control P2|=0xE0;P2&=0xFF;
#define led_control P2|=0x80;P2&=0x9F;
#define DELAY_TIME 20
#define uchar unsigned char
#define uint unsigned int
sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */
uchar code led[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0xFF,0x8E/*F*/,0xC1/*U*/};
uchar one,two,three,four,five,six,seven,eight;
bit S7_down,S6_down,S5_down,S4_down;
bit time0_flag=0,display_flag=1,display_page=0;led_flag=1,DAC_flag=0;
void delay(uint xms);
void init(void);
void display4(uchar seven,uchar eight);
void display3(uchar five,uchar six);
void display2(uchar three,uchar four);
void display1(uchar one,uchar two);
void keyscanf(void);
void Timer1Init(void);
void Timer0Init(void);
void IIC_AD_write(uchar dat);
uchar IIC_AD_read(void);
void Timer2Init(void);
uint low,high,low_temp,high_temp,time1_flag=0,volt=0,pinlv=0,ne555_time=0,DAC_time=0;
uchar volt_256;
void main(void)
{
	init();
	Timer1Init();
	Timer0Init();
	Timer2Init();
	EA=1;
	ET1=1;
	while(1)
	{
		if((DAC_time==150)||(DAC_time==300))
		{
			volt_256=IIC_AD_read();
			volt=volt_256*1.96;
			if(DAC_flag==0)
			{
				IIC_AD_write(102);
			}
			if(DAC_flag==1)
			{
				IIC_AD_write(volt_256);
			}
		}
		if(display_flag==1)
		{
			if(display_page==0)
			{
				one=13;two=11;three=11;four=11;five=11;six=volt/100;seven=volt%100/10;eight=volt%10;
			}
			else if(display_page==1)
			{
				ne555_time=(high_temp+low_temp)*5;
				pinlv=1000000/ne555_time;
				if(pinlv<10){one=12;two=11;three=11;four=11;five=11;six=11;seven=11;eight=pinlv%10;}
				else if(pinlv<100){one=12;two=11;three=11;four=11;five=11;six=11;seven=pinlv%100/10;eight=pinlv%10;}
				else if(pinlv<1000){one=12;two=11;three=11;four=11;five=11;six=pinlv%1000/100;seven=pinlv%100/10;eight=pinlv%10;}
				else if(pinlv<10000){one=12;two=11;three=11;four=11;five=pinlv%10000/1000;six=pinlv%1000/100;seven=pinlv%100/10;eight=pinlv%10;}
				else if(pinlv<100000){one=12;two=11;three=11;four=pinlv%100000/10000;five=pinlv%10000/1000;six=pinlv%1000/100;seven=pinlv%100/10;eight=pinlv%10;}
				else if(pinlv<1000000){one=12;two=11;three=pinlv/100000;four=pinlv%100000/10000;five=pinlv%10000/1000;six=pinlv%1000/100;seven=pinlv%100/10;eight=pinlv%10;}	
			}
		}
		else if(display_flag==0)
		{
			one=11;two=11;three=11;four=11;five=11;six=11;seven=11;eight=11;
		}
		if(led_flag==1)
		{
			if(display_page==0)
			{
				led_control;
				P0|=0x02;P0&=0xFE;
				P2=0x00;
				if(volt<150){led_control;P0|=0x04;}
				else if((volt>=150)&&(volt<250)){led_control;P0&=0xFB;}	
				else if((volt>=250)&&(volt<350)){led_control;P0|=0x04;}
				else if(volt>=350){led_control;P0&=0xFB;}
			}
			else if(display_page==1)
			{
				led_control;
				P0|=0x01;P0&=0xFD;
				P2=0x00;
				if(pinlv<1000){led_control;P0|=0x08;}
				else if((pinlv>=1000)&&(pinlv<5000)){led_control;P0&=0xF7;}
				else if((pinlv>=5000)&&(pinlv<10000)){led_control;P0|=0x08;}
				else if((pinlv>=10000)){led_control;P0&=0xF7;}
			}
		}
		else if(led_flag==0)
		{
			led_control;
			P0=0xFF;
		}
		

		
		
		keyscanf();
		display1(one,two);
		display2(three,four);
		display3(five,six);
		display4(seven,eight);
	}
}



void keyscanf(void)
{
	if(P30==0)
	{
		delay(5);
		if(P30==0)
		{
			S7_down=1;
		}
	}
	else if(P31==0)
	{
		delay(5);
		if(P31==0)
		{
			S6_down=1;
		}
	}
	else if(P32==0)
	{
		delay(5);
		if(P32==0)
		{
			S5_down=1;
		}
	}
	else if(P33==0)
	{
		delay(5);
		if(P33==0)
		{
			S4_down=1;
		}
	}
	if((S7_down==1)&&(P30==1))
	{
		delay(5);
		if(P30==1)
		{
			S7_down=0;
			display_flag=~display_flag;
			/*function*/
		}
	}
	else if((S6_down==1)&&(P31==1))
	{
		delay(5);
		if(P31==1)
		{
			S6_down=0;
			if(led_flag==0)led_flag=1;
			else led_flag=0;
			/*function*/
		}
	}
	else if((S5_down==1)&&(P32==1))
	{
		delay(5);
		if(P32==1)
		{
			S5_down=0;
			if(DAC_flag==0)DAC_flag=1;
			else DAC_flag=0;
			/*function*/
		}
	}
	else if((S4_down==1)&&(P33==1))
	{
		delay(5);
		if(P33==1)
		{
			S4_down=0;
			display_page=~display_page;
			/*function*/
		}
	}
}

void Timer0Init(void)		//5微秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xC4;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	TF0 = 0;		//清除TF0标志
}

void Timer1Init(void)		//2毫秒@12.000MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x40;		//设置定时初值
	TH1 = 0xA2;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
}

void Timer2Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0x20;		//设置定时初值
	T2H = 0xD1;		//设置定时初值
	AUXR |= 0x10;		//定时器2开始计时
	IE2|=0x04;
}

void time2_isr() interrupt 12
{
	DAC_time++;
	if(DAC_time>=310)
	{
		DAC_time=0;
	}
}

void time1_isr() interrupt 3
{
	time1_flag++;
	if(time1_flag==495)
	{
		TR0=1;
		ET0=1;
	}
	else if(time1_flag==500)
	{
		TR0=0;
		ET0=0;
		time1_flag=0;
	}
}

void time0_isr() interrupt 1
{
	if(P34==0)
	{
		high++;
		if(time0_flag==1)
		{
			low_temp=low;
			low=0;
			time0_flag=0;
		}
	}
	else if(P34==1)
	{
		low++;
		if(time0_flag==0)
		{
			high_temp=high;
			high=0;
			time0_flag=1;
		}
	}
}

void display1(uchar one,uchar two)
{
	wei_control;
	P0=0x01;
	duan_control;
	P0=led[one];
	delay(1);
	wei_control;
	P0=0x02;
	duan_control;
	P0=led[two];
	delay(1);
}

void display2(uchar three,uchar four)
{
	wei_control;
	P0=0x04;
	duan_control;
	P0=led[three];
	delay(1);
	wei_control;
	P0=0x08;
	duan_control;
	P0=led[four];
	delay(1);
}

void display3(uchar five,uchar six)
{
	wei_control;
	P0=0x10;
	duan_control;
	P0=led[five];
	delay(1);
	wei_control;
	P0=0x20;
	duan_control;
	if((display_page==0)&&(display_flag==1))
	P0=led[six]&0x7F;
	else
		P0=led[six];
	delay(1);
}

void display4(uchar seven,uchar eight)
{
	wei_control;
	P0=0x40;
	duan_control;
	P0=led[seven];
	delay(1);
	wei_control;
	P0=0x80;
	duan_control;
	P0=led[eight];
	delay(1);
	wei_control;
	P0=0x80;
	duan_control;
	P0=0xFF;
}

void init(void)
{
	control;
	P0=0x00;
	wei_control;
	P0=0xFF;
	duan_control;
	P0=0xFF;
	led_control;
	P0=0xFF;
}

void delay(uint xms)
{
	uint i,j;
	for(i=xms;i>0;i--)
		for(j=845;j>0;j--);
}

void IIC_Delay(unsigned char i)
{
    do{_nop_();}
    while(i--);        
}
//总线启动条件
void IIC_Start(void)
{
    SDA = 1;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 0;
    IIC_Delay(DELAY_TIME);
    SCL = 0;	
}

//总线停止条件
void IIC_Stop(void)
{
    SDA = 0;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//等待应答
bit IIC_WaitAck(void)
{
    bit ackbit;
	
    SCL  = 1;
    IIC_Delay(DELAY_TIME);
    ackbit = SDA;
    SCL = 0;
    IIC_Delay(DELAY_TIME);
    return ackbit;
}

//通过I2C总线发送数据
void IIC_SendByte(unsigned char byt)
{
    unsigned char i;

    for(i=0; i<8; i++)
    {
        SCL  = 0;
        IIC_Delay(DELAY_TIME);
        if(byt & 0x80) SDA  = 1;
        else SDA  = 0;
        IIC_Delay(DELAY_TIME);
        SCL = 1;
        byt <<= 1;
        IIC_Delay(DELAY_TIME);
    }
    SCL  = 0;  
}

//从I2C总线上接收数据
unsigned char IIC_RecByte(void)
{
    unsigned char i, da;
    for(i=0; i<8; i++)
    {   
    	SCL = 1;
	IIC_Delay(DELAY_TIME);
	da <<= 1;
	if(SDA) da |= 1;
	SCL = 0;
	IIC_Delay(DELAY_TIME);
    }
    return da;    
}

uchar IIC_AD_read(void)
{
	uchar data_AD;
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x03);
	IIC_WaitAck();
	IIC_Stop();
	delay(1);
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	data_AD=IIC_RecByte();
	IIC_Stop();
	return data_AD;
}

void IIC_AD_write(uchar dat)
{
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x40);
	IIC_WaitAck();
	IIC_SendByte(dat);
	IIC_WaitAck();
	IIC_Stop();
}
