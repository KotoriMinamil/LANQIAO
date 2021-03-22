#include <STC15F2K60S2.h>
#include <intrins.h>

#define uchar unsigned char
#define uint unsigned int
	
#define control P2|=0xA0;P2&=0xBF;
#define wei_control P2|=0xC0;P2&=0xDF;
#define duan_control P2|=0xE0;P2&=0xFF;
#define led_control P2|=0x80;P2&=0x9F;


sbit SCLK=P1^7;		
sbit IO=P2^3;		
sbit CE = P1^3;   // DS1302复位
sbit DQ = P1^4;  //单总线接口
uchar code led[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0xFF};
uchar time[7]={50,59,23,0,0,0,0};
uchar one,two,three,four,five,six,seven,eight;
bit S7_down,S6_down,S5_down,S4_down;


void init(void);
void delay(uint xms);

void display1(uchar one,uchar two);
void display2(uchar three,uchar four);
void display3(uchar five,uchar six);
void display4(uchar seven,uchar eight);
void keyscanf(void);
void DS1302_readtime(void);
void DS1302_init(void);
uchar DS18B20_change(void);

void main(void)
{
	init();
	while(1)
	{
		
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
			S5_down=1;
		}
	}
	if((S7_down==1)&&(P30==1))
	{
		delay(5);
		if(P30==1)
		{
			S7_down=0;
			/*function*/
		}
	}
	else if((S6_down==1)&&(P31==1))
	{
		delay(5);
		if(P31==1)
		{
			S6_down=0;
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
	Delay_OneWire(5);
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

uchar DS18B20_change(void)
{
	uchar i,temp;
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

void DS1302_writebyte(uchar dat)
{
	uchar i;
	for(i=0;i<8;i++)
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
	temp=(dat<<4/10)|(dat%10);
	DS1302_writebyte(temp);
	CE=0;
}

uchar DS1302_read(uchar add)
{
	uchar i,temp;
	uchar data1,data2;
	CE=0;
	SCLK=0;
	CE=1;
	DS1302_writebyte(add);
	for(i=0;i<8;i++)
	{
		SCLK=0;
		temp>>=1;
		if(IO){temp|=0x80;}
		SCLK=1;
	}
	IO=0;
	data1=temp/16;
	data2=temp%16;
	temp=data1*10;data2;
	return temp;
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
	