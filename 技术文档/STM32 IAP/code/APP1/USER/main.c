#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"


/************************************************
 ALIENTEKս��STM32������ʵ��4
 ����ʵ��
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/


int main(void)
{
    SCB->VTOR = FLASH_BASE | 0x4000;
    
    delay_init();	    	 //��ʱ������ʼ��
    uart_init(115200);	 //���ڳ�ʼ��Ϊ115200

    while(1)
    {
        printf("����ִ��APP1����,��ʱ1��\r\n\r\n");
        delay_ms(1000);
    }
}

