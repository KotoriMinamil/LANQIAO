#include <STC15F2K60S2.h>
#include <intrins.h>

#define DELAY_TIME 40

#define uchar unsigned char
#define uint unsigned int
	
#define control P2|=0xA0;P2&=0xBF;
#define wei_control P2|=0xC0;P2&=0xDF;
#define duan_control P2|=0xE0;P2&=0xFF;
#define led_control P2|=0x80;P2&=0x9F;
 
sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */
sbit DQ=P1^4;

uchar code led[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0xFF,0xC6/*C*/,0x8C/*P*/};
uchar one,two,three,four,five,six,seven,eight;

bit S7_down=0,S6_down=0,S5_down=0,S4_down=0;//S4界面切换 S5上下限切换 默认下限，S6+ S7-

void init(void);
void delay(uint xms);
void display1(uchar one,uchar two);
void display2(uchar three,uchar four);
void display3(uchar five,uchar six);
void display4(uchar seven,uchar eight);
void keyscanf(void);
uchar DS18B20_change(void);
void Timer0Init(void);
void IIC_AD_write(uchar dat);

bit addi=0,sub=0;
uchar setting=0,wendu=0,shangxian=30,xiaxian=20,shangxian_temp=0,xiaxian_temp=0,canshu_page=1,led_flash=0xFF;
uint time0_flag=0;

void main(void)
{
	init();
	Timer0Init();
	ET0=1;
	EA=1;
	while(1)
	{
		if(setting==0)
		{
			one=12;two=11;three=11;four=11;five=11;six=11;seven=wendu/10;eight=wendu%10;
			if(wendu>shangxian)
			{
				IIC_AD_write(204);
				led_flash&=0xFE;
				led_flash|=0x06;
				led_control;
				P0=led_flash;
				P2=0x00;P0=0xFF;
			}
			else if(wendu<xiaxian)
			{
				IIC_AD_write(102);
				led_flash&=0xFB;
				led_flash|=0x03;
				led_control;
				P0=led_flash;
				P2=0x00;P0=0xFF;
			}
			else
			{
				IIC_AD_write(153);
				led_flash&=0xFD;
				led_flash|=0x05;
				led_control;
				P0=led_flash;
				P2=0x00;P0=0xFF;
			}
		}
		else if(setting==1)
		{
			one=13;two=11;three=11;four=shangxian_temp/10;five=shangxian_temp%10;six=11;seven=xiaxian_temp/10;eight=xiaxian_temp%10;
			if(canshu_page==1)
			{
				if(addi==1)
				{
					addi=0;
					if(xiaxian_temp<99)
					{
						xiaxian_temp++;
					}
				}
				if(sub==1)
				{
					sub=0;
					if(xiaxian_temp!=0)
					{
						xiaxian_temp--;
					}
				}
			}
			else if(canshu_page==2)
			{
				if(addi==1)
				{
					addi=0;
					if(shangxian_temp<99)
					{
						shangxian_temp++;
					}
				}
				if(sub==1)
				{
					sub=0;
					if(shangxian_temp!=0)
					{
						shangxian_temp--;
					}
				}
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
			if(setting==1){sub=1;}
			/*function*/
		}
	}
	else if((S6_down==1)&&(P31==1))
	{
		delay(5);
		if(P31==1)
		{
			S6_down=0;
			if(setting==1){addi=1;}
			/*function*/
		}
	}
	else if((S5_down==1)&&(P32==1))
	{
		delay(5);
		if(P32==1)
		{
			S5_down=0;
			switch(canshu_page)
			{
				case 1: canshu_page=2;break;
				case 2: canshu_page=1;break;
				default: break;
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
			if(setting==0){shangxian_temp=shangxian;xiaxian_temp=xiaxian;canshu_page=1;setting=1;}
			else if(setting==1)
			{
				if(shangxian_temp>=xiaxian_temp)
				{
					shangxian=shangxian_temp;
					xiaxian=xiaxian_temp;
					led_flash|=0x08;
					led_control;
					P0=led_flash;
					P2=0x00;P0=0xFF;
				}
				else
				{
					led_flash&=0xF7;
					led_control;
					P0=led_flash;
					P2=0x00;P0=0xFF;
				}
				setting=0;canshu_page=0;
			}
			/*function*/
		}
	}
}

void Timer0Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x20;		//设置定时初值
	TH0 = 0xD1;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

void time0_isr() interrupt 1
{
	time0_flag++;
	if(time0_flag>=450)
	{
		time0_flag=0;
		if(setting==0)
		wendu=DS18B20_change();
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

void IIC_AD_write(uchar dat)
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

