#include <STC15F2K60S2.h>
#include <intrins.h>

#define sound_somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}
#define DELAY_TIME 20
#define control {P2|=0xA0;P2&=0xBF;}
#define wei_control {P2|=0xC0;P2&=0xDF;}
#define duan_control {P2|=0xE0;P2&=0xFF;}
#define led_control {P2|=0x80;P2&=0x9F;}
#define ADD 0x01
#define uchar unsigned char
#define uint unsigned int
sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */
sbit TX=P1^0;
sbit RX=P1^1;
sbit SCLK=P1^7;		
sbit IO=P2^3;		
sbit CE = P1^3;   // DS1302复位
	
uchar code led[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0xFF};
uchar time[7]={0,59,6,0,0,0,0};
uchar one,two,three,four,five,six,seven,eight;
uchar code password_origin[6]={6,5,4,3,2,1};
uchar password_now[6]={11,11,11,11,11,11};
uchar password_old[6]={11,11,11,11,11,11};
void delay(uint xms);
void init(void);
void display4(uchar seven,uchar eight);
void display3(uchar five,uchar six);
void display2(uchar three,uchar four);
void display1(uchar one,uchar two);
void Timer0Init(void);
void Timer1Init(void);
void send_wave(void);
void sound(void);
void DS1302_readtime(void);
void DS1302_init(void);
void keyscanf16(void);
uchar IIC_EEPROM_read(uchar add);
void IIC_EEPROM_write(uchar add,uchar dat);
void password_11(void);
void re_password(void);
void passpage_1(void);
void passpage_2(void);
uint time0_flag=0,distance=0,sound_time=0,open_time=0,buzzer_time=0;
bit sound_flag=0,open_flag,key_flag=0,time_flag=1,buzzer_flag=0,reset=0;
uchar num=0,error_flag=0,setting=0,sure_flag=0;
void main(void)
{
	uchar i;
	init();
	Timer0Init();
	Timer1Init();
	EA=1;
	ET0=1;
	DS1302_init();
	for(i=0;i<5;i++)
	{
		password_old[i]=IIC_EEPROM_read(ADD+i);
		delay(5);
	}
	while(1)
	{
		DS1302_readtime();
		if((time[2]>=7)&&(time[2]<22))
		{
			if(setting==0)
			{
				time_flag=1;
				sound();
				if(distance<30)
				{
					control;
					P0=0x10;
					open_flag=1;
				}
				one=time[2]/10;two=time[2]%10;three=10;four=time[1]/10;five=time[1]%10;six=10;seven=time[0]/10;eight=time[0]%10;
			}
			else if(setting==1)
			{
				if(time_flag==1)
				{
					one=11;two=10;three=11;four=11;five=11;six=11;seven=11;eight=11;
				}
				passpage_1();
			}
			else if(setting==2)
			{
				passpage_2();
			}
		}
		else
		{
			if(setting==0)
			{
				if(time_flag==1)
				{
					one=10;two=10;three=11;four=11;five=11;six=11;seven=11;eight=11;
				}
				if((three==11)&&(key_flag==1))
				{
					time_flag=0;
					key_flag=0;
					three=num;
					password_now[0]=three;
				}
				else if((four==11)&&(key_flag==1))
				{
					key_flag=0;
					four=num;
					password_now[1]=four;
				}
				else if((five==11)&&(key_flag==1))
				{
					key_flag=0;
					five=num;
					password_now[2]=five;
				}
				else if((six==11)&&(key_flag==1))
				{
					key_flag=0;
					six=num;
					password_now[3]=six;
				}
				else if((seven==11)&&(key_flag==1))
				{
					key_flag=0;
					seven=num;
					password_now[4]=seven;
				}
				else if((eight==11)&&(key_flag==1))
				{
					key_flag=0;
					eight=num;
					password_now[5]=eight;
				}
				if(sure_flag==1)
				{
					sure_flag=0;
					key_flag=0;
					for(i=0;i<5;i++)
					{
						if(password_now[i]!=password_old[i])
						{
							error_flag++;
							time_flag=1;
							break;
						}
						else
							sure_flag++;
					}
					if(sure_flag==5)
					{
						sure_flag=0;
						control;
						P0=0x10;
						open_flag=1;
						time_flag=1;
						password_11();
					}
				}
				if(error_flag>=3)
				{
					error_flag=0;
					buzzer_flag=1;
					control;
					P0=0x40;
					password_11();
				}
			}
			else if(setting==1)
			{
				passpage_1();
				if(time_flag==1)
				{
					one=11;two=10;three=11;four=11;five=11;six=11;seven=11;eight=11;
				}
			}
			else if(setting==2)
			{
				passpage_2();
			}
		}
		if(reset==1)
		{
			reset=0;
			re_password();
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
				case 0xFE:num=0;key_flag=1;/*S7*/break;
				case 0xFD:num=4;key_flag=1;/*S6*/break;
				case 0xFB:num=8;key_flag=1;/*S5*/break;
				case 0xF7:/*S4*/break;
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
				case 0xFE:num=1;key_flag=1;/*S11*/break;
				case 0xFD:num=5;key_flag=1;/*S10*/break;
				case 0xFB:num=9;key_flag=1;/*S9*/break;
				case 0xF7:/*S8*/break;
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
				case 0xDE:num=2;key_flag=1;/*S15*/break;
				case 0xDD:num=6;key_flag=1;/*S14*/break;
				case 0xDB:if(setting==0)
									{
										setting=1;
										one=11;two=10;three=11;four=11;five=11;six=11;seven=11;eight=11;
									}/*S13*/break;
				case 0xD7:if(eight!=11)sure_flag=1;/*querenS12*/break;
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
				case 0xEE:num=3;key_flag=1;/*S19*/break;
				case 0xED:num=7;key_flag=1;/*S18*/break;
				case 0xEB:reset=1;/*resetS17*/break;
				case 0xE7:setting=0;/*quitS16*/break;
			}
			while(temp!=0x0F)
			{
				temp=P3;
				temp=temp&0x0F;
			}
		}
	}
}

void sound(void)
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
				display1(one,two);
				display2(three,four);
				display3(five,six);
				display4(seven,eight);
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

void Timer0Init(void)		//2毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x9A;		//设置定时初值
	TH0 = 0xA9;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

void time0_isr() interrupt 1
{
	time0_flag++;
	if(time0_flag==500)
	{
		time0_flag=0;
		sound_flag=1;
	}
	if(open_flag==1)
	{
		open_time++;
		if(open_time==2500)
		{
			open_flag=0;
			open_time=0;
			control;
			P0=0x00;
		}
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

void Timer1Init(void)		//2毫秒@11.0592MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
}

void password_11(void)
{
	uchar i;
	for(i=0;i<5;i++)
	{
		password_now[i]=11;
	}
}

void re_password(void)
{
	uchar i;
	for(i=0;i<5;i++)
	{
		password_old[i]=password_origin[i];
	}
	for(i=0;i<5;i++)
	{
		IIC_EEPROM_write(ADD+i,password_old[i]);
		delay(5);
	}
}

void passpage_1(void)
{
	uchar i;
	if((three==11)&&(key_flag==1))
	{
		time_flag=0;
		key_flag=0;
		three=num;
		password_now[0]=three;
	}
	else if((four==11)&&(key_flag==1))
	{
		key_flag=0;
		four=num;
		password_now[1]=four;
	}
	else if((five==11)&&(key_flag==1))
	{
		key_flag=0;
		five=num;
		password_now[2]=five;
	}
	else if((six==11)&&(key_flag==1))
	{
		key_flag=0;
		six=num;
		password_now[3]=six;
	}
	else if((seven==11)&&(key_flag==1))
	{
		key_flag=0;
		seven=num;
		password_now[4]=seven;
	}
	else if((eight==11)&&(key_flag==1))
	{
		key_flag=0;
		eight=num;
		password_now[5]=eight;
	}
	if(sure_flag==1)
	{
		sure_flag=0;
		key_flag=0;
		for(i=0;i<5;i++)
		{
			if(password_now[i]!=password_old[i])
			{
				error_flag++;
				time_flag=1;
				break;
			}
			else
				sure_flag++;
		}
		if(sure_flag==5)
		{
			setting=2;
			one=10;two=11;three=11;four=11;five=11;six=11;seven=11;eight=11;
		}
	}
	if(error_flag>=3)
	{
		error_flag=0;
		buzzer_flag=1;
		control;
		P0=0x40;
		password_11();
		setting=0;
	}
}

void passpage_2(void)
{
	uchar i;
	if((three==11)&&(key_flag==1))
	{
		time_flag=0;
		key_flag=0;
		three=num;
		password_now[0]=three;
	}
	else if((four==11)&&(key_flag==1))
	{
		key_flag=0;
		four=num;
		password_now[1]=four;
	}
	else if((five==11)&&(key_flag==1))
	{
		key_flag=0;
		five=num;
		password_now[2]=five;
	}
	else if((six==11)&&(key_flag==1))
	{
		key_flag=0;
		six=num;
		password_now[3]=six;
	}
	else if((seven==11)&&(key_flag==1))
	{
		key_flag=0;
		seven=num;
		password_now[4]=seven;
	}
	else if((eight==11)&&(key_flag==1))
	{
		key_flag=0;
		eight=num;
		password_now[5]=eight;
	}
	if(sure_flag==1)
	{
		sure_flag=0;
		key_flag=0;
		for(i=0;i<5;i++)
		{
			password_old[i]=password_now[i];
		}
		for(i=0;i<5;i++)
		{
			IIC_EEPROM_write(ADD+i,password_old[i]);
			delay(5);
		}
		password_11();
		setting=0;
		time_flag=1;
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
