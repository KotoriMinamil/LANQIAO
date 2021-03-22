#include <STC15F2K60S2.h>
#include <intrins.h>
#include <stdio.h>
#define control P2|=0xA0;P2&=0xBF;
#define wei_control P2|=0xC0;P2&=0xDF;
#define duan_control P2|=0xE0;P2&=0xFF;
#define led_control P2|=0x80;P2&=0x9F;
#define DELAY_TIME 20
#define uchar unsigned char
#define uint unsigned int
sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */
sbit SCLK=P1^7;		
sbit IO=P2^3;		
sbit CE = P1^3;   // DS1302复位
sbit DQ = P1^4;  //单总线接口
uchar code led[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0xFF,0xC6/*C*/,0x89/*H*/};
uchar time[7]={55,59,23,0,0,0,0};
uchar one,two,three,four,five,six,seven,eight;
uchar buffer[6]={"111111"};
uchar buffer_tx[30]={"testtesttesttest"};
uchar text[6]={"AAASSS"};
bit S7_down=0;S6_down=0;S5_down=0;S4_down=0;
void keyscanf(void);
void init(void);
void delay(uint xms);
void display4(uchar seven,uchar eight);
void display3(uchar five,uchar six);
void display2(uchar three,uchar four);
void display1(uchar one,uchar two);
void UartInit(void);
void send(uchar *p);
uchar IIC_EEPROM_read(uchar add);
void IIC_EEPROM_write(uchar add,uchar dat);
void DS1302_readtime(void);
void DS1302_init(void);
uchar IIC_AD_read(uchar add);
uchar ds18b20_get(void);
void check(void);
void Timer0Init(void);
uint time0_flag=0;
uchar buffer_flag=0,light=0,display_page=0,near_flag=0,led_flash=0xFF;
uchar wendu,shidu,go_s,go_m,come_s,come_m,near_time,ADD=0x08;
bit mode=0,send_flag=0,check_flag=0,stop_flag=0;
//mode=0:autosend;mode=1:autosave;
void main(void)
{
	uchar i;
	init();
	DS1302_init();
	Timer0Init();
	UartInit();
	ET0=1;
	EA=1;
	ES=1;
	while(1)
	{
		DS1302_readtime();
		light=IIC_AD_read(0x01);light=IIC_AD_read(0x01);
		wendu=ds18b20_get();
		shidu=IIC_AD_read(0x03)*0.39;shidu=IIC_AD_read(0x03)*0.39;
		if(light<100)
		{near_flag=1;led_flash&=0xFB;led_control;P0=led_flash;}
		else
		{near_flag=0;led_flash|=0x04;led_control;P0=led_flash;}
		if(mode==0)
		{
			led_flash|=0x02;led_flash&=0xFE;led_control;P0=led_flash;
			if((check_flag==1)&&(send_flag==1))
			{
				send_flag=0;
				sprintf((char*)buffer_tx,"{%0.2bd-%0.2bd%%}{%0.2bd-%0.2bd-%0.2bd}{%bd}\r\n",wendu,shidu,time[2],time[1],time[0],near_flag);
				send(buffer_tx);
			}
		}
		else if(mode==1)
		{
			led_flash|=0x01;led_flash&=0xFD;led_control;P0=led_flash;
			if(near_flag==1)
			{
				if(stop_flag==0)
				{
					come_s=time[0];come_m=time[1];
					stop_flag=1;
				}
			}
			else
			{
				if(stop_flag==1)
				{
					stop_flag=0;
					go_s=time[0];go_m=time[1];
					if((go_m>=come_m)||(go_s>=come_s))
					{
						near_time=go_s-come_s;
						if(go_m>=come_m)
							near_time=near_time+(go_m-come_m)*60;
					}
					IIC_EEPROM_write(ADD,wendu);delay(5);ADD++;
					IIC_EEPROM_write(ADD,shidu);delay(5);ADD++;
					IIC_EEPROM_write(ADD,time[2]);delay(5);ADD++;
					IIC_EEPROM_write(ADD,time[1]);delay(5);ADD++;
					IIC_EEPROM_write(ADD,time[0]);delay(5);ADD++;
					IIC_EEPROM_write(ADD,near_time);delay(5);ADD++;
					if(ADD==0x37)ADD=0x08;
				}
			}
			if(send_flag==1)
			{
				send_flag=0;
				ADD=0x08;
				for(i=0;i<5;i++)
				{
					wendu=IIC_EEPROM_read(ADD);delay(5);ADD++;
					shidu=IIC_EEPROM_read(ADD);delay(5);ADD++;
					time[2]=IIC_EEPROM_read(ADD);delay(5);ADD++;
					time[1]=IIC_EEPROM_read(ADD);delay(5);ADD++;
					time[0]=IIC_EEPROM_read(ADD);delay(5);ADD++;
					near_time=IIC_EEPROM_read(ADD);delay(5);ADD++;
					sprintf((char*)buffer_tx,"{%0.2bd-%0.2bd%%}{%0.2bd-%0.2bd-%0.2bd}{%0.2bd}\r\n",wendu,shidu,time[2],time[1],time[0],near_time);
					send(buffer_tx);
				}
			}
		}
		if(display_page==0)
		{
			one=wendu/10;two=wendu%10;three=12;four=11;five=11;six=shidu/10;seven=shidu%10;eight=13;
		}
		if(display_page==1)
		{
			if(time[0]%2==0)
			{
				one=time[2]/10;two=time[2]%10;three=10;four=time[1]/10;five=time[1]%10;six=10;seven=time[0]/10;eight=time[0]%10;
			}
			else if(time[0]%2==1)
			{
				one=time[2]/10;two=time[2]%10;three=11;four=time[1]/10;five=time[1]%10;six=11;seven=time[0]/10;eight=time[0]%10;
			}
		}
		else if(display_page==2)
		{
			one=11;two=11;three=11;four=10;five=near_time/1000;six=near_time%1000/100;seven=near_time%100/10;eight=near_time%10;
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
	if((P30==1)&&(S7_down==1))
	{
		delay(5);
		if(P30==1)
		{
			S7_down=0;
			/*function*/
		}
	}
	else if((P31==1)&&(S6_down==1))
	{
		delay(5);
		if(P31==1)
		{
			S6_down=0;
			/*function*/
		}
	}
	else if((P32==1)&&(S5_down==1))
	{
		delay(5);
		if(P32==1)
		{
			S5_down=0;
			display_page++;
			if(display_page>2)
				display_page=0;
			/*function*/
		}
	}
	else if((P33==1)&&(S4_down==1))
	{
		delay(5);
		if(P33==1)
		{
			S4_down=0;
			if(mode==0)
				mode=1;
			else if(mode==1)
				mode=0;
			/*function*/
		}
	}
}

void buffer_isr() interrupt 4
{
	RI=0;
	buffer[buffer_flag++]=SBUF;
	if(buffer_flag>=6)
	{
		buffer_flag=0;
		check();
	}
}

void check(void)
{
	uchar i;
	for(i=0;i<6;i++)
	{
		if(buffer[i]!=text[i])
			check_flag=0;
		else
			check_flag=1;
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
	if(check_flag==1)
	{
		time0_flag++;
		if(time0_flag==200)
		{
			time0_flag=0;
			send_flag=1;
		}
	}
	else
		time0_flag=0;
}

void UartInit(void)		//1200bps@11.0592MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0x00;		//设定定时初值
	T2H = 0xF7;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
}

void send(uchar *p)
{
	uchar index=0;
	do
	{
		SBUF=p[index++];
		while(TI==0);
		TI=0;
	}
	while(p[index]!=0);
}

//单总线延时函数
void Delay_OneWire(unsigned int t)  //STC89C52RC
{
	uchar i;
	while(t--)
	{
		for(i=0;i<8;i++);
	}
}

//通过单总线向DS18B20写一个字节
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0;
		DQ = dat&0x01;
		Delay_OneWire(10);
		DQ = 1;
		dat >>= 1;
	}
	Delay_OneWire(10);
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
		Delay_OneWire(10);
	}
	return dat;
}

//DS18B20设备初始化
bit init_ds18b20(void)
{
  	bit initflag = 0;
  	
  	DQ = 1;
  	Delay_OneWire(24);
  	DQ = 0;
  	Delay_OneWire(160);
  	DQ = 1;
  	Delay_OneWire(20); 
    initflag = DQ;     
  	Delay_OneWire(10);
  
  	return initflag;
}

uchar ds18b20_get(void)
{
	uchar temp;
	uchar low,high;
	init_ds18b20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0x44);
	Delay_OneWire(300);
	init_ds18b20();
	Write_DS18B20(0xCC);
	Write_DS18B20(0xBE);
	low=Read_DS18B20();
	high=Read_DS18B20();
	temp=high<<4;
	temp|=(low>>4);
	return temp;
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

uchar IIC_AD_read(uchar add)
{
	uchar dat_AD;
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


