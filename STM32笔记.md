

# STM32创建工程（标准库）

https://blog.csdn.net/2301_80655427/article/details/137519905#:~:text=2.%E5%BB%BA%E7%AB%8B%E6%A0%87%E5%87%86%E5%BA%93%E6%96%87%E4%BB%B6%E7%9A%84%E8%BF%87%E7%A8%8B%202.1%E5%88%9B%E5%BB%BA%E5%B7%A5%E7%A8%8B%E6%96%87%E4%BB%B6%E5%A4%B9%202.2%E6%89%93%E5%BC%80Keil%202.3%20%E5%9C%A8%E5%B7%A5%E7%A8%8B%E6%96%87%E4%BB%B6%E5%A4%B9%E4%B8%8B%E9%9D%A2%E5%86%8D%E5%88%9B%E4%B8%80%E4%B8%AA%E6%96%87%E4%BB%B6%E5%A4%B9,2.4%20%E5%B7%A5%E7%A8%8B%E5%90%8D%E7%A7%B0%202.5%20%E9%80%89%E6%8B%A9%E5%9E%8B%E5%8F%B7%203.%E6%B7%BB%E5%8A%A0%E5%B7%A5%E7%A8%8B%E5%BF%85%E8%A6%81%E6%96%87%E4%BB%B6

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

    

# I2C总线

![image-20240606092712532](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240606092712532.png)

![image-20240606092810741](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240606092810741.png)



![image-20240606093502769](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240606093502769.png)

![image-20240606093655906](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240606093655906.png)

![image-20240606093731285](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240606093731285.png)



# ADC转换（单通道）光敏电阻传感器

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

# 定时器时基单元

![image-20240702152141184](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240702152141184.png)

![image-20240702152210185](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240702152210185.png)

STM32单片机时钟来源

![image-20240702152403497](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240702152403497.png)

1. RCC时钟树
2. 来自从模式控制器的触发信号（TRIG）
3. 来自外部参考信号（ETRF）

## 计数模式

![image-20240702153611644](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240702153611644.png)

# 定时器输出比较

![image-20240702160931230](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240702160931230.png)

# STM32定时器的计算公式

![img](https://i-blog.csdnimg.cn/blog_migrate/4bbf3448dfe2956abbc8f8f95875f14b.jpeg)

公式解释：

ARR（TIM_Period）：自动重装载值，是定时器溢出前的计数值

PSC（TIM_Prescaler）：预分频值，是用来降低定时器时钟频率的参数

Tclk：定时器的输入时钟频率（单位Mhz），通常为系统时钟频率或者定时器外部时钟频率

Tout：定时器溢出时间（单位us）。一定要注意这个单位是us



# 独立看门狗

![image-20241023211322099](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241023211322099.png)

 看门狗这东西虽然简单，但我相信绝大多程序员没有足够重视它。使用看门狗保证系统正常地运行是非常有必要的。我们在设计产品时，代码以及硬件设计缺陷或是外界电磁干扰都有可能使系统死机，如果不能正常对其进行复位，系统的可靠性将大打折扣。看门狗分为软件看门狗和硬件看门狗两类，其原理都是使用一个独立定时器来计时，超出时间就会产生复位信号，主要区别看是否具有独立的硬件结构，如果有，就是硬件看门狗，如果是一个普通定时器实现的那么就是软件看门狗。STM32片内有两个看门狗：独立看门狗IWDG以及窗口看门狗WWDG。

## 两个狗的区别

- 独立看门狗没有[中断](https://so.csdn.net/so/search?q=中断&spm=1001.2101.3001.7020)，窗口看门狗有中断
- 独立看门狗有硬件软件之分，窗口看门狗只能软件控制
- 独立看门狗只有下限，窗口看门狗有下限和上限
- 独立看门狗是12位递减的。窗口看门狗是7位递减的
- 独立看门狗是用的内部的大约40KHZ RC振荡器，窗口看门狗是用的系统时钟APB1ENR
