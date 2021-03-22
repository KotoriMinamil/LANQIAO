#include <STC15F2K60S2.h>
#include <intrins.h>

#define uchar unsigned char
#define uint unsigned int
	
#define control P2|=0xA0;P2&=0xBF;
#define wei_control P2|=0xC0;P2&=0xDF;
#define duan_control P2|=0xE0;P2&=0xFF;
#define led_control P2|=0x80;P2&=0x9F;


#define DELAY_TIME 20

sbit SCLK=P1^7;		
sbit IO=P2^3;		
sbit CE = P1^3;   // DS1302复位

sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */
	
uchar code led[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0xFF};
uchar time[7]={0,30,8,0,0,0,0};
uchar one,two,three,four,five,six,seven,eight;
uchar S7_down,S6_down,S5_down,S4_down;


void DS1302_init(void);
void DS1302_readtime(void);

uchar IIC_AD_read(uchar add);
uchar IIC_EEPROM_read(uchar add);
void IIC_EEPROM_write(uchar add,uchar dat);
	
void display4(uchar seven,uchar eight);
void display3(uchar five,uchar six);
void display2(uchar three,uchar four);
void display1(uchar one,uchar two);
void keyscanf(void);
void init(void);
void delay(uint xms);

uint volt,time0_flag=0;
uchar shidu=0,yuzhi=50;
bit auto_flag=1,water=0,buzzer=1,setting=0,add=0,sub=0;


void main(void)
{
	init();
	DS1302_init();
	if(IIC_EEPROM_read(0x1F)!=18)
	{
		delay(5);
		IIC_EEPROM_write(0x01,yuzhi);
		delay(5);
		IIC_EEPROM_write(0x1F,18);
		delay(5);
	}
	yuzhi=IIC_EEPROM_read(0x01);
	delay(5);
	while(1)
	{
		volt=IIC_AD_read(0x03)*19.6;
		shidu=volt/50;
		DS1302_readtime();
		if(auto_flag==1)
		{
			if(setting==1)
			{
				one=10;two=10;three=11;four=11;five=11;six=11;seven=yuzhi/10;eight=yuzhi%10;
				if(add==1)
				{
					if(yuzhi<99)
					yuzhi++;
					add=0;
				}
				if(sub==1)
				{
					if(yuzhi!=0)
						yuzhi--;
					sub=0;
				}
			}
			else
			{
				one=time[2]/10;two=time[2]%10;three=10;four=time[1]/10;five=time[1]%10;six=11;seven=shidu/10;eight=shidu%10;
				if(shidu<yuzhi)
				{
					control;
					P0=0x10;
				}
				else
				{
					control;
					P0=0x00;
				}
			}
		}
		else if(auto_flag==0)
		{
			one=time[2]/10;two=time[2]%10;three=10;four=time[1]/10;five=time[1]%10;six=11;seven=shidu/10;eight=shidu%10;
			if(water==1)
			{
				if((shidu<yuzhi)&&(buzzer==1))
				{
					control;
					P0=0x50;
				}
				else
				{
					control;
					P0=0x10;
				}
			}
			else if(water==0)
			{
				if((shidu<yuzhi)&&(buzzer==1))
				{
					control;
					P0=0x40;
				}
				else
				{
					control;
					P0=0x00;
				}
			}
		}
		
		DS1302_readtime();
		display1(one,two);
		display2(three,four);
		display3(five,six);
		display4(seven,eight);
		keyscanf();
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
			auto_flag=~auto_flag;
		}
	}
	else if((S6_down==1)&&(P31==1))
	{
		delay(5);
		if(P31==1)
		{
			S6_down=0;
			if(auto_flag==1)
			{
				if(setting==0)
				{
					yuzhi=IIC_EEPROM_read(0x01);
					setting=1;
				}
				else if(setting==1)
				{
					IIC_EEPROM_write(0x01,yuzhi);
					setting=0;
				}
			}
			else if(auto_flag==0)
			{
				buzzer=~buzzer;
			}
		}
	}
	else if((S5_down==1)&&(P32==1))
	{
		delay(5);
		if(P32==1)
		{
			S5_down=0;
			if(auto_flag==0)
			{
				water=1;
			}
			else
			{
				add=1;
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
			if(auto_flag==0)
			{
				water=0;
			}
			else
			{
				sub=1;
			}
			/*function*/
		}
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

uchar IIC_AD_read(uchar add)
{
	uchar data_AD;
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(add);
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

void DS1302_writebyte(uchar dat) 
{
	uchar i;
	for (i=0;i<8;i++)     	
	{ 
		SCLK=0;
		IO=dat&0x01;
		dat>>=1;
		SCLK=1;
	}
}

void DS1302_write(uchar add,uchar dat)
{
	uchar temp;
	CE=0;
	SCLK=0;
	CE=1;
	DS1302_writebyte(add);
	temp=(dat/10<<4)|(dat%10);
	DS1302_writebyte(temp);
	CE=0;
}

uchar DS1302_read (uchar add)
{
 	uchar i,temp;
	uchar data1,data2;
 	CE=0;	
 	SCLK=0;	
 	CE=1;	
 	DS1302_writebyte(add);
 	for (i=0;i<8;i++) 	
 	{		
		SCLK=0;
		temp>>=1;	
 		if(IO)
 		temp|=0x80;	
 		SCLK=1;
	}
	IO=0;
	data1=temp/16;
	data2=temp%16;
	temp=data1*10+data2;
	return (temp);			
}

void DS1302_init(void)
{
	uchar i,add=0x80;
	DS1302_write(0x8E,0x00);
	for(i=0;i<7;i++)
	{
		DS1302_write(add,time[i]);
		add=add+2;
	}
	DS1302_write(0x8E,0x80);
}

void DS1302_readtime(void)
{
	uchar i,add=0x81;
	DS1302_write(0x8E,0x00);
	for(i=0;i<7;i++)
	{
		time[i]=DS1302_read(add);
		add=add+2;
	}
	DS1302_write(0x8E,0x80);
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
