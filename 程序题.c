#include <STC15F2K60S2.h>
#include <intrins.h>
#include <stdio.h>
#define DELAY_TIME 40
#define control P2|=0xA0;P2&=0xBF;
#define wei_control P2|=0xC0;P2&=0xDF;
#define duan_control P2|=0xE0;P2&=0xFF;
#define led_control P2|=0x80;P2&=0x9F;
#define uchar unsigned char
#define uint unsigned int
#define sound_nop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}
sbit TX=P1^0;
sbit RX=P1^1;
sbit DQ = P1^4;  //单总线接口
//总线引脚定义
sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */
bit S17_down=0,S16_down=0,S12_down=0,S13_down=0;
uchar code duanma[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,\
										 0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,0xBF,0xFF,0xC6,0xC7,0xC8,0x8C};
//                                                                      20   21   22C  23L 24n  25P
uchar display[8]={11,12,13,14,15,16,17,18};
uchar dis_count=0;
void Timer0Init(void);
void Timer2Init(void);
void init(void);
void delay(uint xms);
void send_wave(void);
uchar distance_get(void);
long ds18b20_get(void);
void IIC_AD_WRITE(uchar dat);
void IIC_EEPROM_WRITE(uchar add,uchar dat);
uchar IIC_EEPROM_READ(uchar add);
void send(uchar *p,uchar length);
void UartInit(void);
void keyscanf(void);

uchar usart1_rx_buffer[8]={"111"};
uchar usart1_tx_buffer[15]={"texttext"};
uchar usart1_rx_flag=0,usart1_rx_count=0,usart1_rx_done=0;
uint distance,sound_time,check_time=0,check=0,set_number=0;
uchar sound_flag=0,wendu_flag=0;juli=0,key_time=0,display_page=0,setting=0,led_flash=0xFF,t_yuzhi=30,d_yuzhi=35;
uchar DAC_flag=1,t_temp=0,d_temp=0,E_number1=0,E_number2=0,E_number3=0;
uchar wendu_temp1=0,wendu_temp2=0;
long wendu;
void main(void)
{
	init();
	Timer2Init();
	Timer0Init();
	UartInit();
	ES=1;
//	E_number1=IIC_EEPROM_READ(0x61);delay(5);
//	E_number2=IIC_EEPROM_READ(0x62);delay(5);
//	E_number3=IIC_EEPROM_READ(0x63);delay(5);
//	set_number=E_number1*10000+E_number2*100+E_number3;
	led_flash&=0xFB;led_control;P0=led_flash;
	while(1)
	{
		if(setting==0)
		{
			if(wendu_flag==1)
			{
				wendu_flag=0;
				wendu=ds18b20_get();
			}
			if(sound_flag==1)
			{
				sound_flag=0;
				juli=distance_get();
			}
			if(display_page==0)
			{
				display[0]=22;display[1]=21;display[2]=21;display[3]=21;display[4]=wendu/100000;display[5]=wendu%100000/10000+10;display[6]=wendu%10000/1000;display[7]=wendu%1000/100;
				if(wendu/10000>=t_yuzhi)
				{
					led_flash&=0xFE;led_control;P0=led_flash;
				}
				else
				{
					led_flash|=0x01;led_control;P0=led_flash;
				}
			}
			else if(display_page==1)
			{
				display[0]=23;display[1]=21;display[2]=21;display[3]=21;display[4]=21;display[5]=21;display[6]=juli/10;display[7]=juli%10;
				if(juli<d_yuzhi)
				{
					led_flash&=0xFD;led_control;P0=led_flash;
					if(DAC_flag==1){IIC_AD_WRITE(102);}
				}
				else
				{
					led_flash|=0x02;led_control;P0=led_flash;
					if(DAC_flag==1){IIC_AD_WRITE(204);}
				}
			}
			else if(display_page==2)
			{
				display[0]=24;display[1]=21;display[2]=21;
				if(set_number>10000)
				{display[3]=set_number/10000;display[4]=set_number%10000/1000;display[5]=set_number%1000/100;display[6]=set_number%100/10;display[7]=set_number%10;}
				else if(set_number>1000)
				{display[3]=21;display[4]=set_number/1000;display[5]=set_number%1000/100;display[6]=set_number%100/10;display[7]=set_number%10;}
				else if(set_number>100)
				{display[3]=21;display[4]=21;display[5]=set_number/100;display[6]=set_number%100/10;display[7]=set_number%10;}
				else if(set_number>10)
				{display[3]=21;display[4]=21;display[5]=21;display[6]=set_number/10;display[7]=set_number%10;}
				else 
				{display[3]=21;display[4]=21;display[5]=21;display[6]=21;display[7]=set_number%10;}
			}
		}
		else if(setting==1)
		{
			if(display_page==0)
			{
				display[0]=25;display[1]=21;display[2]=21;display[3]=1;display[4]=21;display[5]=21;display[6]=t_yuzhi/10;display[7]=t_yuzhi%10;
			}
			else if(display_page==1)
			{
				display[0]=25;display[1]=21;display[2]=21;display[3]=2;display[4]=21;display[5]=21;display[6]=d_yuzhi/10;display[7]=d_yuzhi%10;
			}
		}
		if(usart1_rx_done==1)
		{
			usart1_rx_done=0;
			if((usart1_rx_buffer[0]='S')&(usart1_rx_buffer[1]='T')&(usart1_rx_buffer[2]='\r')&(usart1_rx_buffer[3]='\n'))
			{
				wendu_temp1=wendu/100000*10+wendu%100000/10000;
				wendu_temp2=wendu%10000/1000*10+wendu%1000/100;
				sprintf((char*)usart1_tx_buffer,"$%0.2bd,%0.2bd,%0.2bd\r\n",juli,wendu_temp1,wendu_temp2);
				send(usart1_tx_buffer,11);
			}
			else if((usart1_rx_buffer[0]='P')&(usart1_rx_buffer[1]='A')&(usart1_rx_buffer[2]='R')&(usart1_rx_buffer[3]='A')&(usart1_rx_buffer[4]='\r')&(usart1_rx_buffer[5]='\n'))
			{
				sprintf((char*)usart1_tx_buffer,"#0.2bd,0.2bd\r\n",d_yuzhi,t_yuzhi);
				send(usart1_tx_buffer,8);
			}
			else
			{
				sprintf((char*)usart1_tx_buffer,"ERROR\r\n");
				send(usart1_tx_buffer,7);
			}
		}
		keyscanf();
//		display[0]=juli/10;display[1]=juli%10;display[2]=20;display[3]=wendu/100000;display[4]=wendu%100000/10000;display[5]=wendu%10000/1000;display[6]=wendu%1000/100;display[7]=check%10;
		
	}
}

void keyscanf(void)
{
	P35=1;P34=0;
	if(P32==0)
	{
		delay(5);
		if(P32==0)
		{
			S17_down=1;
		}
	}
	else if(P33==0)
	{
		delay(5);
		if(P33==0)
		{
			S16_down=1;
		}
	}
	if((S17_down==1)&&(P32==1))
	{
		delay(5);
		if(P32==1)
		{
			S17_down=0;
			if(setting==1)
			{
				switch(display_page)
				{
					case 0: if(t_yuzhi>0){t_yuzhi-=2;}break;
					case 1: if(d_yuzhi>0){d_yuzhi-=5;}break;
					default: break;
				}
			}
		}
	}
	else if((S16_down==1)&&(P33==1))
	{
		delay(5);
		if(P33==1)
		{
			S16_down=0;
			if(setting==1)
			{
				switch(display_page)
				{
					case 0: if(t_yuzhi<98){t_yuzhi+=2;}break;
					case 1: if(d_yuzhi<95){d_yuzhi+=5;}break;
					default: break;
				}
			}
		}
	}
	
	P35=0;P34=1;
	if(P32==0)
	{
		delay(5);
		if(P32==0)
		{
			S13_down=1;
			while(!P32)
			{
				delay(100);
				if(++key_time>10)
				{
					S13_down=0;key_time=0;
					if(DAC_flag==1)
					{
						DAC_flag=0;
						led_flash|=0x04;
						led_control;
						P0=led_flash;
						IIC_AD_WRITE(20);
					}
					else if(DAC_flag==0)
					{
						DAC_flag=1;
						led_flash&=0xFB;
						led_control;
						P0=led_flash;
					}
					break;
				}
			}
			if(S13_down==1)
			{
				S13_down=0;key_time=0;
				if(setting==0)
				{
					t_temp=t_yuzhi;
					d_temp=d_yuzhi;
					display_page=0;
					setting=1;
				}
				else if(setting==1)
				{
					if((t_yuzhi!=t_temp)|(d_yuzhi!=d_temp))
					{
						set_number=set_number+1;
						E_number1=set_number/10000;
						E_number2=set_number%10000/100;
						E_number3=set_number%100;
						IIC_EEPROM_WRITE(0x61,E_number1);delay(5);
						IIC_EEPROM_WRITE(0x62,E_number2);delay(5);
						IIC_EEPROM_WRITE(0x63,E_number3);delay(5);
					}
					display_page=0;
					setting=0;
				}
			}
		}
		while(!P32);
	}
	else if(P33==0)
	{
		delay(5);
		if(P33==0)
		{
			S12_down=1;
			while(!P33)
			{
				delay(100);
				if(++key_time>10)
				{
					S12_down=0;key_time=0;
					set_number=0;
					IIC_EEPROM_WRITE(0x61,0);delay(5);
					IIC_EEPROM_WRITE(0x62,0);delay(5);
					IIC_EEPROM_WRITE(0x63,0);delay(5);
					break;
				}
			}
			if(S12_down==1)
			{
				S12_down=0;key_time=0;
				if(setting==0)
				{
					display_page++;
					if(display_page==3)
						display_page=0;
				}
				else if(setting==1)
				{
					display_page++;
					if(display_page==2)
						display_page=0;
				}
			}
		}
		while(!P33);
	}
}
	

void Timer2Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0x20;		//设置定时初值
	T2H = 0xD1;		//设置定时初值
	AUXR |= 0x10;		//定时器2开始计时
	IE2 |= 0X04;
	EA=1;
}

void tim2_isr() interrupt 12
{
	check_time++;
	wei_control;
	P0=(1<<dis_count);
	duan_control;
	P0=duanma[display[dis_count]];
	if(++dis_count==8)dis_count=0;
	if(check_time==499)
	{
		wendu_flag=1;
	}
	if(check_time==999)
	{
		check_time=0;
		wendu_flag=1;
		sound_flag=1;
	}
	if(usart1_rx_flag>=1)
	{
		usart1_rx_flag++;
		if(usart1_rx_flag>=30)
		{
			usart1_rx_flag=0;
			usart1_rx_done=1;
		}
	}
}

void UartInit(void)		//4800bps@12.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1 = 0x8F;		//设定定时初值
	TH1 = 0xFD;		//设定定时初值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
}

void send(uchar *p,uchar length)
{
	uchar i;
	for(i=0;i<length;i++)
	{
		SBUF=*(p+i);
		while(TI==0);
		TI=0;
	}
}

void uart_isr() interrupt 4
{
	if(RI)
	{
		usart1_rx_buffer[usart1_rx_count]=SBUF;
		usart1_rx_count++;
		usart1_rx_flag=1;
		RI=0;
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

uchar IIC_EEPROM_READ(uchar add)
{
	uchar dat;
	IIC_Start();
	IIC_SendByte(0xA0);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(0xA1);
	IIC_WaitAck();
	dat=IIC_RecByte();
	IIC_Stop();
	return dat;
}

void IIC_EEPROM_WRITE(uchar add,uchar dat)
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

void IIC_AD_WRITE(uchar dat)
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

//单总线延时函数
void Delay_OneWire(unsigned int t)  //STC89C52RC
{
	while(t--);
}

//通过单总线向DS18B20写一个字节
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0;
		DQ = dat&0x01;
		Delay_OneWire(40);
		DQ = 1;
		dat >>= 1;
	}
	Delay_OneWire(40);
}

//从DS18B20读取一个字节
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  
	for(i=0;i<8;i++)
	{
		DQ = 0;
		dat >>= 1;
		DQ = 1;
		if(DQ)
		{
			dat |= 0x80;
		}	    
		Delay_OneWire(40);
	}
	return dat;
}

//DS18B20设备初始化
bit init_ds18b20(void)
{
  	bit initflag = 0;
  	
  	DQ = 1;
  	Delay_OneWire(96);
  	DQ = 0;
  	Delay_OneWire(640);
  	DQ = 1;
  	Delay_OneWire(80); 
    initflag = DQ;     
  	Delay_OneWire(40);
  
  	return initflag;
}

long ds18b20_get(void)
{
	long temp;
	uchar low,high;
	init_ds18b20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0x44);
	Delay_OneWire(200);
	init_ds18b20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0xBE);
	low=Read_DS18B20();
	high=Read_DS18B20();
	temp=(high&0x0F);
	temp<<=8;
	temp|=low;
	temp=temp*625;
	return temp;
}

void Timer0Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
}

void send_wave(void)
{
	uchar i=8;
	do
	{
		TX=1;
		sound_nop;sound_nop;sound_nop;sound_nop;sound_nop;sound_nop;sound_nop;sound_nop;sound_nop;sound_nop;
		TX=0;
		sound_nop;sound_nop;sound_nop;sound_nop;sound_nop;sound_nop;sound_nop;sound_nop;sound_nop;sound_nop;
	}
	while(i--);
}

unsigned char distance_get(void)
{
	TL0=0;TH0=0;
	send_wave();
	TR0=1;
	while((RX==1)&&(TF0==0));
	TR0=0;
	if(TF0==1)
	{
		TF0=0;
		distance=99;
	}
	else 
	{
		sound_time=TH0;
		sound_time<<=8;
		sound_time|=TL0;
		distance=(uint)(sound_time*0.017);
		distance=distance/12;
	}
	return distance;
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
