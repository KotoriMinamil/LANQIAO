#include <STC15F2K60S2.h>
#include <intrins.h>
#define DELAY_TIME 20
#define sound_somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}
#define control P2|=0xA0;P2&=0xBF;
#define wei_control P2|=0xC0;P2&=0xDF;
#define duan_control P2|=0xE0;P2&=0xFF;
#define led_control P2|=0x80;P2&=0x9F;
#define uchar unsigned char
#define uint unsigned int
#define ADD 0x07
sbit TX=P1^0;
sbit RX=P1^1;
sbit SCLK=P1^7;		
sbit IO=P2^3;		
sbit CE = P1^3;   // DS1302复位
sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */
uchar code led[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0xFF};
uchar time[7]={59,50,11,0,0,0,0};
uchar one,two,three,four,five,six,seven,eight;
bit S7_down=0,S6_down=0,S5_down=0,S4_down=0;
void delay(uint xms);
void init();
void display4(uchar seven,uchar eight);
void display3(uchar five,uchar six);
void display2(uchar three,uchar four);
void display1(uchar one,uchar two);
void keyscanf(void);
void distance_get(void);
void Timer1Init(void);
void Timer0Init(void);
void DS1302_readtime(void);
void DS1302_init(void);
uchar IIC_EEPROM_read(uchar add);
void IIC_EEPROM_write(uchar add,uchar dat);
uint time0_flag=0,sound_time=0,distance=0,distance_EE=30,warning_distance=0,buzzer_time=0;
bit sound_flag=0,addi=0,sub=0,setting_flag=0,buzzer_flag=0;
uchar display_page=0,led_flash=0;
void main(void)
{
	init();
	Timer1Init();
	Timer0Init();
	EA=1;
	ET0=1;
	DS1302_init();
	distance_EE=IIC_EEPROM_read(ADD);
	while(1)
	{
		DS1302_readtime();
		if(display_page==0)
		{
			one=time[2]/10;two=time[2]%10;three=10;four=time[1]/10;five=time[1]%10;six=10;seven=time[0]/10;eight=time[0]%10;
		}
		else if(display_page==1)
		{
			distance_get();
			warning_distance=(uint)(1.2*distance_EE);
			if((distance>=distance_EE)&&(distance<=warning_distance))
			{
				led_flash++;
				led_control;P0=0xFE;
				if(led_flash>100)
				{
					led_control;P0=0xFF;
				}
				else if(led_flash>200)
				{
					led_flash=0;
					led_control;P0=0xFE;
				}
			}
			else if(distance<distance_EE)
			{
				led_control;P0=0xFF;
				buzzer_flag=1;
				control;
				P0=0x40;
			}
			else if(distance>warning_distance)
				led_control;P0=0xFF;
			one=11;two=11;three=11;four=11;five=11;six=distance/100;seven=distance%100/10;eight=distance%10;
		}
		else if(display_page==2)
		{
			if(setting_flag==0)
			{
				if(time[0]%2==0)
				{
					one=time[2]/10;two=time[2]%10;three=10;four=time[1]/10;five=time[1]%10;six=10;seven=time[0]/10;eight=time[0]%10;
				}
				else if(time[0]%2==1)
				{
					one=11;two=11;three=10;four=time[1]/10;five=time[1]%10;six=10;seven=time[0]/10;eight=time[0]%10;
				}
				if(addi==1)
				{
					addi=0;
					if(time[2]<23)
					{time[2]++;DS1302_init();}
				}
				if(sub==1)
				{
					sub=0;
					if(time[2]!=0)
					{time[2]--;DS1302_init();}
				}
			}
			else if(setting_flag==1)
			{
				if(time[0]%2==0)
				{
					one=time[2]/10;two=time[2]%10;three=10;four=time[1]/10;five=time[1]%10;six=10;seven=time[0]/10;eight=time[0]%10;
				}
				else if(time[0]%2==1)
				{
					one=time[2]/10;two=time[2]%10;three=10;four=11;five=11;six=10;seven=time[0]/10;eight=time[0]%10;
				}
				if(addi==1)
				{
					addi=0;
					if(time[1]<59)
					{time[1]++;DS1302_init();}
				}
				if(sub==1)
				{
					sub=0;
					if(time[1]!=0)
					{time[1]--;DS1302_init();}
				}
			}
		}
		else if(display_page==3)
		{
			one=11;two=11;three=11;four=11;five=11;six=11;seven=distance_EE/10;eight=distance_EE%10;
			if(addi==1)
			{
				addi=0;
				if(distance_EE<99)
				{distance_EE++;IIC_EEPROM_write(ADD,distance_EE);delay(5);}
			}
			if(sub==1)
			{
				sub=0;
				if(distance!=0)
				{distance_EE--;IIC_EEPROM_write(ADD,distance_EE);delay(5);}
			}
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
			if(display_page==0)
				display_page=1;
			else if(display_page==1)
				display_page=0;
			/*function*/
		}
	}
	else if((S6_down==1)&&(P31==1))
	{
		delay(5);
		if(P31==1)
		{
			S6_down=0;
			switch(display_page)
			{
				case 0:display_page=2;break;
				case 1:display_page=3;break;
				case 2:if(setting_flag!=1){setting_flag=1;}
								else{setting_flag=0;display_page=0;}
								break;
				case 3:display_page=1;break;
				default: break;
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
			if(display_page>1)
				addi=1;
			/*function*/
		}
	}
	else if((S4_down==1)&&(P33==1))
	{
		delay(5);
		if(P33==1)
		{
			S4_down=0;
			if(display_page>1)
				sub=1;
			/*function*/
		}
	}
}

void Timer0Init(void)		//2毫秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x40;		//设置定时初值
	TH0 = 0xA2;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

void time0_isr() interrupt 1
{
	time0_flag++;
	if(time0_flag==100)
	{
		time0_flag=0;
		sound_flag=1;
	}
	if(buzzer_flag==1)
	{
		buzzer_time++;
		if(buzzer_time==1500)
		{
			buzzer_flag=0;
			buzzer_time=0;
			control;
			P0=0x00;
		}
	}
}

void Timer1Init(void)		//2毫秒@12.000MHz
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
			TF1=0;
			distance=9999;
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

void Write_Ds1302(unsigned  char temp) 
{
	unsigned char i;
	for (i=0;i<8;i++)     	
	{ 
		SCLK=0;
		IO=temp&0x01;
		SCLK=1;
		temp>>=1; 
	}
}   

void Write_Ds1302_Byte( unsigned char address,unsigned char dat )     
{
	uchar temp;
 	CE=0;	
 	SCLK=0;	
 	CE=1; 	  
 	Write_Ds1302(address);
	temp=(dat/10<<4)|(dat%10);
 	Write_Ds1302(temp);		
 	CE=0; 
}

unsigned char Read_Ds1302_Byte ( unsigned char address )
{
 	unsigned char i,temp=0x00;
	uchar dat1,dat2;
 	CE=0;	
 	SCLK=0;	
 	CE=1;	
 	Write_Ds1302(address);
 	for (i=0;i<8;i++) 	
 	{		
		SCLK=0;
		temp>>=1;	
 		if(IO)
 		temp|=0x80;	
 		SCLK=1;
	} 
	IO=0;	
	dat1=temp/16;
	dat2=temp%16;
	temp=dat1*10+dat2;
	return (temp);			
}

void DS1302_init(void)
{
	uchar add=0x80,i;
	Write_Ds1302_Byte(0x8E,0x00);
	for(i=0;i<7;i++)
	{
		Write_Ds1302_Byte(add,time[i]);
		add=add+2;
	}
	Write_Ds1302_Byte(0x8E,0x80);
}

void DS1302_readtime(void)
{
	uchar add=0x81,i;
	Write_Ds1302_Byte(0x8E,0x00);
	for(i=0;i<7;i++)
	{
		time[i]=Read_Ds1302_Byte(add);
		add=add+2;
	}
	Write_Ds1302_Byte(0x8E,0x80);
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

void init()
{
	control;
	P0=0x40;
	wei_control;
	P0=0xFF;
	duan_control;
	P0=0x00;
	led_control;
	P0=0xFE;
	delay(1000);
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
