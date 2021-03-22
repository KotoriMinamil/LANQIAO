#include <STC15F2K60S2.h>
#include <intrins.h>
#define DELAY_TIME 20
#define uchar unsigned char
#define uint unsigned int
	
#define control P2|=0xA0;P2&=0xBF;
#define wei_control P2|=0xC0;P2&=0xDF;
#define duan_control P2|=0xE0;P2&=0xFF;
#define led_control P2|=0x80;P2&=0x9F;

sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */
bit watch_out=0;
uchar code led[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0xFF};

uchar one,two,three,four,five,six,seven,eight;
uchar S7_down,S6_down,S5_down,S4_down;
uint time0_flag=0;
uint ml_10=0,money=0,light=0;
void init(void);
void delay(uint xms);
void Timer0Init(void);
void display4(uchar seven,uchar eight);
void display3(uchar five,uchar six);
void display2(uchar three,uchar four);
void display1(uchar one,uchar two);

void keyscanf(void);
uchar IIC_AD_read(void);

void main()
{
	one=11;two=0;three=5;four=0;
	init();
	Timer0Init();
	while(1)
	{
		light=IIC_AD_read()*19.6;
		if(light<=1250)
		{
			led_control;
			P0=0xFE;
		}
		else
		{
			led_control;
			P0=0xFF;
		}
		if(watch_out==1)
		{
			EA=0;ET0=0;
			control;
			P0=0x00;
			money=ml_10*5;
			//five=money/10000;six=money%10000/1000;seven=money%1000/100;eight=money%100/10;
			five=5;six=0;seven=0;eight=0;
			ml_10=0;
			watch_out=0;
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
			EA=1;ET0=1;
			control;
			P0=0x10;
		}
	}
	else if((S6_down==1)&&(P31==1))
	{
		delay(5);
		if(P31==1)
		{
			S6_down=0;
			EA=0;ET0=0;
			control;
			P0=0x00;
			money=ml_10*5;
			five=money/10000;six=money%10000/1000;seven=money%1000/100;eight=money%100/10;
			ml_10=0;
			/*function*/
		}
	}
	else if((S5_down==1)&&(P32==1))
	{
		delay(5);
		if(P32==1)
		{
			S5_down=0;
			/*function*/
		}
	}
	else if((S4_down==1)&&(P33==1))
	{
		delay(5);
		if(P33==1)
		{
			S4_down=0;
			/*function*/
		}
	}
}

void Timer0Init(void)		//5毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x00;		//设置定时初值
	TH0 = 0x28;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

void time0_isr() interrupt 1
{
	time0_flag++;
	if(time0_flag==20)
	{
		time0_flag=0;
		ml_10++;
		if(ml_10>=9999)
		{
			watch_out=1;
		}
		five=ml_10/1000;six=ml_10%1000/100;seven=ml_10%100/10;eight=ml_10%10;
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
	P0=led[two]&0x7F;
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
	P0=led[six]&0x7F;
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
	IIC_SendByte(0x01);
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

