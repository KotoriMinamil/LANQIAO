#include <STC15F2K60S2.h>
#include <intrins.h>

#define uchar unsigned char
#define uint unsigned int
	
#define control P2|=0xA0;P2&=0xBF;
#define wei_control P2|=0xC0;P2&=0xDF;
#define duan_control P2|=0xE0;P2&=0xFF;
#define led_control P2|=0x80;P2&=0x9F;

sbit DQ = P1^4;  //单总线接口
uchar code duanma[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0xFF};

uchar one,two,three,four,five,six,seven,eight;

void display4(uchar seven,uchar eight);
void display3(uchar five,uchar six);
void display2(uchar three,uchar four);
void display1(uchar one,uchar two);
void Timer0Init(void);
void delay(uint xms);
void init(void);
void keyscanf16(void);
uchar DS18B20_change();

uchar Tmax=30,Tmin=20,zhuangtai,wendu,num;
bit setting=0,delete_flag=0,key_flag=0,L1_flash=0;
uint time0_flag=0;
void main(void)
{
	init();
	Timer0Init();
	EA=1;ET0=1;
	while(1)
	{	
		if(setting==0)
		{
			wendu=DS18B20_change();
			if(wendu<Tmin)
			{
				zhuangtai=0;
				one=10;two=0;three=10;four=11;five=11;six=11;seven=wendu/10;eight=wendu%10;
				control;
				P0=0x00;
			}
			else if(wendu>Tmax)
			{
				zhuangtai=2;
				one=10;two=2;three=10;four=11;five=11;six=11;seven=wendu/10;eight=wendu%10;
				control;
				P0=0x10;
			}
			else
			{
				zhuangtai=1;
				one=10;two=1;three=10;four=11;five=11;six=11;seven=wendu/10;eight=wendu%10;
				control;
				P0=0x00;
			}
		}
		else if(setting==1)
		{
			if(delete_flag==1)
			{
				one=10;two=11;three=11;four=11;five=11;six=10;seven=11;eight=11;
				delete_flag=0;
			}
			if((two==11)&&(key_flag==1))
			{
				two=num;
				key_flag=0;
			}
			else if((three==11)&&(key_flag==1))
			{
				three=num;
				key_flag=0;
			}
			else if((seven==11)&&(key_flag==1))
			{
				seven=num;
				key_flag=0;
			}
			else if((eight==11)&&(key_flag==1))
			{
				eight=num;
				key_flag=0;
				if((two*10+three)<=(seven*10+eight))
				{
					one=10;two=11;three=11;four=11;five=11;six=10;seven=11;eight=11;
					led_control;
					P0=0xFD;
				}
			}
		}
		
		keyscanf16();
		display1(one,two);
		display2(three,four);
		display3(five,six);
		display4(seven,eight);
	}
}

void time0_isr() interrupt 1
{
	time0_flag++;
	if((time0_flag==40)&&(zhuangtai==2))
	{
		time0_flag=0;
		if(L1_flash==0)
		{
			led_control;
			P0=0xFE;
			L1_flash=1;
		}
		else if(L1_flash==1)
		{
			led_control;
			P0=0xFF;
			L1_flash=0;
		}
	}
	else if((time0_flag==80)&&(zhuangtai==1))
	{
		time0_flag=0;
		if(L1_flash==0)
		{
			led_control;
			P0=0xFE;
			L1_flash=1;
		}
		else if(L1_flash==1)
		{
			led_control;
			P0=0xFF;
			L1_flash=0;
		}
	}
	else if((time0_flag==120)&&(zhuangtai==0))
	{
		time0_flag=0;
		if(L1_flash==0)
		{
			led_control;
			P0=0xFE;
			L1_flash=1;
		}
		else if(L1_flash==1)
		{
			led_control;
			P0=0xFF;
			L1_flash=0;
		}
	}
}

void keyscanf16(void)
{
	uchar temp;
	if(setting==1)
	{
			P3=0xFF;P4=0xEF;
		temp=P3;
		temp&=0x0F;
		if(temp!=0x0F)
		{
			delay(2);
			temp=P3;
			temp&=0x0F;
			if(temp!=0x0F)
			{
				temp=P3;
				switch(temp)
				{
					case 0xFE:num=0;key_flag=1;break;
					case 0xFD:num=3;key_flag=1;break;
					case 0xFB:num=6;key_flag=1;break;
					case 0xF7:num=9;key_flag=1;break;
				}
				while(temp!=0x0F)
				{
					temp=P3;
					temp&=0x0F;
				}
			}
		}
		P3=0xDF;P4=0xFF;
		temp=P3;
		temp&=0x0F;
		if(temp!=0x0F)
		{
			delay(2);
			temp=P3;
			temp&=0x0F;
			if(temp!=0x0F)
			{
				temp=P3;
				switch(temp)
				{
					case 0xDE:num=2;key_flag=1;break;
					case 0xDD:num=5;key_flag=1;break;
					case 0xDB:num=8;key_flag=1;break;
					case 0xD7:delete_flag=1;break;
				}
				while(temp!=0x0F)
				{
					temp=P3;
					temp&=0x0F;
				}
			}
		}
	}
	
	P3=0xFF;P4=0xFB;
	temp=P3;
	temp&=0x0F;
	if(temp!=0x0F)
	{
		delay(2);
		temp=P3;
		temp&=0x0F;
		if(temp!=0x0F)
		{
			temp=P3;
			switch(temp)
			{
				case 0xFE:if(setting==1){num=1;key_flag=1;}break;
				case 0xFD:if(setting==1){num=4;key_flag=1;}break;
				case 0xFB:if(setting==1){num=7;key_flag=1;}break;
				case 0xF7:if(setting==0){setting=1;one=10;two=11;three=11;four=11;five=11;six=10;seven=11;eight=11;led_control;P0=0xFF;TR0=0;}
									else if(setting==1){Tmax=two*10+three;Tmin=seven*10+eight;led_control;P0=0xFF;setting=0;led_control;P0=0xFF;time0_flag=0;TR0=1;}break;
			}
			while(temp!=0x0F)
			{
				temp=P3;
				temp&=0x0F;
			}
		}
	}
	
	P3=0xEF;P4=0xFF;
	temp=P3;
	temp&=0x0F;
	if(temp!=0x0F)
	{
		delay(2);
		temp=P3;
		temp&=0x0F;
		if(temp!=0x0F)
		{
			temp=P3;
			switch(temp)
			{
				case 0xEE:/*S19*/;break;
				case 0xED:/*S18*/;break;
				case 0xEB:/*S17*/;break;
				case 0xE7:/*S16*/;break;
			}
			while(temp!=0x0F)
			{
				temp=P3;
				temp&=0x0F;
			}
		}
	}
}


void display1(uchar one,uchar two)
{
	wei_control;
	P0=0x01;
	duan_control;
	P0=duanma[one];
	delay(1);
	wei_control;
	P0=0x02;
	duan_control;
	P0=duanma[two];
	delay(1);
}

void display2(uchar three,uchar four)
{
	wei_control;
	P0=0x04;
	duan_control;
	P0=duanma[three];
	delay(1);
	wei_control;
	P0=0x08;
	duan_control;
	P0=duanma[four];
	delay(1);
}

void display3(uchar five,uchar six)
{
	wei_control;
	P0=0x10;
	duan_control;
	P0=duanma[five];
	delay(1);
	wei_control;
	P0=0x20;
	duan_control;
	P0=duanma[six];
	delay(1);
}

void display4(uchar seven,uchar eight)
{
	wei_control;
	P0=0x40;
	duan_control;
	P0=duanma[seven];
	delay(1);
	wei_control;
	P0=0x80;
	duan_control;
	P0=duanma[eight];
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

uchar DS18B20_change()
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

void Timer0Init(void)		//5毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x00;		//设置定时初值
	TH0 = 0x28;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}
