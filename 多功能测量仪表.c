#include <STC15F2K60S2.h>
#include <intrins.h>
#define ADD 0x40
#define control P2|=0xA0;P2&=0xBF;
#define wei_control P2|=0xC0;P2&=0xDF;
#define duan_control P2|=0xE0;P2&=0xFF;
#define led_control P2|=0x80;P2&=0x9F;
#define DELAY_TIME 40
#define uchar unsigned char
#define uint unsigned int
sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */
sbit DQ = P1^4;  //单总线接口
uchar code duan[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,
									 0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10,
									 0xBF,0xFF,0xC1/*U*/,0x8E/*F*/,0xC6/*C*/,0x89/*H*/,0x8C/*P*/};
bit S7_down=0;S6_down=0;S5_down=0;S4_down=0;
uchar display[8]={11,11,11,11,11,11,11,11};
uchar dis_count=0;

void init(void);
void delay(uint xms);
void Timer2Init(void);
uchar IIC_AD_READ(void);
void Timer1Init(void);
void keyscanf(void);
long DS18B20_GET(void);
void IIC_EEPROM_WRITE(uchar add,uchar dat);
uchar IIC_EEPROM_READ(uchar add);

long wendu=0,E_wendu1=0,E_wendu2=0,E_wendu=0;
uint E_freq1=0,E_freq2=0,E_freq3=0,E_freq=0;
uint ne_time=0,high=0,high_now=0,down=0,down_now=0,freq=0,E_dianya=0;
uint dianya=0,ms_count=0,dianya_yuzhi16=0;
uchar display_page=0,AD=0,S6_time=0,key_double=0,dianya_yuzhi8=10,led_flash=0,E_AD=0,led=0xFF;
bit time1_flag=0,setting_flag=0,save_flag=0,warning_flag=0;
void main(void)
{
	init();
	Timer2Init();
	Timer1Init();
	dianya_yuzhi8=IIC_EEPROM_READ(ADD+6);delay(5);
	dianya_yuzhi16=dianya_yuzhi8*0.196;
	led|=0x07;led&=0xFD;led_control;P0=led;
	while(1)
	{
		if(setting_flag==0)
		{
			if(display_page<3)
			{
				switch(ms_count)
				{
					case 1:AD=IIC_AD_READ();dianya=AD*0.196;if((dianya>dianya_yuzhi16)&&(warning_flag==0)){led&=0x7F;led_control;P0=led;}
																									else if((dianya>dianya_yuzhi16)&&(warning_flag==1)){led|=0x80;led_control;P0=led;}
																									else if(dianya<=dianya_yuzhi16){led|=0x80;led_control;P0=led;}break;
					case 150:wendu=DS18B20_GET();break;
					case 250:TR1=1;ET1=1;break;
					case 300:TR1=0;ET1=0;ms_count=0;ne_time=(high_now+down_now)*5;freq=1000000/ne_time;break;
					default: break;
				}
			}
			
			switch(display_page)
			{
				case 0:display[0]=22;display[1]=21;display[2]=21;display[3]=21;display[4]=21;display[5]=21;display[6]=dianya/10+10;display[7]=dianya%10;break;
				case 1:display[0]=23;display[1]=21;display[2]=21;if(freq>=10000){display[3]=freq/10000;display[4]=freq%10000/1000;display[5]=freq%1000/100;display[6]=freq%100/10;display[7]=freq%10;}
																												 else if(freq>=1000){display[3]=21;display[4]=freq/1000;display[5]=freq%1000/100;display[6]=freq%100/10;display[7]=freq%10;}
																												 else if(freq>=100){display[3]=21;display[4]=21;display[5]=freq/100;display[6]=freq%100/10;display[7]=freq%10;}
																												 else if(freq>=10){display[3]=21;display[4]=21;display[5]=21;display[6]=freq/10;display[7]=freq%10;}break;
				case 2:display[0]=24;display[1]=21;display[2]=21;display[3]=21;display[4]=wendu/100000;display[5]=wendu%100000/10000+10;display[6]=wendu%10000/1000;display[7]=wendu%1000/100;break;
																												 
																												 
				case 3:display[0]=25;display[1]=22;display[2]=21;display[3]=21;display[4]=21;display[5]=21;display[6]=E_dianya/10+10;display[7]=E_dianya%10;break;
				case 4:display[0]=25;display[1]=23;display[2]=21;if(E_freq>=10000){display[3]=E_freq/10000;display[4]=E_freq%10000/1000;display[5]=E_freq%1000/100;display[6]=E_freq%100/10;display[7]=E_freq%10;}
																												 else if(E_freq>=1000){display[3]=21;display[4]=E_freq/1000;display[5]=E_freq%1000/100;display[6]=E_freq%100/10;display[7]=E_freq%10;}
																												 else if(E_freq>=100){display[3]=21;display[4]=21;display[5]=E_freq/100;display[6]=E_freq%100/10;display[7]=E_freq%10;}
																												 else if(E_freq>=10){display[3]=21;display[4]=21;display[5]=21;display[6]=E_freq/10;display[7]=E_freq%10;}break;
		    case 5:display[0]=25;display[1]=24;display[2]=21;display[3]=21;display[4]=E_wendu/100000;display[5]=E_wendu%100000/10000+10;display[6]=E_wendu%10000/1000;display[7]=E_wendu%1000/100;break;
			}
		}
		else if(setting_flag==1)
		{
			display[0]=26;display[1]=21;display[2]=21;display[3]=21;display[4]=21;display[5]=21;display[6]=dianya_yuzhi8/10+10;display[7]=dianya_yuzhi8%10;
		}
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
			while(P31==0)
			{
				delay(100);
				if(++S6_time>=8)
				{
					do
					{
						dianya_yuzhi8++;
						if(dianya_yuzhi8==51)
							dianya_yuzhi8=1;
					}
					while(!P31);
				}
			}
			if((S6_down==1)&&(key_double!=2))
			{
				S6_down=0;S6_time=0;key_double++;
				if((setting_flag==0)&&(display_page<3))
				{
					E_AD=IIC_EEPROM_READ(ADD);delay(5);
					E_wendu1=IIC_EEPROM_READ(ADD+1);delay(5);
					E_wendu2=IIC_EEPROM_READ(ADD+2);delay(5);
					E_freq1=IIC_EEPROM_READ(ADD+3);delay(5);
					E_freq2=IIC_EEPROM_READ(ADD+4);delay(5);
					E_freq3=IIC_EEPROM_READ(ADD+5);delay(5);
					E_dianya=E_AD*0.196;
					E_wendu=E_wendu1*1000+E_wendu2*100;
					E_freq=E_freq1*1000+E_freq2*100+E_freq3;
					display_page=display_page+3;
				}
				else if((setting_flag==0)&&(display_page>2))
				{
					display_page=display_page-3;
				}
				else if(setting_flag==1)
				{
					dianya_yuzhi8++;
					if(dianya_yuzhi8==51)
						dianya_yuzhi8=1;
				}
			}
			else if(key_double==2)
			{
				key_double=0;
			}
		}
		while(!P30);
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
			if((setting_flag==0)&&(display_page<3))
				setting_flag=1;
			else if(setting_flag==1)
			{
				setting_flag=0;
				IIC_EEPROM_WRITE(ADD+6,dianya_yuzhi8);delay(5);
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
			if(setting_flag==0)
			{
				save_flag=1;
				IIC_EEPROM_WRITE(ADD,AD);delay(5);
				IIC_EEPROM_WRITE(ADD+1,wendu/10000);delay(5);
				IIC_EEPROM_WRITE(ADD+2,wendu%10000/100);delay(5);
				IIC_EEPROM_WRITE(ADD+3,freq/10000);delay(5);
				IIC_EEPROM_WRITE(ADD+4,freq%10000/100);delay(5);
				IIC_EEPROM_WRITE(ADD+5,freq%100);delay(5);
				save_flag=0;
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
			if(setting_flag==0)
			{
				switch(display_page)
				{
					case 0:display_page++;led|=0x07;led&=0xFD;led_control;P0=led;break;
					case 1:display_page++;led|=0x07;led&=0xFB;led_control;P0=led;break;
					case 2:display_page=0;led|=0x07;led&=0xFE;led_control;P0=led;break;
					case 3:display_page++;break;
					case 4:display_page++;break;
					case 5:display_page=3;break;
					default: break;
				}
			}
//			else if(setting_flag==1)
//			{
//				display_page++;
//				if(display_page==6)
//					display_page=3;
//			}
			/*function*/
		}
	}
}

void Timer2Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0x20;		//设置定时初值
	T2H = 0xD1;		//设置定时初值
	AUXR |= 0x10;		//定时器2开始计时
	IE2|=0x04;
	EA=1;
}

void Timer1Init(void)		//5微秒@12.000MHz
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0xC4;		//设置定时初值
	TH1 = 0xFF;		//设置定时初值
	TF1 = 0;		//清除TF1标志
}

void tim2_isr(void) interrupt 12
{
	wei_control;
	P0=(1<<dis_count);
	duan_control;
	P0=duan[display[dis_count]];
	if(++dis_count==8)
		dis_count=0;
	if(save_flag==0)
	{
		ms_count++;
	}
	if(display_page<3)
	{
		led_flash++;
		if(led_flash==201)
		{
			led_flash=0;
			warning_flag=~warning_flag;
		}
	}
}

void tim1_isr(void) interrupt 3
{
	if(P34==0)
	{
		down++;
		if(time1_flag==0)
		{
			time1_flag=1;
			high_now=high;
			high=0;
		}
	}
	else if(P34==1)
	{
		high++;
		if(time1_flag==1)
		{
			time1_flag=0;
			down_now=down;
			down=0;
		}
	}
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
		Delay_OneWire(50);
		DQ = 1;
		dat >>= 1;
	}
	Delay_OneWire(50);
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
		Delay_OneWire(50);
	}
	return dat;
}

//DS18B20设备初始化
bit init_ds18b20(void)
{
  	bit initflag = 0;
  	
  	DQ = 1;
  	Delay_OneWire(120);
  	DQ = 0;
  	Delay_OneWire(800);
  	DQ = 1;
  	Delay_OneWire(100); 
    initflag = DQ;     
  	Delay_OneWire(50);
  
  	return initflag;
}

long DS18B20_GET(void)
{
	uchar low,high;
	long temp;
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

uchar IIC_EEPROM_READ(uchar add)
{
	uchar dat;
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
	dat=IIC_RecByte();
	return dat;
}

uchar IIC_AD_READ(void)
{
	uchar dat;
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
	dat=IIC_RecByte();
	return dat;
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

