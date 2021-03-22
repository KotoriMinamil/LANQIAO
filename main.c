#include <STC15F2K60S2.h>
#include <intrins.h>

#define control P2|=0xA0;P2&=0xBF;
#define wei_control P2|=0xC0;P2&=0xDF;
#define duan_control P2|=0xE0;P2&=0xFF;
#define led_control P2|=0x80;P2&=0x9F;

#define uchar unsigned char
#define uint unsigned int
	
#define DELAY_TIME 20
//总线引脚定义
sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */

uchar code led[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0xFF,0xC1/*U*/,0x8C/*P*/,0xC8/*n*/};

uchar one,two,three,four,five,six,seven,eight;
void init(void);
void display1(uchar one,uchar two);
void display2(uchar three,uchar four);
void display3(uchar five,uchar six);
void display4(uchar seven,uchar eight);
void delay(uint xms);
void keyscanf16(void);
uchar IIC_AD_read(void);
uchar IIC_EEPROM_read(uchar add);
void IIC_EEPROM_write(uchar add,uchar dat);
void Timer0Init(void);

bit chufa_flag=0,setting_page=0,add=0,sub=0,qingling=0,L1_flag=0,right_flag=0,start_flag=0;
uint volt_now=0,time0_flag=0;
uchar volt_save=0,error_flag=0;
float volt_set=3.0;
long number=0;
uchar display_page=0,led_flash=0xFF;
void main(void)
{
	init();
	Timer0Init();
	ET0=1;
	EA=1;
	volt_save=IIC_EEPROM_read(0x00);
	volt_set=(float)volt_save/10;
	while(1)
	{
		if(setting_page==1)
		{
			one=13;two=11;three=11;four=11;five=11;six=(uint)volt_set%10;seven=((uint)(volt_set*10)%10);eight=0;
			if(add==1)
			{
				if(volt_set<5.0)
				{
					volt_set=volt_set+0.5;
					volt_save=(uchar)(volt_set*10);
				}
				else 
				{
					volt_set=0.0;
					volt_save=0;
				}
				add=0;
			}
			if(sub==1)
			{
				if(volt_set>0.0)
				{
					volt_set=volt_set-0.5;
					volt_save=(uchar)(volt_set*10);
				}
				else
				{
					volt_set=5.0;
					volt_save=(uchar)(volt_set*10);
				}
				sub=0;
			}
		}
		else if(setting_page==0)
		{
			volt_now=1.96*IIC_AD_read();
			if((volt_now<=(int)(volt_set*100))&&(chufa_flag==0))
			{
				if(start_flag!=0)
				{
					number++;
					chufa_flag=1;
				}
			}
			else if((volt_now>(int)(volt_set*100))&&(chufa_flag==1))
			{
				start_flag=1;
				chufa_flag=0;
				L1_flag=0;
			}
			if(display_page==0)
			{
				one=12;two=11;three=11;four=11;five=11;six=volt_now/100;seven=volt_now%100/10;eight=volt_now%10;
			}
			else if(display_page==1)
			{
				if(qingling==1)
				{
					number=0;
					qingling=0;
				}
				if(number<10){one=14;two=11;three=11;four=11;five=11;six=11;seven=11;eight=number;}
				else if(number<100){one=14;two=11;three=11;four=11;five=11;six=11;seven=number%100/10;eight=number%10;}
				else if(number<1000){one=14;two=11;three=11;four=11;five=11;six=number/100;seven=number%100/10;eight=number%10;}
				else if(number<10000){one=14;two=11;three=11;four=11;five=number/1000;six=number%1000/100;seven=number%100/10;eight=number%10;}
				else if(number<100000){one=14;two=11;three=11;four=number/10000;five=number%10000/1000;six=number%1000/100;seven=number%100/10;eight=number%10;}
				else if(number<1000000){one=14;two=11;three=number/100000;four=number%100000/10000;five=number%10000/1000;six=number%1000/100;seven=number%100/10;eight=number%10;}
				else if(number<10000000){one=14;two=number/1000000;three=number%1000000/100000;four=number%100000/10000;five=number%10000/1000;six=number%1000/100;seven=number%100/10;eight=number%10;}
			}
		}
		if(error_flag>=3)
		{
			led_flash&=0xFB;
			led_control;
			P0=led_flash;
			P2=0x00;
		}
		else if(right_flag==1)
		{
			right_flag=0;
			led_flash=led_flash|0x04;
			led_control;
			P0=led_flash;
			P2=0x00;
		}
		if(number%2==1)
		{
			led_control;
			led_flash&=0xFD;
			P0=led_flash;
			P2=0x00;
		}
		else if(number%2==0)
		{
			led_flash|=0x02;
			led_control;
			P0=led_flash;
			P2=0x00;
		}
		
		
		
		
		
		keyscanf16();
		display1(one,two);
		display2(three,four);
		display3(five,six);
		display4(seven,eight);
	}
}

void keyscanf16(void)
{
	uchar temp;
	P3=0xFF;P4=0xEF;
	temp=P3;
	temp=temp&0x0F;
	if(temp!=0x0F)
	{
		delay(2);
		temp=P3;
		temp=temp&0x0F;
		if(temp!=0x0F)
		{
			temp=P3;
			switch(temp)
			{
				case 0xFE:error_flag++;/*S7*/break;
				case 0xFD:error_flag++;/*S6*/break;
				case 0xFB:error_flag++;/*S5*/break;
				case 0xF7:error_flag++;/*S4*/break;
			}
			while(temp!=0x0F)
			{
				temp=P3;
				temp=temp&0x0F;
			}
		}
	}
	P3=0xFF;P4=0xFB;
	temp=P3;
	temp=temp&0x0F;
	if(temp!=0x0F)
	{
		delay(2);
		temp=P3;
		temp=temp&0x0F;
		if(temp!=0x0F)
		{
			temp=P3;
			switch(temp)
			{
				case 0xFE:error_flag++;/*S11*/break;
				case 0xFD:error_flag++;/*S10*/break;
				case 0xFB:error_flag++;/*S9*/break;
				case 0xF7:error_flag++;/*S8*/break;
			}
			while(temp!=0x0F)
			{
				temp=P3;
				temp=temp&0x0F;
			}
		}
	}
	P3=0xDF;P4=0xFF;
	temp=P3;
	temp=temp&0x0F;
	if(temp!=0x0F)
	{
		delay(2);
		temp=P3;
		temp=temp&0x0F;
		if(temp!=0x0F)
		{
			temp=P3;
			switch(temp)
			{
				case 0xDE:error_flag++;/*S15*/break;
				case 0xDD:error_flag++;/*S14*/break;
				case 0xDB:if(display_page==1){qingling=1;error_flag=0;right_flag=1;}
									else error_flag++;/*S13*/break;
				case 0xD7:if((display_page==0)&&(setting_page==0)){setting_page=1;error_flag=0;right_flag=1;}
									else if((setting_page==1)&&(display_page==0)){display_page=1;setting_page=0;IIC_EEPROM_write(0x00,volt_save);delay(5);error_flag=0;right_flag=1;}
									else if(display_page==1){display_page=0;error_flag=0;right_flag=1;}/*S12*/break;
			}
			while(temp!=0x0F)
			{
				temp=P3;
				temp=temp&0x0F;
			}
		}
	}
	P3=0xEF;P4=0xFF;
	temp=P3;
	temp=temp&0x0F;
	if(temp!=0x0F)
	{
		delay(2);
		temp=P3;
		temp=temp&0x0F;
		if(temp!=0x0F)
		{
			temp=P3;
			switch(temp)
			{
				case 0xEE:error_flag++;/*S19*/break;
				case 0xED:error_flag++;/*S18*/break;
				case 0xEB:if(setting_page==1){sub=1;error_flag=0;right_flag=1;}
									else error_flag++;/*S17*/break;
				case 0xE7:if(setting_page==1){add=1;error_flag=0;right_flag=1;}
									else error_flag++;/*S16*/break;
			}
			while(temp!=0x0F)
			{
				temp=P3;
				temp=temp&0x0F;
			}
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
	if((setting_page==1)||(display_page==0))
		P0=led[six]&0x7F;
	else
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

void Timer0Init(void)		//5毫秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xA0;		//设置定时初值
	TH0 = 0x15;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

void time0_sir() interrupt 1
{
	if(chufa_flag==1)
	{
		time0_flag++;
		if(time0_flag==1000)
		{
			time0_flag=0;
			led_control;
			led_flash&=0xFE;
			P0=led_flash;
			P2=0x00;
		}
	}
	if(L1_flag==0)
	{
		L1_flag=1;
		time0_flag=0;
		led_control;
		led_flash|=0x01;
		P0=led_flash;
		P2=0x00;
	}
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

