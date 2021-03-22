#include <STC15F2K60S2.h>
#include <intrins.h>
#define sound_somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}
#define DELAY_TIME 20
#define control P2|=0xA0;P2&=0xBF;
#define wei_control P2|=0xC0;P2&=0xDF;
#define duan_control P2|=0xE0;P2&=0xFF;
#define led_control P2|=0x80;P2&=0x9F;
#define uchar unsigned char
#define uint unsigned int
#define ADD 0x38
sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */
sbit TX = P1^0;
sbit RX = P1^1;
uchar code led[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0xFF};
uchar one,two,three,four,five,six,seven,eight;

bit S7_down=0;S6_down=0;S5_down=0;S4_down=0;


void delay(uint xms);
void init(void);
void display4(uchar seven,uchar eight);
void display3(uchar five,uchar six);
void display2(uchar three,uchar four);
void display1(uchar one,uchar two);
void keyscanf(void);
uchar IIC_AD_read(void);
uchar IIC_EEPROM_read(uchar add);
void IIC_EEPROM_write(uchar add,uchar dat);
void distance_get(void);
void send_wave(void);
void Timer1Init(void)	;
void Timer0Init(void)	;

uint time0_flag=0,sound_time=0,distance=0,zhongliang=0,led_time=0;
uchar huowu=0,watch_flag=0,time_one=2,time_two=4,setting=0,start_flag=0,time=0,time_s=0,led_duan=0xFF;
bit sound_flag=0,led_flash=0,addi=0;

void main(void)
{
	init();
	Timer1Init();
	Timer0Init();
	EA=1;
	ET0=1;
	time_one=IIC_EEPROM_read(ADD);delay(5);
	time_two=IIC_EEPROM_read(ADD+1);delay(5);
	while(1)
	{
		if(start_flag==0)
		{
			zhongliang=IIC_AD_read()*19.6;
			if((zhongliang>0)&&(zhongliang<1000))
			{
				watch_flag=0;
				led_duan=0xFE;
				led_control;
				P0=led_duan;
				control;
				P0=0x00;
			}
			else if((zhongliang>=1000)&&(zhongliang<4000))
			{
				watch_flag=1;
				led_duan=0xFD;
				led_control;
				P0=led_duan;
				control;
				P0=0x00;
			}
			else if(zhongliang>=4000)
			{
				watch_flag=2;
				control;
				P0=0x40;
//				led_duan=0xFB;
//				led_control;
//				P0=led_duan;
//				led_flash=1;
			}
			if(setting==0)
			{
				if(watch_flag==0)
				{
					one=11;two=11;three=11;four=11;five=11;six=11;seven=11;eight=11;
				}
				else
				{
					distance_get();
					if(distance<=30)
					{
						huowu=1;
						one=1;two=11;three=11;four=distance/10;five=distance%10;six=11;seven=11;eight=1;
					}
					else
					{
						huowu=2;
						one=1;two=11;three=11;four=distance/10;five=distance%10;six=11;seven=11;eight=2;
					}
				}
			}
			else if(setting==1)
			{
				if(time_s%2==0)
				{
					one=3;two=11;three=11;four=time_one/10;five=time_one%10;six=11;seven=time_two/10;eight=time_two%10;
				}
				else
				{
					one=3;two=11;three=11;four=11;five=11;six=11;seven=time_two/10;eight=time_two%10;
				}
				if(addi==1)
				{
					addi=0;
					time_one++;
					if(time_one==11)
						time_one=1;
				}
			}
			else if(setting==2)
			{
				if(time_s%2==0)
				{
					one=3;two=11;three=11;four=time_one/10;five=time_one%10;six=11;seven=time_two/10;eight=time_two%10;
				}
				else
				{
					one=3;two=11;three=11;four=time_one/10;five=time_one%10;six=11;seven=11;eight=11;
				}
				if(addi==1)
				{
					addi=0;
					time_two++;
					if(time_two==11)
						time_two=1;
				}
			}
		}
		else if(start_flag==1)
		{
			control;
			P0=0x10;
			led_duan|=0x08;
			led_control;
			P0=led_duan;
			one=2;two=11;three=11;four=11;five=11;six=11;seven=time/10;eight=time%10;
		}
		else if(start_flag==2)
		{
			control;
			P0=0x00;
//			led_duan&=0xF7;
//			led_control;
//			P0=led_duan;
//			led_flash=1;
			one=2;two=11;three=11;four=11;five=11;six=11;seven=time/10;eight=time%10;
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
			if(setting!=0)
			{
				addi=1;
			}
			/*function*/
		}
	}
	else if((S6_down==1)&&(P31==1))
	{
		delay(5);
		if(P31==1)
		{
			S6_down=0;
			if(watch_flag==0)
			{
				setting++;
				if(setting==3)
				{
					IIC_EEPROM_write(ADD,time_one);delay(5);
					IIC_EEPROM_write(ADD+1,time_two);delay(5);
					setting=0;
				}
			}
			/*function*/
		}
	}
	else if((S5_down==1)&&(P32==1))
	{
		delay(5);
		if(P32==1)
		{
			S5_down=0;
			if(start_flag==1)
			{
				start_flag=2;
			}
			else if(start_flag==2)
			{
				start_flag=1;
			}
			/*function*/
		}
	}
	else if((S4_down==1)&&(P33==1))
	{
		delay(5);
		if(P33==1)
		{
			S4_down=0;
			if((watch_flag==1)&&(start_flag==0))
			{
				if(huowu==1){time=time_one;}
				else if(huowu==2){time=time_two;}
				start_flag=1;
			}
			/*function*/
		}
	}
}

void Timer0Init(void)		//2毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x9A;		//设置定时初值
	TH0 = 0xA9;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

void time0_isr(void) interrupt 1
{
	time0_flag++;
	if(time0_flag==100)
	{
		time0_flag=0;
		sound_flag=1;
	}
	if(watch_flag==2)
	{
		led_time++;
		if(led_time==250)
		{
			led_time=0;
			if(led_flash==1)
			{
				led_flash=0;
				led_duan=0xFF;
				led_control;
				P0=led_duan;
			}
			else
			{
				led_flash=1;
				led_duan=0xFB;
				led_control;
				P0=led_duan;
			}
		}
	}
	if(start_flag==2)
	{
		led_time++;
		if(led_time==250)
		{
			led_time=0;
			if(led_flash==1)
			{
				led_flash=0;
				led_duan|=0x08;
				led_control;
				P0=led_duan;
			}
			else
			{
				led_flash=1;
				led_duan&=0xF7;
				led_control;
				P0=led_duan;
			}
		}
	}
//	if(setting!=0)
//	{
//		led_time++;
//		if(led_time==500)
//		{
//			led_time=0;
//			time_s++;
//			if(time_s==61)
//				time_s=0;
//		}
//	}
//	if(start_flag==1)
//	{
//		led_time++;
//		if(led_time==500)
//		{
//			led_time=0;
//			time--;
//			if(time==0)
//				start_flag=0;
//		}
//	}
	if((setting!=0)||(start_flag==1))
	{
		led_time++;
		if(led_time==500)
		{
			led_time=0;
			time_s++;
			if(time_s==61)
				time_s=0;
			time--;
			if(time==0)
			{
				start_flag=0;
			}
		}
	}
}

void Timer1Init(void)		//2毫秒@11.0592MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
}

void send_wave(void)
{
	uchar i=8;
	do
	{
		TX=1;
		sound_somenop;sound_somenop;sound_somenop;sound_somenop;sound_somenop;sound_somenop;sound_somenop;sound_somenop;sound_somenop;sound_somenop;
		TX=0;
		sound_somenop;sound_somenop;sound_somenop;sound_somenop;sound_somenop;sound_somenop;sound_somenop;sound_somenop;sound_somenop;sound_somenop;
	}
	while(i--);
}

void distance_get(void)
{
	if(sound_flag==1)
	{
		sound_flag=0;
		send_wave();
		TR1=1;
		while((RX==1)&&(TF1==0))
		{
			if((TH1==0x40)|(TH1==0x80)|(TH1==0xA0)|(TH1==0xE0))
			{
				display1(one,two);display2(three,four);display3(five,six);display4(seven,eight);
			}
		}
		TR1=0;
		if(TF1==1)
		{
			distance=99;
			TF1=0;
		}
		else
		{
			sound_time=TH1;
			sound_time<<=8;
			sound_time|=TL1;
			distance=(uint)(sound_time*0.017);
			distance=distance/12;
		}
		TH1=0;
		TL1=0;
	}
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
	uchar dat_EE;
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
	dat_EE=IIC_RecByte();
	IIC_Stop();
	return dat_EE;
}

uchar IIC_AD_read(void)
{
	uchar dat_AD;
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
	dat_AD=IIC_RecByte();
	IIC_Stop();
	return dat_AD;
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


