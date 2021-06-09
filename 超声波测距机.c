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

sbit SDA = P2^1;  /* ������ */
sbit SCL = P2^0;  /* ʱ���� */

sbit TX=P1^0;
sbit RX=P1^1;

uchar code duanma[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xBF,0xFF,0xC6,0x8E};
																																							//12'C'  'F'
uchar display[8]={1,2,3,4,5,6,7,8};//�������±�0~7�ֱ��Ӧ��1~8λ����ܡ�
uchar discount=0;					//���ڼ����������ڼ��������

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
		if(display_page==0)//�����ʾ
		{
			if(start_flag==0)//�ϵ���������ָʾ�������ָ������2������ĩβ������3
			{
				display[0]=12;display[1]=11;
				display[2]=distance/100;display[3]=distance%100/10;display[4]=distance%10;
				display[5]=distance_save[3]/100;display[6]=distance_save[3]%100/10;display[7]=distance_save[3]%10;
			}
			else
			{
				display[0]=12;display[1]=11;
				display[2]=distance/100;display[3]=distance%100/10;display[4]=distance%10;
				switch(distance_count)//���ڿ��Ż��ռ�
				{
					case 0:display[5]=distance_save[2]/100;display[6]=distance_save[2]%100/10;display[7]=distance_save[2]%10;break;
					case 1:display[5]=distance_save[3]/100;display[6]=distance_save[3]%100/10;display[7]=distance_save[3]%10;break;
					case 2:display[5]=distance_save[0]/100;display[6]=distance_save[0]%100/10;display[7]=distance_save[0]%10;break;
					case 3:display[5]=distance_save[1]/100;display[6]=distance_save[1]%100/10;display[7]=distance_save[1]%10;break;
					default:display[5]=distance_save[2]/100;display[6]=distance_save[2]%100/10;display[7]=distance_save[2]%10;break;
				}
			}
			
		}
		else if(display_page==1)//���ݻ���
		{
			display[0]=return_count+1;display[1]=11;display[2]=11;display[3]=11;display[4]=11;
			display[5]=distance_save[return_count]/100;display[6]=distance_save[return_count]%100/10;display[7]=distance_save[return_count]%10;
		}
		else if(display_page==2)//�����趨
		{
			display[0]=13;display[1]=11;display[2]=11;display[3]=11;display[4]=11;display[5]=11;
			display[6]=warning/10;display[7]=warning%10;
		}
		keyscanf();
	}
}

void keyscanf(void)				//��������������ʱ�����þ�Ϊ����
{
	if(P30==0)
	{
		delay(5);
		if(P30==0)
		{
			S7_down=1;			//�ж�S7����
		}
	}
	else if(P31==0)
	{
		delay(5);
		if(P31==0)
		{
			S6_down=1;			//�ж�S6����
		}
	}
	else if(P32==0)
	{
		delay(5);
		if(P32==0)
		{
			S5_down=1;			//�ж�S5����
		}
	}
	else if(P33==0)
	{
		delay(5);
		if(P33==0)
		{
			S4_down=1;			//�ж�S4����
		}
	}
	if((S7_down==1)&&(P30==1))	//�ж�S7���¹��������ɿ�
	{
		delay(5);
		if(P30==1)
		{
			S7_down=0;			//�ָ�S7���±�־λ
			if(display_page==1)
			{
				return_count++;    //���Ա�־λ��ҳ
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
			S6_down=0;			//�ָ�S6���±�־λ
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
			S5_down=0;			//�ָ�S5���±�־λ
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
			S4_down=0;			//�ָ�S4���±�־λ
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
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
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
	send_wave();					//���Ͳ��κ���
	TR1=1;								//������ʱ��1����ʼ��ʱ
	while((RX==1)&&(TF1==0))			//���Ͳ�����ɺ�ʹ��һ����ѭ���ȴ��ز����յ��ز���RX���Żᱻ���ͣ��Ӷ�����ѭ��
	{															//�����ʱ��1����ֵ�������Ҳ����ѭ������ֹ�ڼ�ⳬ��Χʱ�����ܷɡ�
																//��������Ƹ������ݵ�ʱ�䣬�������еĴ󲿷�ʱ����������ѭ����
			//������δ�����Ϊ���չ�ʹ�������ʱ�����������ʾ
			//������������ѭ���ȴ��ز�ʱ��main����while(1)�е���ʾ���������ᱻ����
			//��ʱ���Թ۲쵽��������Ϊ�������˸
			//����������£�����ʱ��1��ʱ��һ��ʱ��ʱ������һ����ʾ����
//			if((TH1==0x40)|(TH1==0x80)|(TH1==0xA0)|(TH1==0xE0))
//			{
//				display1(one,two);display2(three,four);display3(five,six);display4(seven,eight);
//			}
	}
	TR1=0;							//�յ��ز���ʱ��1����ֵ������ʱ��1ֹͣ��ʱ��
	if(TF1==1)					//�Զ�ʱ��1�������־λ�����ж�
	{
		TF1=0;						//����ʱ��1�������־λ������0
		distance=9999;		//9999 �����޷��أ����
	}
	else
	{
		sound_time=TH1;		
		sound_time<<=8;
		sound_time|=TL1;	//����ʱ��1�еļ���ֵȡ��
		
		distance=(uint)(sound_time*0.017);
		//�˴��Ĺ�ʽΪ��d=t/1000000*340/2*100
		//��ʱ���ļ���ֵ���ӷ�ʽΪ��ÿ����һ���������ڣ���������ֵ��1��ֱ���������е�ֵ�����
		//��STC89C52��Ƭ��������12MHz��Ϊ����52��Ƭ���У�һ����������=12���������ڣ�12M/12=1M������1M�õ�����ʱ�䣬��λΪS��
		//�����Ĵ����ٶ�ԼΪ340m/s
		//��Ϊ��õ�ʱ���ǳ���������һ�𻨷ѵ�ʱ�䣬����Ҫ����2��
		//�������������λΪm����Ϊ��Ŀ��һ��Ҫ��Ϊcm�����Գ���100��
		
		distance=distance/12;
		//�Ϸ�ע�͵ļ�������52��Ƭ��Ϊ���ģ�������IAP15F2K61S2��Ƭ���У�һ����������=һ���������ڡ�
		//���Ե����߶�����12MHz��ʱ��15��Ƭ����52��Ƭ����12�������ｫ���12����ȥ��
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
	TH1=0;	//����ʱ��1�ļ���ֵ����
	TL1=0;
}

void Timer0Init(void)		//5����@12.000MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0xA0;		//���ö�ʱ��ֵ
	TH0 = 0x15;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
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

void Timer2Init(void)		//1����@12.000MHz
{
	AUXR |= 0x04;		//��ʱ��ʱ��1Tģʽ
	T2L = 0x20;		//���ö�ʱ��ֵ
	T2H = 0xD1;		//���ö�ʱ��ֵ
	AUXR |= 0x10;		//��ʱ��2��ʼ��ʱ
    IE2 |= 0x04;	//������ʱ��2�ж�
    EA = 1;			//�����ж�
}

void tim2_isr() interrupt 12 //��ʱ��2���жϷ����Ϊ12
{
    wei_control;
    P0=(1<<discount);			//0000 0001��0000 0010�Դ�����
    duan_control;
    P0=duanma[display[discount]];	//����
    if(++discount==8)discount=0;//����ֵ���㣬�����´ӵ�һλ����ܿ�ʼ
}

void IIC_Delay(unsigned char i)
{
    do{_nop_();}
    while(i--);        
}
//������������
void IIC_Start(void)
{
    SDA = 1;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 0;
    IIC_Delay(DELAY_TIME);
    SCL = 0;	
}

//����ֹͣ����
void IIC_Stop(void)
{
    SDA = 0;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}

//�ȴ�Ӧ��
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

//ͨ��I2C���߷�������
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
	IIC_SendByte(add);		//Ҫд��ĵ�ַ
	IIC_WaitAck();
	IIC_SendByte(dat);		//Ҫд�������
	IIC_WaitAck();
	IIC_Stop();
}

uchar IIC_EEPROM_READ(uchar add)
{
	uchar dat;
	IIC_Start();
	IIC_SendByte(0xA0);		//cmd write
	IIC_WaitAck();
	IIC_SendByte(add);		//Ҫ��ȡ�ĵ�ַ
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(0xA1);		//cmd read
	IIC_WaitAck();
	dat=IIC_RecByte();		//��������
	IIC_Stop();
	return dat;
}

void init(void)
{
    control;
    P0=0x00;	//�رշ�����&�̵���
    wei_control;
    P0=0xFF;	//ѡ�����������
    duan_control;
    P0=0xFF;	//�ر����������
    led_control;
    P0=0xFF;	//�ر�����LED
}

void delay(uint xms)//���뼶��ʱ���������5����ʱ5ms��û�ж�ʱ��׼��һ������������Լ�IIC��������֮�С�
{
    uint i,j;
    for(i=xms;i>0;i--)
        for(j=845;j>0;j--);
}