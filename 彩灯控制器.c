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

bit S7_down,S6_down,S5_down,S4_down,start=0,set_flash=0;

uchar code led[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0xFF};
uchar one,two,three,four,five,six,seven,eight;

uchar IIC_AD_read(void);
uchar IIC_EEPROM_read(uchar add);
void IIC_EEPROM_write(uchar add,uchar dat);
void init(void);
void delay(uint xms);
void keyscanf(void);
void Timer0Init(void);
void display4(uchar seven,uchar eight);
void display3(uchar five,uchar six);
void display2(uchar three,uchar four);
void display1(uchar one,uchar two);

uint time0_flag=0,volt=0,set_time=0;
uchar light,led_close=0,moshi=1,led_move=0,setting=0,led_time,wait=4;

void main(void)
{
	init();
	Timer0Init();
	one=11;two=11;three=11;four=11;five=11;six=11;seven=11;eight=11;
	wait=IIC_EEPROM_read(0x02);
	delay(5);
	moshi=IIC_EEPROM_read(0x01);
	delay(5);
	while(1)
	{
		volt=19.6*IIC_AD_read();
		if(volt<=1250){light=1;led_close=2;}
		else if((volt>1250)&&(volt<=2500)){light=2;led_close=7;}
		else if((volt>2500)&&(volt<=3750)){light=3;led_close=12;}
		else if(volt>3750){light=4;led_close=19;}
		
		if((S4_down==1)&&(setting==0))
		{
			one=11;two=11;three=11;four=11;five=11;six=11;seven=10;eight=light;
		}
		else if(setting==0)
		{
			one=11;two=11;three=11;four=11;five=11;six=11;seven=11;eight=11;
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
	if((S4_down==1)&&(P33==1))
	{
		S4_down=0;
		if(setting==1)
		{
			if(moshi>0)
				moshi--;
		}
		else if(setting==2)
		{
			if(wait>4)
				wait--;
		}
	}
	if((P30==1)&&(S7_down==1))
	{
		delay(5);
		if(P30==1)
		{
			S7_down=0;
			if(start==0){start=1;time0_flag=0;led_move=0;}
			else if(start==1){start=0;}
		}
	}
	else if((P31==1)&&(S6_down==1))
	{
		delay(5);
		if(P31==1)
		{
			S6_down=0;
			if(setting==0){setting=1;set_time=0;}
			else if(setting==1){setting=2;set_time=0;}
			else if(setting==2){setting=0;set_time=0;
													one=11;two=11;three=11;four=11;five=11;six=11;seven=11;eight=11;
								IIC_EEPROM_write(0x01,moshi);delay(5);IIC_EEPROM_write(0x02,wait);delay(5);}
		}
	}
	else if((P32==1)&&(S5_down==1))
	{
		delay(5);
		if(P32==1)
		{
			S5_down=0;
			if(setting==1)
			{
				if(moshi<4)
					moshi++;
			}
			else if(setting==2)
			{
				if(wait<12)
					wait++;
			}
		}
	}
}

void time0_isr() interrupt 1
{
	time0_flag++;
	set_time++;
	if((time0_flag==1)&&(start==1))
	{
		if(moshi==1)
		{
			led_control;P0=~(0x01<<led_move);
		}
		else if(moshi==2)
		{
			led_control;P0=~(0x80>>led_move);
		}
		else if(moshi==3)
		{
			led_control;
			if(led_move==0){P0=0x7E;}
			else if(led_move==1){P0=0xBD;}
			else if(led_move==2){P0=0xDB;}
			else if(led_move==3){P0=0xE7;}
		}
		else if(moshi==4)
		{
			led_control;
			if(led_move==0){P0=0xE7;}
			else if(led_move==1){P0=0xDB;}
			else if(led_move==2){P0=0xBD;}
			else if(led_move==3){P0=0x7E;}
		}
	}
	else if(time0_flag==led_close)
	{
		led_control;P0=0xFF;
	}
	else if(time0_flag==20)
	{
		time0_flag=0;led_time++;
		if(led_time*20>=wait*100)
		{
			led_time=0;
			if(moshi==1){led_move++;if(led_move>=8){led_move=0;moshi=2;}}
			else if(moshi==2){led_move++;if(led_move>=8){led_move=0;moshi=3;}}
			else if(moshi==3){led_move++;if(led_move>=4){led_move=0;moshi=4;}}
			else if(moshi==4){led_move++;if(led_move>=4){led_move=0;moshi=1;}}
		}
	}
	if(set_time==800)
	{
		if(setting==1)
		{
			set_time=0;
			if(set_flash==0)
			{
				set_flash=1;
				one=10;two=moshi;three=10;four=11;five=wait/10;if(wait<10)five=11;six=wait%10;seven=0;eight=0;
			}
			else if(set_flash==1)
			{
				set_flash=0;
				one=11;two=11;three=11;four=11;five=wait/10;if(wait<10)five=11;six=wait%10;seven=0;eight=0;
			}
		}
		else if(setting==2)
		{
			set_time=0;
			if(set_flash==0)
			{
				set_flash=1;
				one=10;two=moshi;three=10;four=11;five=wait/10;if(wait<10)five=11;six=wait%10;seven=0;eight=0;
			}
			else if(set_flash==1)
			{
				set_flash=0;
				one=10;two=moshi;three=10;four=11;five=11;six=11;seven=11;eight=11;
			}
		}
	}
}

void Timer0Init(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xCD;		//设置定时初值
	TH0 = 0xD4;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;
	EA=1;
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

void delay(uint xms)
{
	uint i,j;
	for(i=xms;i>0;i--)
		for(j=845;j>0;j--);
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

void IIC_EEPROM_write(uchar add,uchar dat)
{
	IIC_Start();
	IIC_SendByte(0xA0);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	IIC_SendByte(dat);
	IIC_WaitAck();
	IIC_Stop();
}

uchar IIC_EEPROM_read(uchar add)
{
	uchar data_EE;
	IIC_Start();
	IIC_SendByte(0xA0);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	IIC_Stop();
	delay(1);
	IIC_Start();
	IIC_SendByte(0xA1);
	IIC_WaitAck();
	data_EE=IIC_RecByte();
	IIC_Stop();
	return data_EE;
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