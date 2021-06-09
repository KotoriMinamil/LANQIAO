#include <intrins.h>
#include <stc15f2k60s2.h>

#define uchar unsigned char
#define uint unsigned int
	
#define control P2|=0XA0;P2&=0XBF;
#define wei_control P2|=0xC0;P2&=0xDF;
#define duan_control P2|=0xE0;P2&=0xFF;
#define led_control P2|=0x80;P2&=0x9F;

#define DELAY_TIME 40

#define sound_somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}

#define ADD 0x70

sbit SDA = P2^1;  /* 数据线 */
sbit SCL = P2^0;  /* 时钟线 */

sbit TX=P1^0;
sbit RX=P1^1;

uchar code duanma[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0xFF,0xC6,0x8E};
																																							//12'C'  'F'
uchar display[8]={1,2,3,4,5,6,7,8};//此数组下标0~7分别对应第1~8位数码管。
uchar discount=0;					//用于计数点亮到第几个数码管

bit S7_down=0;S6_down=0;S5_down=0;S4_down=0;

uint time0_flag=0,sound_time=0,distance=0;

void delay(uint xms);
void init(void);
void Timer0Init(void);
void Timer1Init(void);
void Timer2Init(void);

void IIC_DA_out(uchar dat);
uchar IIC_EEPROM_READ(uchar add);
void IIC_EEPROM_WRITE(uchar add,uchar dat);

void distance_get(void);

void keyscanf(void);
uchar distance_save[4]={0,0,0,0};

uchar warning=20,distance_count=0,display_page=0,c=0,return_count=0,LED_count=0,LED_flash=0xFF;

bit LED_flag=0,start_flag=0;

void main(void)
{
	init();
	Timer0Init();
	Timer1Init();
	Timer2Init();
	for(c=0;c<4;c++)
	{
		distance_save[c]=IIC_EEPROM_READ(ADD+c);delay(5);
	}
	warning=IIC_EEPROM_READ(ADD+4);
 	distance_count=IIC_EEPROM_READ(ADD+5);
	
	while(1)
	{
		if(display_page==0)//测距显示
		{
			if(start_flag==0)//上电数组索引指示，否则会指向索引2而不是末尾的索引3
			{
				display[0]=12;display[1]=11;
				display[2]=distance/100;display[3]=distance%100/10;display[4]=distance%10;
				display[5]=distance_save[3]/100;display[6]=distance_save[3]%100/10;display[7]=distance_save[3]%10;
			}
			else
			{
				display[0]=12;display[1]=11;
				display[2]=distance/100;display[3]=distance%100/10;display[4]=distance%10;
				switch(distance_count)//存在可优化空间
				{
					case 0:display[5]=distance_save[2]/100;display[6]=distance_save[2]%100/10;display[7]=distance_save[2]%10;break;
					case 1:display[5]=distance_save[3]/100;display[6]=distance_save[3]%100/10;display[7]=distance_save[3]%10;break;
					case 2:display[5]=distance_save[0]/100;display[6]=distance_save[0]%100/10;display[7]=distance_save[0]%10;break;
					case 3:display[5]=distance_save[1]/100;display[6]=distance_save[1]%100/10;display[7]=distance_save[1]%10;break;
					default:display[5]=distance_save[2]/100;display[6]=distance_save[2]%100/10;display[7]=distance_save[2]%10;break;
				}
			}
			
		}
		else if(display_page==1)//数据回显
		{
			display[0]=return_count+1;display[1]=11;display[2]=11;display[3]=11;display[4]=11;
			display[5]=distance_save[return_count]/100;display[6]=distance_save[return_count]%100/10;display[7]=distance_save[return_count]%10;
		}
		else if(display_page==2)//参数设定
		{
			display[0]=13;display[1]=11;display[2]=11;display[3]=11;display[4]=11;display[5]=11;
			display[6]=warning/10;display[7]=warning%10;
		}
		keyscanf();
	}
}

void keyscanf(void)				//本函数中所有延时的作用均为消抖
{
	if(P30==0)
	{
		delay(5);
		if(P30==0)
		{
			S7_down=1;			//判定S7按下
		}
	}
	else if(P31==0)
	{
		delay(5);
		if(P31==0)
		{
			S6_down=1;			//判定S6按下
		}
	}
	else if(P32==0)
	{
		delay(5);
		if(P32==0)
		{
			S5_down=1;			//判定S5按下
		}
	}
	else if(P33==0)
	{
		delay(5);
		if(P33==0)
		{
			S4_down=1;			//判定S4按下
		}
	}
	if((S7_down==1)&&(P30==1))	//判定S7按下过，并已松开
	{
		delay(5);
		if(P30==1)
		{
			S7_down=0;			//恢复S7按下标志位
			if(display_page==1)
			{
				return_count++;    //回显标志位翻页
				if(return_count==4)
				{
					return_count=0;
				}
			}
			else if(display_page==2)
			{
				warning+=10;
				if(warning==40)
				{
					warning=0;
				}
				IIC_EEPROM_WRITE(ADD+4,warning);delay(5);
			}
				//function
		}
	}
	else if((S6_down==1)&&(P31==1))
	{
		delay(5);
		if(P31==1)
		{
			S6_down=0;			//恢复S6按下标志位
			if(display_page!=2)
			{
				led_control;
				LED_flash&=0xBF;
				LED_flash|=0x80;
				P0=LED_flash;
				display_page=2;
			}
			else
			{
				led_control;
				LED_flash=0xFF;
				P0=LED_flash;
				display_page=0;
			}
				//function
		}
	}
	else if((S5_down==1)&&(P32==1))
	{
		delay(5);
		if(P32==1)
		{
			S5_down=0;			//恢复S5按下标志位
			if(display_page==0)
			{
				led_control;
				LED_flash&=0x7F;
				LED_flash|=0x40;
				P0=LED_flash;
				display_page=1;
				return_count=0;
			}
			else if(display_page==1)
			{
				led_control;
				LED_flash=0xFF;
				P0=LED_flash;
				display_page=0;
			}
				//function
		}
	}
	else if((S4_down==1)&&(P33==1))
	{
		delay(5);
		if(P33==1)
		{
			S4_down=0;			//恢复S4按下标志位
			if(display_page==0)
			{
				distance_get();
				start_flag=1;
				if(distance_count>=4) //FIF0
				{
					distance_save[0]=distance_save[1];
					distance_save[1]=distance_save[2];
					distance_save[2]=distance_save[3];
					distance_save[3]=distance;
				}
				else
				{
					distance_save[distance_count++]=distance;
				}
				for(c=0;c<4;c++)
				{
					IIC_EEPROM_WRITE(ADD+c,distance_save[c]);
					delay(5);
				}
				IIC_EEPROM_WRITE(ADD+5,distance_count);delay(5);
			}
				//function
		}
	}
}

void Timer1Init(void)		
{
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
}

void send_wave(void)
{
	uchar i=8;
	do
	{
		TX=1;
		sound_somenop;sound_somenop;sound_somenop;sound_somenop;sound_somenop;
		sound_somenop;sound_somenop;sound_somenop;sound_somenop;sound_somenop;
		
		TX=0;
		sound_somenop;sound_somenop;sound_somenop;sound_somenop;sound_somenop;
		sound_somenop;sound_somenop;sound_somenop;sound_somenop;sound_somenop;
	}
	while(i--);
}

void distance_get(void)
{
	uint temp;
	send_wave();					//发送波形函数
	TR1=1;								//启动定时器1，开始计时
	while((RX==1)&&(TF1==0))			//发送波形完成后使用一段死循环等待回波，收到回波后RX引脚会被拉低，从而跳出循环
	{															//如果定时器1计数值溢出，则也跳出循环，防止在检测超范围时程序跑飞。
																//如果不限制更新数据的时间，程序运行的大部分时间会在跑这段循环。
			//下面这段代码是为了照顾使用软件延时进行数码管显示
			//程序在跑入死循环等待回波时，main函数while(1)中的显示函数将不会被运行
			//此时可以观察到的现象大概为数码管闪烁
			//解决方法如下，当定时器1计时到一定时间时，运行一次显示函数
//			if((TH1==0x40)|(TH1==0x80)|(TH1==0xA0)|(TH1==0xE0))
//			{
//				display1(one,two);display2(three,four);display3(five,six);display4(seven,eight);
//			}
	}
	TR1=0;							//收到回波或定时器1计数值溢出，令定时器1停止计时。
	if(TF1==1)					//对定时器1的溢出标志位进行判断
	{
		TF1=0;						//将定时器1的溢出标志位重新置0
		distance=9999;		//9999 代表无返回，溢出
	}
	else
	{
		sound_time=TH1;		
		sound_time<<=8;
		sound_time|=TL1;	//将定时器1中的计数值取出
		
		distance=(uint)(sound_time*0.017);
		//此处的公式为，d=t/1000000*340/2*100
		//定时器的计数值增加方式为，每经过一个机器周期，计数器中值加1，直到计数器中的值溢出。
		//以STC89C52单片机工作在12MHz下为例，52单片机中，一个机器周期=12个晶振周期，12M/12=1M，除以1M得到具体时间，单位为S。
		//声音的传播速度约为340m/s
		//因为测得的时间是超声波往返一起花费的时间，所以要除以2。
		//这样计算出来后单位为m，因为题目中一般要求为cm，所以乘以100。
		
		distance=distance/12;
		//上方注释的计算是以52单片机为例的，但是在IAP15F2K61S2单片机中，一个机器周期=一个晶振周期。
		//所以当两者都处于12MHz下时，15单片机比52单片机快12倍，这里将多的12倍除去。
	}
	if(distance<=warning)
	{
		IIC_DA_out(0);
	}
	else if(distance>warning)
	{
		temp=(distance-warning)*256*0.02/5;
		if(temp>=255)
		{
			IIC_DA_out(255);
		}
		else
		{
			IIC_DA_out(temp);
		}
	}
	led_control;
	LED_flash&=0xFE;
	LED_flash|=0xFE;
	P0=LED_flash;
	LED_flag=1;
	LED_count=0;
	TH1=0;	//将定时器1的计数值清零
	TL1=0;
}

void Timer0Init(void)		//5毫秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xA0;		//设置定时初值
	TH0 = 0x15;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0=1;
}

void tim0_isr() interrupt 1
{
	if(LED_flag==1)
	{
		time0_flag++;
		if(time0_flag==200)
		{
			led_control;
			LED_flash|=0x01;
			P0=LED_flash;
			LED_count++;
		}
		else if(time0_flag==400)
		{
			time0_flag=0;
			led_control;
			LED_flash&=0xFE;
			LED_flash|=0xFE;
			P0=LED_flash;
		}
		if(LED_count==3)
		{
			LED_flag=0;
		}
	}
}

void Timer2Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = 0x20;		//设置定时初值
	T2H = 0xD1;		//设置定时初值
	AUXR |= 0x10;		//定时器2开始计时
    IE2 |= 0x04;	//开启定时器2中断
    EA = 1;			//打开总中断
}

void tim2_isr() interrupt 12 //定时器2的中断服务号为12
{
    wei_control;
    P0=(1<<discount);			//0000 0001→0000 0010以此类推
    duan_control;
    P0=duanma[display[discount]];	//段码
    if(++discount==8)discount=0;//计数值清零，以重新从第一位数码管开始
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

void IIC_DA_out(uchar dat)
{
	IIC_Start();
	IIC_SendByte(0x90);		//cmd write
	IIC_WaitAck();
	IIC_SendByte(0x40);		//cmd ANALOGUE OUTPUT ENABLE
	IIC_WaitAck();
	IIC_SendByte(dat);
	IIC_WaitAck();
	IIC_Stop();
}

void IIC_EEPROM_WRITE(uchar add,uchar dat)
{
	IIC_Start();
	IIC_SendByte(0xA0);		//cmd write
	IIC_WaitAck();
	IIC_SendByte(add);		//要写入的地址
	IIC_WaitAck();
	IIC_SendByte(dat);		//要写入的数据
	IIC_WaitAck();
	IIC_Stop();
}

uchar IIC_EEPROM_READ(uchar add)
{
	uchar dat;
	IIC_Start();
	IIC_SendByte(0xA0);		//cmd write
	IIC_WaitAck();
	IIC_SendByte(add);		//要读取的地址
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(0xA1);		//cmd read
	IIC_WaitAck();
	dat=IIC_RecByte();		//读出数据
	IIC_Stop();
	return dat;
}

void init(void)
{
    control;
    P0=0x00;	//关闭蜂鸣器&继电器
    wei_control;
    P0=0xFF;	//选中所有数码管
    duan_control;
    P0=0xFF;	//关闭所有数码管
    led_control;
    P0=0xFF;	//关闭所有LED
}

void delay(uint xms)//毫秒级延时，传入参数5则延时5ms，没有定时器准，一般用于数码管以及IIC连续调用之中。
{
    uint i,j;
    for(i=xms;i>0;i--)
        for(j=845;j>0;j--);
}