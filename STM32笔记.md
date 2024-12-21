# STM32GPIO模式

## 输入模式

- 上拉输入(GPIO_Mode_IPU)
- 下拉输入(GPIO_Mode_IPD)
- 浮空输入(GPIO_Mode_IN_FLOATING)
- 模拟输入(GPIO_Mode_AIN)

## 输出模式

- 推挽输出(GPIO_Mode_Out_PP)
- 开漏输出(GPIO_Mode_Out_OD)
- 复用推挽输出(GPIO_Mode_AF_PP)
- 复用开漏输出(GPIO_Mode_AF_OD)

1. 上拉输入(GPIO_Mode_IPU)

   上拉就是把点位的电平拉高，比如拉到Vcc。上拉就是将不确定的信号通过一个电阻嵌位在高电平。电阻同时起到限流的作用。弱强只是上拉电阻的阻值不同，没有什么严格区分。![image-20240527223619995](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240527223619995.png)

2. 下拉输入(GPIO_Mode_IPD)、

   就是把电压拉低，拉到GND。与上拉原理相似。![image-20240527223940946](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240527223940946.png)

3. 浮空输入(GPIO_Mode_IN_FLOATING)

   浮空就是逻辑器件与引脚即不接高电平，也不接低电平。由于逻辑器件的内部结构，当它输入引脚悬空时，相当于该引脚接了高电平。一般实际运用时，引脚不建议悬空，易受干扰。通俗讲就是浮空就是浮在空中，就相当于此端口在默认情况下什么都不接，呈高阻态，这种设置在数据传输时用的比较多。浮空最大的特点就是电压的不确定性，它可能是0V，页可能是VCC，还可能是介于两者之间的某个值。浮空输入一般多用于外部按键、ADC输入用，这样可以减少上下拉电阻对结果的影响。![image-20240527224035080](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240527224035080.png)

4. 模拟输入(GPIO_Mode_AIN)

   模拟输入是指传统方式的输入，数字输入是输入PCM数字信号，即0、1的二进制数字信号，通过数模转换，转换成模拟信号，经前级放大进入功率放大器，功率放大器还是模拟的。![image-20240527224124819](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240527224124819.png)

5. 推挽输出(GPIO_Mode_Out_PP)

   可以输出高、低电平，连接数字器件；推挽结构一般是指两个三级管分别受到互补信号的控制，总是在一个三极管导通的时候另一个截止。高低电平由IC的电源低定。

   推挽电路是两个参数相同的三极管或MOSFET，以推挽方式存在于电路中，各负责正负半周的波形方法任务，电路工作时，两只对称的功率开关管每次只有一个导通，所以导通损耗小，效率高。输出即可以向负载灌电流。推拉式输出级即提高电路的负载能力，又提高开关速度。![image-20240527224223139](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240527224223139.png)

6. 复用推挽输出(GPIO_Mode_AF_PP)

   可以理解为GPIO口被用作第二功能时的配置情况（并非作为通用IO口使用）。![image-20240527224346711](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240527224346711.png)

7. 开漏输出(GPIO_Mode_Out_OD)

   输出端相当于三极管的集电极，要得到高电平状态需要上拉电阻才行，适合于做电流型的驱动，其吸收电流的能力相对强（一般20mA以内）。开漏形式的电路有以下几个特点：

   1. 利用外部电路的驱动能力，减少IC内部的驱动。当IC内部MOSFET导通时，驱动电流是从外部的VCC流经R pull-up ，MOSFET到GND，IC内部仅需很小的栅极驱动电流。
   2. 一般来说，开漏是用来连接不同电平的器件，匹配电平用的，因为开漏引脚不连接外部的上拉电阻时，只能输出低电平，如果需要同时具备输出高电平的功能，则需要接上拉电阻，**很好的一个优点是通过改变上拉电源的电压，便可以改变传输电平**。比如加上上拉电阻就可以提供TTL/CMOS电平输出等。(上拉电阻的阻值决定了逻辑电平转换的沿的速度 ，阻值越大，速度越低功耗越小，所以**负载电阻的选择要兼顾功耗和速度**。)
   3. OPEN-DRAIN提供了灵活的输出方式，但是也有其弱点，就是带来上升沿的延时。因为上升沿是通过外接上拉无源电阻对负载充电，所以当电阻选择小时延时就小，但功耗大;反之延时大功耗小。所以如果对延时有要求，则建议用下降沿输出。
   4. 可以将多个开漏输出的Pin连接到一条线上，通过一只上拉电阻，在不增加任何器件的情况下，形成“与逻辑”关系。这也是I2C，SMBus等总线判断总线占用状态的原理。可以简单的理解为：在所有引脚连在一起时，外接一上拉电阻，如果有一个引脚输出为逻辑0，相当于接地，与之并联的回路“相当于被一根导线短路”，所以外电路逻辑电平便为0，只有都为高电平时，与的结果才为逻辑1。

   在一个结点(线)上，连接一个上拉电阻到电源VCC或VDD和n个NPN或NMOS晶体管的集电极C或漏极D，这些晶体管的发射极E或源极S都接到地线上，只要有一个晶体管饱和，这个结点(线)就被拉到地线电平上。因为这些晶体管的基极注入电流(NPN)或栅极加上高电平(NMOS)，晶体管就会饱和，所以这些基极或栅极对这个结点(线)的关系是或非NOR逻辑。如果这个结点后面加一个反相器，就是或OR逻辑。![image-20240527224527847](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240527224527847.png)

8. 复用开漏输出(GPIO_Mode_AF_OD)

   可以理解为GPIO口被用作第二功能时的配置情况（即并非作为通用IO口使用）。端口必须配置成复用功能输出模式（推挽或开漏）。![image-20240527224606787](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240527224606787.png)

# STM32EXTI的配置步骤（GPIO外部中断）

1. 使能GPIO时钟
2. 设置GPIO输入模式（上/下/浮空）
3. 使能AFIO/SYSCFG时钟（设置AFIO/SYSCFG时钟开启寄存器）
4. 设置EXTI和IO对应关系(选择PA~PK到底那组IO对应EXTI输入线，AFIO_EXTICR/SYSCFG_EXTICR)
5. 设置EXTI屏蔽（设置EXTI对应通道的屏蔽和上升沿/下降沿触发，IMR,RTSR）
6. 设置NVIC（设置优先级分组，设置优先级，使能中断）
7. 设置中断服务函数（编写对应中断的中断服务函数，清除中断标志）

# STM32EXTI的HAL库设置步骤

1. 使能GPIO时钟（使用：__HAL_RCC_GPIOx_CLK_ENABLE）

2. GPIO/AFIO(SYSCFG)/EXTI(使用:HAL_GPIO_Init)

3. 设置中断分组（使用:HAL_NVIC_SetPriorityGrouping）此函数只需设置一次

4. 设置中断优先级（使用：HAL_NVIC_SetPriority）

5. 使能中断（使用：HAL_NVIC_EnableIRQ）

6. 设计中断服务函数（编写：EXTIx_IRQHandler，中断服务函数，清除中断标志）

   STM32仅有：EXTI0~4,EXTI9~5,EXTI15~10,7个外部服务中断函数

# USART/UART(串口通信)

USART 通用同步/异步收发器

UART 通用异步收发器

1. 比特率：每秒传送的比特数，单位bit/s

2. 波特率：每秒传送的码元数，单位Baud

3. 比特率 = 波特率*log2M,M表示每个码元承载的信息量

4. RS-232异步通信示意图

   ![image-20240530091134562](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240530091134562.png)

5. STM32和USB通信

   ![image-20240530092323012](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240530092323012.png)
   
   ![image-20240606090036847](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240606090036847.png)

## 串口轮询模式：(HAL库)

```c
char message[] = "Hello World!\r\n";
char receive_message[10];
/* USER CODE END 2 */

/* Infinite loop */
/* USER CODE BEGIN WHILE */
  while (1) {
      HAL_UART_Receive(&huart1,(uint8_t *)receive_message,10,HAL_MAX_DELAY);
      HAL_UART_Transmit(&huart1,(uint8_t *)receive_message,10,HAL_MAX_DELAY);
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  }
```



## 串口中断模式：(HAL库)

```c
//串口回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart == &huart1)
    {
        HAL_UART_Transmit_IT(&huart1,(uint8_t *)message, strlen(message));
    }
    HAL_UART_Receive_IT(&huart1,(uint8_t *)receive_message,sizeof (receive_message));

}
```



## 串口配合DMA方式：(HAL库)

![image-20241211150738768](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241211150738768.png)

```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart == &huart1)
    {
        HAL_UART_Transmit_DMA(&huart1,(uint8_t *)message, strlen(message));
    }
    HAL_UART_Receive_DMA(&huart1,(uint8_t *)receive_message,sizeof (receive_message));

}
```

## 串口接收不定长的数据：

使用串口的空闲中断。

```c
//串口空闲中断
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(huart == &huart1)
    {
        HAL_UART_Transmit_DMA(&huart1,(uint8_t *)receive_message,Size);

        HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t *)receive_message,sizeof (receive_message));
        __HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT); // 关闭DMA过半中断
    }
}
```

# 配置STM32外部中断流程（标准库：按键控制LED）

1. 开启RCC时钟，把涉及到的时钟都打开

   ```c
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//开启GIOP时钟
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//开启AFIO时钟
   ```

2. 配置GPIO,选择端口为输入模式（以PB1为例）

   ```C
   GPIO_InitTypeDef GPIO_InitStruct;//定义GPIO结构体
   GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;//设置为上拉输入,根据具体的按键电路图来选择
   GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;//选择GPIO引脚
   GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOB,&GPIO_InitStruct);
   ```

3. 配置AFIO(复用功能IO),选择我们用的GPIO连接到后面的EXTI

   ```c
   //配置AFIO
   GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);//因为我们配置的GPIO引脚为PB1和PB11,所以选择GPIO_PinSource1
   ```

4. 配置EXTI(外部中断/事件控制器)，选择触发方式(上升沿,下降沿,双边沿),触发响应方式(中断或者事件触发)

   ```c
   EXTI_InitTypeDef EXTI_Struect;
   EXTI_Struect.EXTI_Line = EXTI_Line1;//选择EXTI线,对应GPIO线
   EXTI_Struect.EXTI_LineCmd = ENABLE;//开启中断
   EXTI_Struect.EXTI_Mode = EXTI_Mode_Interrupt;//选择中断模式
   EXTI_Struect.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
   EXTI_Init(&EXTI_Struect);
   ```

5. 配置NVIC(嵌套向量中断控制器),给外设配置合适的优先级，最后通过NVIC中断信号进入CPU

   ```C
   //配置NVIC
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先分组
   NVIC_InitTypeDef NVIC_Struct;
   NVIC_Struct.NVIC_IRQChannel = EXTI1_IRQn;//参数整个工程搜索在stm32f10x.h中，选择md的芯片中找
   NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 2;//设置抢占优先级
   NVIC_Struct.NVIC_IRQChannelSubPriority = 2;//设置响应优先级
   NVIC_Init(&NVIC_Struct);
   ```

6. 写中断函数

   ```C
   void EXTI1_IRQHandler(void)
   {
   	//先判断EXTI1的中断标志位
   	if(EXTI_GetITStatus(EXTI_Line1) == SET)
   	{
   		LED1_Turn();//LED翻转
   	}
   	EXTI_ClearITPendingBit(EXTI_Line1);//清除中断标志位,以便进行下一次中断
   }
   ```

7. GPIO外部中断简图 GPIO->AFIO(F1)/SYSCFG(F4,F7,H7)->EXTI->NVIC->CPU

8. 中断：要进入NVIC,有相应的中断服务函数，需要CPU处理

   事件：不进入NVIC,仅用于内部硬件自动控制，如TIM,DMA,ADC

9. AFIO(F1):主要用于重映射和外部中断映射配置

   作用：

   - 调试IO配置
   - 重映射配置
   - 外部中断配置 AFIO_EXTICR1~4,配置EXTI中断线0~~15对应的具体IO口

   使能AFIO时钟： 

   ```c
   _HAL_RCC_AFIO_CLK_ENABLE();//HAL库
   ```

   

10. SYSCFG简介(F4/F7/H7)

    系统配置控制器，用于外部中断映射配置等。

    外部中断配置 SYSCFG_EXTICR1~4，配置EXTI中断线0~15对应具体那个IO口

    使能SYSCFG函数：

    ```C
    _HAL_RCC_AFIO_CLK_ENABLE();//HAL库
    ```

    

# 定时器

## 定时器输出比较

![image-20240702160931230](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240702160931230.png)

## STM32定时器的计算公式

![img](https://i-blog.csdnimg.cn/blog_migrate/4bbf3448dfe2956abbc8f8f95875f14b.jpeg)

公式解释：

ARR（TIM_Period）：自动重装载值，是定时器溢出前的计数值

PSC（TIM_Prescaler）：预分频值，是用来降低定时器时钟频率的参数

Tclk：定时器的输入时钟频率（单位Mhz），通常为系统时钟频率或者定时器外部时钟频率

Tout：定时器溢出时间（单位us）。一定要注意这个单位是us。



## 基本介绍

STM32总共有8个定时器，分别是2个高级定时器（TIM1、TIM8），4个通用定时器（TIM2、TIM3、TIM4、TIM5）和2个基本定时器（TIM5、TIM6），根据具体的芯片看数据手册。

三种定时器的的区别如下： 即高级定时器具有捕获/比较通道和互补输出，通用定时器只有捕获/比较通道，基本定时器没有以上两者。

![img](https://i-blog.csdnimg.cn/blog_migrate/da5b3fa2ccd9477ae9ae2020e48bac4d.png)

### 通用定时器的特点：

STM32的众多定时器中我们使用最多的是高级定时器和通用定时器，而高级定时器一般也是用作通用定时器的功能，下面我们就以通用定时器为例进行讲解，其功能和特点包括：

- 位于低速的APB1总线上(APB1)
- 16 位向上、向下、向上/向下(中心对齐)计数模式，自动装载计数器（TIMx_CNT）。
- 16 位可编程(可以实时修改)预分频器(TIMx_PSC)，计数器时钟频率的分频系数 为 1～65535 之间的任意数值。
- 4 个独立通道（TIMx_CH1~4），这些通道可以用来作为：
  - 输入捕获
  - 输出比较
  - PWM生成（边缘或中间对齐模式）
  - 单脉冲输出模式
- 可使用外部信号（TIMx_ETR）控制定时器和定时器互连（可以用 1 个定时器控制另外一个定时器）的同步电路。
- 如下事件发生时产生中断/DMA（6个独立的IRQ/DMA请求生成器）：
  - 更新：[计数器](https://so.csdn.net/so/search?q=计数器&spm=1001.2101.3001.7020)向上溢出/向下溢出，计数器初始化(通过[软件](https://marketing.csdn.net/p/3127db09a98e0723b83b2914d9256174?pId=2782&utm_source=glcblog&spm=1001.2101.3001.7020)或者内部/外部触发) 
  - 触发事件(计数器启动、停止、初始化或者由内部/外部触发计数) 
  - 输入捕获
  - 输出比较 
  - 支持针对定位的增量(正交)编码器和霍尔传感器电路 
  - 触发输入作为外部时钟或者按周期的电流管理
- STM32 的通用定时器可以被用于：测量输入信号的脉冲长度(输入捕获)或者产生输出波形(输出比较和 PWM)等。
- 使用定时器预分频器和 RCC 时钟控制器预分频器，脉冲长度和波形周期可以在几个微秒到几个毫秒间调整。STM32 的每个通用定时器都是完全独立的，没有互相共享的任何资源。

### 计数模式：

通用定时器可以向上计数、向下计数、向上向下双向计数模式。

1. **向上计数模式**：计数器从0计数到自动加载值(TIMx_ARR)，然后重新从0开始计数并且产生一个计数器溢出事件。
2. **向下计数模式**：计数器从自动装入的值(TIMx_ARR)开始向下计数到0，然后从自动装入的值重新开始，并产生一个计数器向下溢出事件。
3. **中央对齐模式（向上/向下计数）**：计数器从0开始计数到自动装入的值-1，产生一个计数器溢出事件，然后向下计数到1并且产生一个计数器溢出事件；然后再从0开始重新计数。

![](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240702153611644.png)

### 定时器工作原理：

#### 1.定时器基本框图：

下图为定时器基本框图：

![img](https://i-blog.csdnimg.cn/blog_migrate/960f0c62fc8156b5cad8d14bb5c18a59.png)

款图可以分为四大部分（用红色笔标出）分别是：①时钟产生器部分，②时基单元部分，③输入捕获部分、④输出比较部分。



#### 2.时钟产生器部分：

在第一部分的时钟选择上，STM32定时器有4种时钟源选择（图种篮笔标识），分别是：

- 内部时钟（CK_INT）
- 外部时钟模式：外部触发输入（ETR）
- 内部触发输入（ITRx）：使用一个定时器作为另一个定时器的预分频器，如可以配置一个定时器Timer1而作为另一个定时器Timer2的预分频器
- 外部时钟模式：外部输入脚（TIX）

这四种情况可由下图来表示：

![img](https://i-blog.csdnimg.cn/blog_migrate/ba69c99756975cbc795e9449f99c263d.png)

其中，内部触发输入口1~4除了ITR1/ITR2/ITR3/ITR4之外还有一种情况：用一个定时器作为另一个定时器的分频器。

外部捕获比较引脚有两种，分别是：

引脚1：TI1FP1或TI1F_ED

引脚2：TI2FP2



#### 3.时基单元

![image-20240702152403497](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240702152403497.png)

![image-20240702152141184](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240702152141184.png)

![image-20240702152210185](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240702152210185.png)

时基单元如上图所示：它包括三个寄存器：计数器寄存器（TIMx_CNT)、预分频器寄存器（TIMx_PSC)和自动装载寄存器（TIMx_ARR)。对这三个寄存器的介绍如下：

- 计数器寄存器（TIMx_CNT）：向上计数、向下计数或中心对齐计数。
- 预分频器寄存器（TIMx_PSC)：可将时钟频率按1到65535之间的任意值进行分频，可在运行时改变其设置值。
- 自动装载寄存器（TIMx_ARR)：
  - 如果TIMx_CR1寄存器中的ARPE位为0，ARR寄存器的内容将直接写入影子寄存器；
  - 如果ARPE为1，ARR寄存器将在每次的更新时间UEV发生时，传送到影子寄存器。
  - 如果TIMx_CR1中的UDIS位为0，当计数器溢出，产生更新时间。



#### 4.输入捕获通道

- IC1、2和IC3、4可以分别通过软件设置将其映射到TI1、TI2和TI3、TI4;
- 4个16位捕捉比较寄存器可以编程用于存放检测到对应的每一次输入捕捉时计数器的值。
- 当产生一次捕捉，相应的的CCxIF标志位被置1；同时如果中断或DMA请求使能，则产生中断或DMA请求。
- 如果单CCxIF标志位已经为1，当又产生一个捕捉，则捕捉溢出标志位CCxOF将被置1。

![img](https://i-blog.csdnimg.cn/blog_migrate/7e9b77d5f3d474ac342a114a7eafb64c.png)

#### 5.输出比较通道（PWM）

- PWM模式运行产生：

  - 频率和占空比可以进行如下设定：

    - 一个自动重装寄存器用于设定PWM的周期

    - 每个PWM通道有一个捕捉比较寄存器用于设定占空时间

      例如：产生一个40KHZ的PWM信号：在定时器2的时钟为72MHZ下，占空比为50%：

      预分频器设置为0（计数器的时钟为TIM1CLK(O+1)）,自动重装寄存器设置为1799，CCRx寄存器设置为899.

- 两种可设置PWM模式：

  - 边沿对齐模式

  - 中心对齐模式

    ![img](https://i-blog.csdnimg.cn/blog_migrate/9b093f31edaaeec5cb7875d2db4cd048.png)

    

##### 	PWM计算公式：

`ARR（TIM_Period）` 是计数值；

`PSC（TIM_Prescaler）` 是预[分频](https://so.csdn.net/so/search?q=分频&spm=1001.2101.3001.7020)值。

TIMx->CCR:输出比较寄存器。

**周期：Fpwm = 主频 / （（ARR+1）*（PSC+1））； 单位（hz）**

**占空比计算公式：duty circle = TIMx->CCR / ARR**



例如：STM32F103C8T6           主频 72MHZ          ARR = 100 -1   PSC = 72-1   TIMx->CCR = 20

Fpwm = 72M / (100 * 72) = 10Khz  所以PWM的周期是： 1/ 10000 = 100us     占空比为：20 / 100 = 20%



### 定时器中断相关寄存器：

计数器当前值寄存器CNT

![img](https://i-blog.csdnimg.cn/blog_migrate/12f28d1aa09124fd1b12d6967f8dc00d.png)

预分频寄存器TIMx_PSC

![img](https://i-blog.csdnimg.cn/blog_migrate/b2963996f425f37deda328068fcd1332.png)

自动重装寄存器TIMx_ARR

![img](https://i-blog.csdnimg.cn/blog_migrate/54109e51db9bd0b0975fc8bafcb9d356.png)

控制寄存器TIMx_CR1

![img](https://i-blog.csdnimg.cn/blog_migrate/54109e51db9bd0b0975fc8bafcb9d356.png)

DMA中断使能寄存器：

![img](https://i-blog.csdnimg.cn/blog_migrate/d6f6ca83188c0a2e6eddd9a36a2b9606.png)

TIM1延时1S中断：

![image-20241212212249563](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241212212249563.png)

```c
HAL_TIM_Base_Start_IT(&htim1); //中断定时器启动函数

//定时器中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim == &htim1)
  {
    HAL_UART_Transmit_IT(&huart1,(uint8_t*)message, strlen(message));
  }
}
```

**PWM模式在FreeRTOS中。**

# 独立看门狗

![image-20241023211322099](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241023211322099.png)

 看门狗这东西虽然简单，但我相信绝大多程序员没有足够重视它。使用看门狗保证系统正常地运行是非常有必要的。我们在设计产品时，代码以及硬件设计缺陷或是外界电磁干扰都有可能使系统死机，如果不能正常对其进行复位，系统的可靠性将大打折扣。看门狗分为软件看门狗和硬件看门狗两类，其原理都是使用一个独立定时器来计时，超出时间就会产生复位信号，主要区别看是否具有独立的硬件结构，如果有，就是硬件看门狗，如果是一个普通定时器实现的那么就是软件看门狗。STM32片内有两个看门狗：独立看门狗IWDG以及窗口看门狗WWDG。

## 两个狗的区别

- 独立看门狗没有[中断](https://so.csdn.net/so/search?q=中断&spm=1001.2101.3001.7020)，窗口看门狗有中断
- 独立看门狗有硬件软件之分，窗口看门狗只能软件控制
- 独立看门狗只有下限，窗口看门狗有下限和上限
- 独立看门狗是12位递减的。窗口看门狗是7位递减的
- 独立看门狗是用的内部的大约40KHZ RC振荡器，窗口看门狗是用的系统时钟APB1ENR



# ADC

## ADC转换（单通道）光敏电阻传感器(标准库)

```c
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);//选择输入通道ADC_SampleTime_55Cycles5采样周期
	
	//配置ADC结构
	ADC_InitTypeDef ADC_InitStruct;
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE; //单次转换模式
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//数据向右对齐
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件触发启动
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//独立模式
	ADC_InitStruct.ADC_NbrOfChannel = 1;//顺序进行规则转换的ADC通道的数目
	ADC_InitStruct.ADC_ScanConvMode =DISABLE;//关闭扫描模式
	
	ADC_Init(ADC1,&ADC_InitStruct);
	
	//开启ADC
	ADC_Cmd(ADC1,ENABLE);
	
	//进行ADC校准
	ADC_ResetCalibration(ADC1);//使能复位校准
	while(ADC_GetResetCalibrationStatus(ADC1) == SET);//等待复位校准结束
	ADC_StartCalibration(ADC1);//开启AD校准
	while(ADC_GetCalibrationStatus(ADC1) == SET);//等待ADC校准结束
```



Analog-to-Digital Converter的缩写。指模/数转换器或者模拟/数字转换器。是指将连续变量的模拟信号转换为离散的数字信号的器件。

典型的模拟数字转换器将模拟信号转换为表示一定比例电压值的数字信号。

简单地说就是将模拟电压值，转换成对应的肉眼可读数值

12位ADC是一种逐次逼近型模拟数字转换器。它有，3个ADC控制器，多达18个通道，可测量16个外部和2个内部信号源。各通道的A/D转换可以单次、连续、扫描或间断模式执行。ADC的结果可以左对齐或右对齐方式存储在16位数据寄存器中。

12位模拟数字转换器
就是ADC的数字存储是12位的 也就是说转换器通过采集转换所得到的最大值是4095 “111111111111”=4095 二进制的12位可表示0-4095个数， 对应着所测电压的实际值，转换的电压范围是0v-3.3v的话，转换器就会把0v-3.3v平均分成4096份。设转换器所得到的值为x，所求电压值为y。

![在这里插入图片描述](https://i-blog.csdnimg.cn/blog_migrate/5318048e6e66ff031682bde32a92f72f.png)

同理，可以理解8位精度和10位精度

## ADC的采样步骤：

1. **采样保持：**在一定时间内，对模拟信号进行采样并保持其电压值不变。
2. **量化：**将采样保持的模拟信号电压值转换成数字量，通常使用比特位数表示。
3. **编码：**将量化后的数字量编码成二进制的形式。
4. **存储：**将编码后的数字信号存储在计算机或其它数字处理设备中，以便进行进一步的数字信号处理。

ADC采样的精度和速度很大程度上取决于采样率和比特数的选择。较高的采样率和比特数可以提供更精确的数字信号，但也会增加系统的成本和复杂性。

## ADC的特性：

1. **分辨率：**ADC的分辨率是指它能够将输入信号分成多少个离散的量化级别。通常用比特数来表示，例如8位ADC的分变率位2^8=256个量化级别。
2. **采样率：**ADC的采样率是指它能够对输入信号进行采样的频率。采样率越高，可以捕获到更高频率的信号，但也需要更高的处理能力和更大的存储空间。
3. **精度：**ADC的精度是指它能够将输入信号转换为数字信号的准确程度。精度通常用百分比误差或最小有效位（LSB）来表示。
4. **噪声：**ADC的噪声是指在信号转换的过程中引入的随机误差。噪声越小，ADC的精度和可靠性就高。
5. **功耗：**ADC的功耗是指它在工作过程中消耗的电力。功耗越低，可以延长电池寿命或降低系统成本。



## 在多通道采集时需要注意以下几点：

1. 通道选择：需要指定采样的通道数以及每个通道的物理引脚。
2. 采样顺序：需要指定采样的顺序，可以是顺序采样或者随机采样。
3. 转换时间：需要根据ADC的转换时间和采样频率来确定每个通道的采样时间，以确保转换完成后能够在下一个转换周期开始前完成数据读取。
4. 数据格式：需要确定ADC输出数据的格式，包括数据为，精度和符号位等。



## 逐次逼近形ADC

采用逐次逼近法的AD转换器是有一个比较器，DA转换器、缓冲寄存器和逻辑控制电路组成。如下图所示：

![img](https://i-blog.csdnimg.cn/blog_migrate/2f999303cdca59a1e62a38ef0403c28b.png)

  基本原理是：从高位到低位逐次试探比较，就像用天平秤物体，从重到轻逐级增减砝码进行试探。逐次逼近法的转换过程是：初始化时将逐次逼近寄存器各位清零，转换开始时，先将逐次逼近寄存器最高位置1，送入DA转换器，经DA转换后生成的模拟量送入比较器，称为U0，与送入比较器的待转换的模拟量Ux进行比较，若U0<Ux，该位1被保留，否则被清除。然后再将逐次逼近寄存器次高位置1，将寄存器中新的数字量送DA转换器，输出的U0再与Ux比较，若U0<Ux，该位1被保留，否则被清除。重复此过程，直至逼近寄存器最低位。转换结束后，将逐次逼近寄存器中的数字量送入缓冲寄存器，得到数字量的输出。逐次逼近的操作过程是在一个控制电路的控制下进行的。



## STM32ADC功能图

![在这里插入图片描述](https://i-blog.csdnimg.cn/blog_migrate/483413e7b3a2af65ec240c72a8172a62.png)

### 多通道采样模式----轮询

1. 连续转换模式：扫描模式（手动转换）。在扫描模式下启动列表之后，里面每一个单独的通道转换完成后不会产生任何标志位、和中断。它只有在整个列表都转换完成之后才会产生一次EOC标志位，才能触发中断。这种方式容易造成数据覆盖的问题如果想要用扫描模式实现多通道的采集，最好配合DMA实现。
2. 单次转换模式。

## 多通道采集模式------DMA





## 数据的左右对齐模式：

因为ADC得到的数据是12位精度的，但是数据存储在 16 位数据寄存器中，所以ADC的存储结果可以分为**左对齐或右对齐**方式（12位）

![在这里插入图片描述](https://i-blog.csdnimg.cn/blog_migrate/7e22a1ff4399f2e297f3f2805d181b51.png)

## STM32ADC电压输入范围：

![在这里插入图片描述](https://i-blog.csdnimg.cn/blog_migrate/8909454d19dc3e5e538de732f04340c5.png)

**ADC一般用于采集小电压，其输入值不能超过VDDA**，即ADC输入范围：VREF- ≤ VIN ≤ VREF+。具体的定义见上图。
**一般把VSSA和VREF- 接地， VREF+ 和 VDDA接3V3，那么ADC的输入范围是0~3.3V。**



## ADC输入通道：

从**ADCx_INT0-ADCx_INT15** 对应三个ADC的16个**外部通道**，进行**模拟信号转换** 此外，还有两个内部通道：**温度检测**或者**内部电压检测**
选择对应通道之后，便会选择**对应GPIO引脚**，相关的引脚定义和描述可在开发板的数据手册里找。

## 注入通道，规则通道

我们看到，在选择了ADC的相关通道引脚之后，在模拟至数字转换器中有两个通道，注入通道，规则通道，
规则通道至多16个，注入通道至多4个

### 规则通道：

规则通道相当于你正常运行的程序，看它的名字就可以知道，很规矩，就是正常执行程序。

### 注入通道：

注入通道可以打断规则通道，听它的名字就知道不安分，如果在规则通道转换过程中，有注入通道进行转换，那么就
要先转换完注入通道，等注入通道转换完成后，再回到规则通道的转换流程。

![在这里插入图片描述](https://i-blog.csdnimg.cn/blog_migrate/c3dc328c5ceb5168a0f4b88bf3904b80.png)

## ADC时钟:

图中的ADC预分频器的ADCCLK是ADC模块的时钟来源。通常，由时钟控制器提供的ADCCLK时钟和PCLK2（APB2时钟）同步。RCC控制器为ADC时钟提供一个专用的可编程预分频器。 分频因子由RCC_CFGR的ADCPRE[1:0]配置，可配置2/4/6/8分频

STM32的ADC最大的转换速率为1MHz,也就是说最快转换时间为1us，为了保证ADC转换结果的准确性，ADC的时钟最好不超过14M。

> T = 采样时间 + 12.5个周期，其中1周期为1/ADCCLK

例如，当 ADCCLK=14Mhz 的时候，并设置 1.5 个周期的采样时间，则得到： Tcovn=1.5+12.5=14 个周期=1us。

## ADC外部触发转换：

ADC 转换可以由ADC 控制寄存器2: ADC_CR2 的ADON 这个位来控制，写1 的时候开始转换，写0 的时候停止转换

当然，除了ADC_CR2寄存器的ADON位控制转换的开始与停止，还可以支持外部事件触发转换（比如定时器捕捉、EXTI线）

包括内部定时器触发和外部IO触发。具体的触发源由ADC_CR2的EXTSEL[2:0]位（规则通道触发源 ）和 JEXTSEL[2:0]位（注入通道触发源）控制。

同时ADC3的触发源与ADC1/2的触发源有所不同，上图已经给出。

## 中断：

中断触发条件有三个，**规则通道转换结束**，**注入通道转换结束**，或者**模拟看门狗状态位被设置**时都能产生中断，

![在这里插入图片描述](https://i-blog.csdnimg.cn/blog_migrate/8388aee597f836cf404b4d0ce8da75f4.png)

### 模拟看门狗中断

当被ADC转换的模拟电压值低于低阈值或高于高阈值时，便会产生中断。阈值的高低值由ADC_LTR和ADC_HTR配置
模拟看门狗，听他的名字就知道，在ADC的应用中是为了防止读取到的电压值超量程或者低于量程

### DMA

同时ADC还支持DMA触发，规则和注入通道转换结束后会产生DMA请求，用于将转换好的数据传输到内存。

注意，只有ADC1和ADC3可以产生DMA请求

因为涉及到DMA传输，所以这里我们不再详细介绍，之后几节会更新DMA,一般我们在使用ADC 的时候都会开启DMA 传输。

## F1和F4ADC的区别：

F4的ADC支持12位，10位，8位和6位精度，F1只支持12位

F1和F4都具有3个ADC，F1可提供21个输入通道，F4可以提供24个输入通道。

F1的ADC最大采样频率为1Msps，2路交替采样可到2Msps（F1不支持3路交替采样）。F4的ADC最大采样频率为2.4Msps，3路交替采样可到7.2Msps。

![image-20241221145609439](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241221145609439.png)

**ADCs_Common_Settings**          **ADC模式设置**
Mode     ADC_Mode_Independent
这里设置为**独立模式**

独立模式模式下，双ADC不能同步，每个ADC接口独立工作。所以如果不需要ADC同步或者只是用了一个ADC的时候，应该设成独立模式，多个ADC同时使用时会有其他模式，如双重ADC同步模式，两个ADC同时采集一个或多个通道，可以提高采样率。

**Data Alignment (数据对齐方式): 右对齐/左对齐**

这个上方有讲解，数据的左右对齐

**Scan Conversion Mode( 扫描模式 ) ：   DISABLE**

如果只是用了一个通道的话，DISABLE就可以了(也只能DISABLE)，如果使用了多个通道的话，会自动设置为ENABLE。 就是是否开启扫描模式

Continuous Conversion Mode(连续转换模式)    ENABLE

设置为ENABLE，即连续转换。如果设置为DISABLE，则是单次转换。两者的区别在于连续转换直到所有的数据转换完成后才停止转换，而单次转换则只转换一次数据就停止，要再次触发转换才可以进行转换

**Discontinuous Conversion Mode(间断模式)    DISABLE**

因为我们只用到了1个ADC,所以这个直接不使能即可

规则通道设置
**Enable Regular Conversions (启用常规转换模式)    ENABLE**

使能 否则无发进行下方配置

**Number OF Conversion(转换通道数)    1**
用到几个通道就设置为几
多个通道自动使能扫描模式



**Extenal Trigger Conversion Source外部触发源中断选择**

![image-20241221145931565](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241221145931565.png)

Regular Conversion launched by software **规则的软件触发** 调用函数触发即可

Timer X Capture Compare X event **外部引脚触发**,

Timer X Trigger Out event **定时器通道输出触发** 需要设置相应的定时器设置



**Rank转换顺序：**

![image-20241221150205664](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241221150205664.png)

**Channel：**ADC转换通道。

**Sampling Time：**通道采样时间。

**多通道会有多个Rank 可以设置通道的采样顺序**



## ADC总转换时间如下计算：

```
TCONV = 采样时间 + 12.5个周期 其中1周期为1/ADCCLK
```

当ADCCLK = 14MHZ时 采样时间为1.5个周期（最快）时，TCONV = 1.5 + 12.5 = 14周期 = 1us  

1周期 = 1/14MHZ = 71.43ns         TCONV = 14*71.43 = 1000.02ns = 1us 

## 注入通道：

![image-20241221153734256](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241221153734256.png)

和转换通道区别不大。

## WatchDog

![image-20241221153809748](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241221153809748.png)

**Enable Analog WatchDog Mode 是否使能模拟看门狗中断**

![在这里插入图片描述](https://i-blog.csdnimg.cn/blog_migrate/ae7851b6a00ff54fd5dad4a703193c15.png)
