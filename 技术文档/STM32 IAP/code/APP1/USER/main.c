#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"


/************************************************
 ALIENTEK战舰STM32开发板实验4
 串口实验
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司
 作者：正点原子 @ALIENTEK
************************************************/


int main(void)
{
    SCB->VTOR = FLASH_BASE | 0x4000;
    
    delay_init();	    	 //延时函数初始化
    uart_init(115200);	 //串口初始化为115200

    while(1)
    {
        printf("正在执行APP1程序,延时1秒\r\n\r\n");
        delay_ms(1000);
    }
}

