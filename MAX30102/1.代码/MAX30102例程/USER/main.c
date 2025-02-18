#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "OLED.h"
#include "string.h" 	
#include "max30102.h"

/*****************���絥Ƭ�����******************
											STM32
 * ��Ŀ			:	MAX30102����Ѫ��������ʵ��                   
 * �汾			: V1.0
 * ����			: 2024.8.18
 * MCU			:	STM32F103C8T6
 * �ӿ�			:	��max30102.h							
 * BILIBILI	:	���絥Ƭ�����
 * CSDN			:	���絥Ƭ�����
 * ����			:	����

**********************BEGIN***********************/

#define MAX_BRIGHTNESS 255
#define INTERRUPT_REG 0X00

/* 	VCC<->3.3V
	GND<->GND
	SCL<->PB7
	SDA<->PB8
	INT<->PB9*/

uint32_t aun_ir_buffer[500]; 	 //IR LED   ��������ݣ����ڼ���Ѫ��
int32_t n_ir_buffer_length;    //���ݳ���
uint32_t aun_red_buffer[500];  //Red LED	������ݣ����ڼ������������Լ���������
int32_t n_sp02; //SPO2ֵ
int8_t ch_spo2_valid;   //������ʾSP02�����Ƿ���Ч��ָʾ��
int32_t n_heart_rate;   //����ֵ
int8_t  ch_hr_valid;    //������ʾ���ʼ����Ƿ���Ч��ָʾ��

uint8_t Temp;

uint32_t un_min, un_max, un_prev_data;  
int i;
int32_t n_brightness;
float f_temp;
//u8 temp_num=0;
u8 temp[6];
u8 str[100];
u8 dis_hr=0,dis_spo2=0;

int main(void)
{
	delay_init(72);	  
	LED_Init();		  				//��ʼ��������豸���ӵ�Ӳ���ӿ�
	OLED_Init();					//OLED��ʼ��
	delay_ms(50);
	OLED_Clear();						//����

	MAX30102_Init();
	USART1_Config();//���ڳ�ʼ��
	
	un_min=0x3FFFF;
	un_max=0;
	
	//��ʾ�����ʣ���
	OLED_ShowChinese(0,0,0,16,1);
	OLED_ShowChinese(16,0,1,16,1);
	OLED_ShowChar(40,0,':',16,1);
	OLED_ShowString(80,0,"BMP",16,1);
	
	//��ʾ��Ѫ������
	OLED_ShowChinese(0,16,2,16,1);
	OLED_ShowChinese(16,16,3,16,1);
	OLED_ShowChar(40,16,':',16,1);
	OLED_ShowChar(80,16,'%',16,1);

	n_ir_buffer_length=500; //����������Ϊ100���ɴ洢��100sps���е�5������
	//��ȡǰ500����������ȷ���źŷ�Χ
	for(i=0;i<n_ir_buffer_length;i++)
	{
			while(MAX30102_INT==1);   //�ȴ���ֱ���ж����Ŷ���
			
			max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);
			aun_red_buffer[i] =  (long)((long)((long)temp[0]&0x03)<<16) | (long)temp[1]<<8 | (long)temp[2];    // ��ֵ�ϲ��õ�ʵ������
			aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03)<<16) |(long)temp[4]<<8 | (long)temp[5];   	 // ��ֵ�ϲ��õ�ʵ������
					
			if(un_min>aun_red_buffer[i])
					un_min=aun_red_buffer[i];    //���¼�����Сֵ
			if(un_max<aun_red_buffer[i])
					un_max=aun_red_buffer[i];    //���¼������ֵ
	}
	un_prev_data=aun_red_buffer[i];
	
	//����ǰ500��������ǰ5���������������ʺ�Ѫ�����Ͷ�
	maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid); 
	

	while(1)
	{
		//��ȥǰ100��������������400�������Ƶ���������100~500����������λ��0~400
			for(i=100;i<500;i++)
			{
					aun_red_buffer[i-100]=aun_red_buffer[i];	//��100-500����������λ��0-400
					aun_ir_buffer[i-100]=aun_ir_buffer[i];		//��100-500����������λ��0-400
					
					//update the signal min and max
					if(un_min>aun_red_buffer[i])			//Ѱ����λ��0-400�е���Сֵ
					un_min=aun_red_buffer[i];
					if(un_max<aun_red_buffer[i])			//Ѱ����λ��0-400�е����ֵ
					un_max=aun_red_buffer[i];
			}
			
			//�ڼ�������ǰȡ100��������ȡ�����ݷ���400-500����������
			for(i=400;i<500;i++)
			{
					un_prev_data=aun_red_buffer[i-1];	//�ڼ�������ǰȡ100��������ȡ�����ݷ���400-500����������
					while(MAX30102_INT==1);
					max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);		//��ȡ���������ݣ���ֵ��temp��
					aun_red_buffer[i] =  (long)((long)((long)temp[0]&0x03)<<16) | (long)temp[1]<<8 | (long)temp[2];    //��ֵ�ϲ��õ�ʵ�����֣�����400-500Ϊ�¶�ȡ����
					aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03)<<16) |(long)temp[4]<<8 | (long)temp[5];   	//��ֵ�ϲ��õ�ʵ�����֣�����400-500Ϊ�¶�ȡ����
					if(aun_red_buffer[i]>un_prev_data)		//���»�ȡ��һ����ֵ����һ����ֵ�Ա�
					{
							f_temp=aun_red_buffer[i]-un_prev_data;
							f_temp/=(un_max-un_min);
							f_temp*=MAX_BRIGHTNESS;			//��ʽ���������ߣ�=������ֵ-����ֵ��/�����ֵ-��Сֵ��*255
							n_brightness-=(int)f_temp;
							if(n_brightness<0)
									n_brightness=0;
					}
					else
					{
							f_temp=un_prev_data-aun_red_buffer[i];
							f_temp/=(un_max-un_min);
							f_temp*=MAX_BRIGHTNESS;			//��ʽ���������ߣ�=������ֵ-����ֵ��/�����ֵ-��Сֵ��*255
							n_brightness+=(int)f_temp;
							if(n_brightness>MAX_BRIGHTNESS)
									n_brightness=MAX_BRIGHTNESS;
					}
			//ͨ��UART�������ͼ��������͵��ն˳���
			if(ch_hr_valid == 1 && n_heart_rate<120)//**/ ch_hr_valid == 1 && ch_spo2_valid ==1 && n_heart_rate<120 && n_sp02<101
			{
				dis_hr = n_heart_rate;
				dis_spo2 = n_sp02;
			}
			else
			{
				dis_hr = 0;
				dis_spo2 = 0;
			}
		}
		maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
		
		//MAX30102
		if(dis_hr==0)
			OLED_ShowNum(47,0,0,3,16,1);
		else
		OLED_ShowNum(47,0,dis_hr-20,3,16,1);
		
		OLED_ShowNum(47,16,dis_spo2,2,16,1);
		printf("����= %d BPM Ѫ��= %d\r\n ",dis_hr,dis_spo2);
		
		 //Serial_SendString("\r\n");

	}
}

