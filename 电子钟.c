#include <STC15F2K60S2.h>
#include <intrins.h>

#define control P2|=0xA0;P2&=0xBF;
#define wei_control P2|=0xC0;P2&=0xDF;
#define duan_control P2|=0xE0;P2&=0xFF;
#define led_control P2|=0x80;P2&=0x9F;

#define uchar unsigned char
#define uint unsigned int
	
bit S7_down,S6_down,S5_down,S4_down;
bit add=0,sub=0,wendu_flag=0,led_flash=0,led_flag=0;	
sbit DQ = P1^4;  //单总线接口

sbit SCLK=P1^7;		
sbit IO=P2^3;		
sbit CE = P1^3;   // DS1302复位

uchar code led[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0xFF,0xC6/*C*/};

uchar one,two,three,four,five,six,seven,eight;
uchar naozhong_set=0,time_set=0;
uchar time[7]={50,59,23,0,0,0,0};
uchar naozhong_time[3]={0,0,0};

void display1(uchar one,uchar two);
void display2(uchar three,uchar four);
void display3(uchar five,uchar six);
void display4(uchar seven,uchar eight);
void init(void);
void delay(uint xms);
void keyscanf(void);
uchar DS18B20_change(void);
void DS1302_init(void);
void DS1302_readtime(void);
void Timer0Init(void);
uchar wendu;
uint led_time=0,time0_flag=0;
void main(void)
{
	init();
	DS1302_init();
	while(1)
	{
		DS1302_readtime();
		wendu=DS18B20_change();
		Timer0Init();
		ET0=1;
		EA=1;
		if(wendu_flag==1)
		{
			if(led_flag==0)
			{one=11;two=11;three=11;four=11;five=11;six=wendu/10;seven=wendu%10;eight=12;}
		}
		else if(wendu_flag==0)
		{
			if(naozhong_set==1)
			{
				if(time[0]%2==0)
				{
					one=naozhong_time[2]/10;two=naozhong_time[2]%10;three=10;four=naozhong_time[1]/10;five=naozhong_time[1]%10;six=10;seven=naozhong_time[0]/10;eight=naozhong_time[0]%10;	
				}
				else if(time[0]%2!=0)
				{
					one=11;two=11;three=10;four=naozhong_time[1]/10;five=naozhong_time[1]%10;six=10;seven=naozhong_time[0]/10;eight=naozhong_time[0]%10;
				}
				if((add==1)&&(naozhong_time[2]<23))
				{
					add=0;
					naozhong_time[2]++;
				}
				if((sub==1)&&(naozhong_time[2]>0))
				{
					sub=0;
					naozhong_time[2]--;
				}
			}
			else if(naozhong_set==2)
			{
				if(time[0]%2==0)
				{
					one=naozhong_time[2]/10;two=naozhong_time[2]%10;four=naozhong_time[1]/10;five=naozhong_time[1]%10;six=10;seven=naozhong_time[0]/10;eight=naozhong_time[0]%10;	
				}
				else if(time[0]%2!=0)
				{
					one=naozhong_time[2]/10;two=naozhong_time[2]%10;three=10;four=11;five=11;six=10;seven=naozhong_time[0]/10;eight=naozhong_time[0]%10;
				}
				if((add==1)&&(naozhong_time[1]<59))
				{
					add=0;
					naozhong_time[1]++;
				}
				if((sub==1)&&(naozhong_time[1]>0))
				{
					sub=0;
					naozhong_time[1]--;
				}
			}
			else if(naozhong_set==3)
			{
				if(time[0]%2==0)
				{
					one=naozhong_time[2]/10;two=naozhong_time[2]%10;four=naozhong_time[1]/10;five=naozhong_time[1]%10;six=10;seven=naozhong_time[0]/10;eight=naozhong_time[0]%10;	
				}
				else if(time[0]%2!=0)
				{
					one=naozhong_time[2]/10;two=naozhong_time[2]%10;three=10;four=naozhong_time[1]/10;five=naozhong_time[1]%10;six=10;seven=11;eight=11;
				}
				if((add==1)&&(naozhong_time[0]<59))
				{
					add=0;
					naozhong_time[0]++;
				}
				if((sub==1)&&(naozhong_time[0]>0))
				{
					sub=0;
					naozhong_time[0]--;
				}
			}
			if(time_set==1)
			{
				if(time[0]%2==0)
				{
					one=time[2]/10;two=time[2]%10;three=10;four=time[1]/10;five=time[1]%10;six=10;seven=time[0]/10;eight=time[0]%10;	
				}
				else if(time[0]%2!=0)
				{
					one=11;two=11;three=10;four=time[1]/10;five=time[1]%10;six=10;seven=time[0]/10;eight=time[0]%10;
				}
				if((add==1)&&(time[2]<23))
				{
					add=0;
					time[2]++;
					DS1302_init();
				}
				if((sub==1)&&(time[2]>0))
				{
					sub=0;
					time[2]--;
					DS1302_init();
				}
			}
			else if(time_set==2)
			{
				if(time[0]%2==0)
				{
					one=time[2]/10;two=time[2]%10;four=time[1]/10;five=time[1]%10;six=10;seven=time[0]/10;eight=time[0]%10;	
				}
				else if(time[0]%2!=0)
				{
					one=time[2]/10;two=time[2]%10;three=10;four=11;five=11;six=10;seven=time[0]/10;eight=time[0]%10;
				}
				if((add==1)&&(time[1]<59))
				{
					add=0;
					time[1]++;
					DS1302_init();
				}
				if((sub==1)&&(time[1]>0))
				{
					sub=0;
					time[1]--;
					DS1302_init();
				}
			}
			else if(time_set==3)
			{
				if(time[0]%2==0)
				{
					one=time[2]/10;two=time[2]%10;four=time[1]/10;five=time[1]%10;six=10;seven=time[0]/10;eight=time[0]%10;	
				}
				else if(time[0]%2!=0)
				{
					one=time[2]/10;two=time[2]%10;three=10;four=time[1]/10;five=time[1]%10;six=10;seven=11;eight=11;
				}
				if((add==1)&&(time[0]<59))
				{
					add=0;
					time[0]++;
					DS1302_init();
				}
				if((sub==1)&&(time[0]>0))
				{
					sub=0;
					time[0]--;
					DS1302_init();
				}
			}
			else if((time_set==0)&&(naozhong_set==0))
			{
				one=time[2]/10;two=time[2]%10;three=10;four=time[1]/10;five=time[1]%10;six=10;seven=time[0]/10;eight=time[0]%10;
			}
		}
		if((time[2]==naozhong_time[2])&&(time[1]==naozhong_time[1])&&(time[0]==naozhong_time[0]))
		{
			led_flag=1;
			led_control;
			P0=0xFE;
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
			if((time_set==0)&&(naozhong_set==0))
			wendu_flag=1;
		}
	}
	if((S7_down==1)&&(P30==1))
	{
		delay(5);
		if(P30==1)
		{
			S7_down=0;
			if(led_flag==1)
			{
				led_flag=0;
				led_control;
				P0=0xFF;
			}
			else if(naozhong_set==0)
			{
				switch(time_set)
				{
					case 0: time_set=1;break;
					case 1: time_set=2;break;
					case 2: time_set=3;break;
					case 3: time_set=0;break;
					default: break;
				}
			}
		}
	}
	else if((S6_down==1)&&(P31==1))
	{
		delay(5);
		if(P31==1)
		{
			S6_down=0;
			if(led_flag==1)
			{
				led_flag=0;
				led_control;
				P0=0xFF;
			}
			else if(time_set==0)
			{
				switch(naozhong_set)
				{
					case 0: naozhong_set=1;break;
					case 1: naozhong_set=2;break;
					case 2: naozhong_set=3;break;
					case 3: naozhong_set=0;break;
					default: break;
				}
			}
		}
	}
	else if((S5_down==1)&&(P32==1))
	{
		delay(5);
		if(P32==1)
		{
			S5_down=0;
			if(led_flag==1)
			{
				led_flag=0;
				led_control;
				P0=0xFF;
			}
			else if(naozhong_set!=0)
			{
				add=1;
			}
			else if(time_set!=0)
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
			S4_down=0;wendu_flag=0;
			if(led_flag==1)
			{
				led_flag=0;
				led_control;
				P0=0xFF;
			}
			else if((naozhong_set!=0)||(time_set!=0))
			{
				sub=1;
			}
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
	if(led_flag==1)
	{
		time0_flag++;
		if(time0_flag==40)
		{
			led_control;
			P0=0xFE;
			led_time++;
		}
		else if(time0_flag==80)
		{
			time0_flag=0;
			led_control;
			P0=0xFF;
			led_time++;
		}
		if(led_time==25)
		{
			led_flag=0;
			led_control;
			P0=0xFF;
		}
	}
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
	uchar data1,data2;
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
	data1=temp/16;
	data2=temp%16;
	temp=data1*10+data2;
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

uchar DS18B20_change(void)
{
	uchar low,high;
	uchar temp;
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