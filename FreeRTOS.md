#  FreeRTOS的移植

1. 下载源码https://www.freertos.org/zh-cn-cmn-s/index.html

2. 拷贝需要文件：7个c文件，include文件，portable（内存，内核，2个c,1个h）和FreeRTOS的配置文件

3. 工程添加文件

4. 修改 配置文件.h

   ```c
   /*添加必须的宏*/
   
   #define xPortPendSVHandler              PendSV_Handler 
   #define vPortSVCHandler                 SVC_Handler
   #define INCLUDE_xTaskGetSchedulerState  1
   ```

5. 修改中断相关的文件 _it.c

   - 引入两个头文件

     ```c
     #include "FreeRTOS.h"
     #include "task.h"
     ```

   - 注释两个函数 ：Pendsv、svc

   - systick中断处理中，添加freeRTOS的函数（先extern引入）

6. 时基

   HAL库默认时基：systick

   FreeRTOS时基：默认systick

   ​	FreeRTOS有些地方会屏蔽中断，造成HAL_Delay不准确  当出现时将HAL时基改为TIMx

![image-20241104134916108](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241104134916108.png)

# 任务创建和删除的API函数

任务创建和删除的本质就是调用FreeRTOS的API函数

| API函数             | 描述         |
| :------------------ | ------------ |
| xTaskCreate()       | 动态创建任务 |
| xTaskCreateStatic() | 静态创建任务 |
| vTaskDelete()       | 删除任务     |



动态创建任务：任务的任务控制块以及任务的栈空间所需的内存，均由FreeRTOS从FreeRTOS管理的堆中分配（自动分配）

静态创建任务：任务的任务控制块以及任务的栈空间所需的内存，需用户分配提供（自行分配）



动态创建任务：

```c
BaseType_t xTaskCreate
(
	TaskFunction_t    			pxTaskCode,  //指向任务函数的指针
	const char* const     		pcName, //任务名字，最大长度configMAX_TASK_NAME_LEN
    const configSTACK_DEPTH_TYPE  usStackDepth, //任务堆栈大小，注意字为单位
    void* const                  pvParameters, //传递给任务函数的参数
    UBaseType_t                  uxPriority,//任务优先级，范围：0~configMAX_PRIORITIES-1
    TaskHandle_t* const          pxCreatedTask//任务句柄，就是任务的任务控制块
);
```

| 返回值                                |     描述     |
| :------------------------------------ | :----------: |
| pdPASS                                | 任务创建成功 |
| errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY | 任务创建失败 |

创建流程：

- 将宏configSUPPORT_DYNAMIC_ALLOCATION置为 1
- 定义函数入口参数
- 编写任务函数

静态创建任务：

```c
TaskHandle_t xTaskCreateStatic
(
	TaskFunction_t 				pxTaskCode,	//指向任务函数的指针
	const char* const            pcName,	//任务函数名
    const uint32_t 				ulStackDepth,//任务堆栈大小，以字为单位
    void *const 				pvParameters,//传递的任务函数参数
    UBaseType_t                  uxPriority,//任务优先级
    StackType_t* const			puxStackBuffer,//任务堆栈，一般为数组，由用户分配
    StaticTask_t * const 		pxTaskBuffer//任务控制块指针，由用户分配

)
```

| 返回值 |                 描述                 |
| ------ | :----------------------------------: |
| NULL   | 用户没有提供相应的内存，任务创建失败 |
| 其他值 |        任务句柄，任务创建成功        |

创建流程：

- 将宏configSUPPORT_STATIC_ALLOCATION配置为1
- 定义空闲任务&定时器任务的任务堆栈以及TCB（任务控制块）
- 实现两个接口函数

| 空闲任务的接口赋值 | vApplicationGetIdleTaskMemory()  |
| ------------------ | -------------------------------- |
| 软件定时器的赋值   | vApplicationGetTimerTaskMemory() |

- 定义函数的入口参数

- 编写任务函数

    

  ```c
  /*空闲任务*/
  /*定义空闲任务的资源参数*/
  StaticTask_t idle_task_pcb;
  StackType_t idle_task_stack[START_STASK_DEPTH]; 
  
  
      void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                          StackType_t ** ppxIdleTaskStackBuffer,
                                          uint32_t * pulIdleTaskStackSize )
      {
          *ppxIdleTaskStackBuffer = &idle_task_pcb;
          *ppxIdleTaskStackBuffer = idle_task_stack;
          *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
      }  
  ```

  

FreeRTOS变量命名规则

![image-20240530184759315](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240530184759315.png)

![image-20240530184846884](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240530184846884.png)

![image-20240530184902641](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240530184902641.png)



# 任务调度简介

-  抢占式调度：FreeRTOS采用抢占式调度方式，允许更高优先级的任务在任何时刻抢占正在执行的低优先级任务。这确保了高优先级任务能够及时响应，并提高了系统的实时性。

- 时间片轮转：在相同优先级的任务之间，FreeRTOS采用时间片轮转策略。每个任务执行一个时间片，如果有其他同优先级的任务等待执行，则切换到下一个任务。这有助于公平地分配CPU时间。

​		但是并不是说高优先级的任务会一直执行，导致低优先级的任务无法得到执行。如果高优先级任务**等待某个资源（延时或等待信号量等）**而无法执行，调度器会选择执行其他就绪的高优先级的任务。

**FreeRTOS中共存在4种状态：**

 **运行态：**当任务实际执行时，它被称为处于运行状态。如果运行 RTOS 的处理器只有一个内核， 那么在任何给定时间内都只能有一个任务处于运行状态。注意在STM32中，同一时间仅一个任务处于运行态。

**就绪态**：准备就绪任务指那些能够执行（它们不处于阻塞或挂起状态）， 但目前没有执行的任务， 因为同等或更高优先级的不同任务已经处于运行状态。

**阻塞态：**如果任务当前正在等待延时或外部事件，则该任务被认为处于阻塞状态。

 **挂起态**：类似暂停，调用函数 vTaskSuspend() 进入挂起态，需要调用解挂函数vTaskResume()才可以进入就绪态。

只有就绪态可转变成运行态，其他状态的任务想运行，必须先转变成就绪态。转换关系如下：

![Free](C:\Users\15819\Desktop\Free.png)

这四种状态中，除了运行态，其他三种任务状态的任务都有其对应的任务状态列表：

**就绪列表：**pxReadyTasksLists[x]，其中x代表任务优先级数值。

**阻塞列表：**pxDelayedTaskList。

**挂起列表：**xSuspendedTaskList。

以就绪列表为例。如果在32位的硬件中，会保存一个32位的变量，代表0-31的优先级。当某个位，置一时，代表所对应的优先级就绪列表有任务存在。

![ss](C:\Users\15819\Desktop\ss.png)

如果有多个任务优先级相同，会连接在同一个就绪列表上：

![aa](C:\Users\15819\Desktop\aa.png)

调度器总是在所有处于就绪列表的任务中，选择具有最高优先级的任务来执行。

![task](C:\Users\15819\Desktop\笔记\task.png)

# 任务挂起和恢复

```c
vTaskSuspend();//挂起任务, 类似暂停，可恢复
vTaskResume();//恢复被挂起的任务
xTaskResumeFromISR();//在中断中恢复被挂起的任务
```

- 任务挂起函数

  ```c
  //参数：传入挂起任务的句柄
  vTaskSuspend(TaskHandle_t xTaskToSuspend);
  ```

- 任务恢复函数

  ```c
  //参数：传入恢复任务的句柄
  vTaskResume();//恢复被挂起的任务
  ```

- 任务恢复函数（中断中恢复）

  ```c
  /*
  参数：要恢复的任务句柄
  返回值：pdTRUE:任务恢复后需要进行切换
  	   pdFALSE:任务恢复后不需要进行任务切换
  */
  Basetype_t xTaskResumeFromISR();//在中断中恢复被挂起的任务
  /*
  注意事项：
  INCLUDE_vTaskSuspend 和 INCLUDE_xTaskResumeFromISR 必须定义为 1。
  在中断服务程序中调用FreeRTOS的API函数时，中断的优先级不能高于FreeRTOS所管理的最高任务优先级。
  STM32的中断优先级越小越大。
  */
  ```

  

# RTOS中的任务调度

![img](https://note.youdao.com/yws/public/resource/1a5cec5c23012eab66471f690e7c3a1f/xmlnote/WEBc1696fafd45502c8525ec20af22312c7/WEBRESOURCEd4871d8cded133762bd02cc6f52d4166/71473)

**OS任务调用策略：**

**优先级高的先执行，在没有更高优先组任务时，低优先级任务再执行。**

**同等优先级的任务会轮转**（轮流、时间片轮转是一个意思）**执行。**

**为保障退出的任务资源可以让空闲任务回收资源，所以必须保障0优先级的任务可以得到CPU的时间片。**

**所以在编写程序时，如任务空闲或等待资源时应让任务进入阻塞状态，以便idle任务可以获取时间片，进行资源的清理。**

# 任务间的通信方式xQueue(队列或消息队列)

- **为何使用队列进行任务间的通信，而非全局变量呢？**

与Linux进程间通信方式类似，FreeRTOS中的通知也采用的队列方式，其本质与Linux一样都是使用的一种环形队列。不过这个数据结构不需要我们去构建，已经封装好了。我们只需要操作上层的接口实现任务间的数据传递即可。（当然任务间在FreeRTOS中也可以使用全局变量传递，但全局变量在多任务中可能会产生竞态的问题）。**FreeRTOS中的队列是用于任务间通信和同步的重要机制。它们提供了线程安全的数据传输方式，并支持阻塞和非阻塞的发送和接收操作。通过使用队列，任务和中断服务程序可以高效、安全地共享数据。**

![img](https://note.youdao.com/yws/public/resource/1a5cec5c23012eab66471f690e7c3a1f/xmlnote/WEBc1696fafd45502c8525ec20af22312c7/WEBRESOURCE8cca5acaf65d157e8b4adc7d46db7f90/71724)

![img](https://note.youdao.com/yws/public/resource/1a5cec5c23012eab66471f690e7c3a1f/xmlnote/WEBc1696fafd45502c8525ec20af22312c7/WEBRESOURCE1fde860ef5474f0aa6a4447f864325a6/71527)

- ## **xQueue队列的基本功能介绍：**

- FIFO（先进先出）：队列遵循先进先出的原则，即最早进入队列的项目最早被移除。
- 多任务通信：队列可以在多个任务之间共享数据，一个任务可以向队列发送数据，另一个任务可以从队列接收数据。
- 线程安全：FreeRTOS提供的队列机制是线程安全的，确保多任务访问队列时不会出现竞争条件或数据损坏。
- 队列默认使用拷贝赋值方式传递数据，而不是引用



1. ### xQueue的创建 xQueueCreate:

   ```c
   QueueHandle_t xQueueCreate( UBaseType_t uxQueueLength,
                                UBaseType_t uxItemSize );
   功能：动态创建一个队列，并返回队列的句柄。
   参数：uxQueueLength 队列可以容纳的最大项目数。
   参数：uxItemSize 存储队列中的每个数据项所需的大小（以字节为单位）
   返回值：
   如果队列创建成功，则返回所创建队列的句柄。 
   如果创建队列所需的内存无法 分配 ，则返回 NULL。
   
   与创建任务类似也提供了相应的静态创建队列的方法：
    xQueueCreateStatic() 创建队列，相应的方法介绍请查看FreeRTOS在线手册。
   ```

2. ### 向队列中发送数据 :xQueueSend

   ```c
   BaseType_t xQueueSend(QueueHandle_t xQueue,
                         const void * pvItemToQueue,
                         TickType_t xTicksToWait);
   功能：在队列中发布项目。该项目按副本排队，而不是按引用排队。
   不得从中断服务程序调用此函数。请参阅 xQueueSendFromISR() 以获取 可用于 ISR 的替代方案。
   参数：xQueue 队列的句柄，数据项将发布到此队列。
   参数：pvItemToQueue 指向待入队数据项的指针
   参数：xTicksToWait 如果队列已满，则任务应进入阻塞态等待队列上出现可用空间的最大时间。
   等待的最大时间可以使用宏：protMAX_DELAY,也可以使用-1或者~0
   
   如果队列已满，并且 xTicksToWait 设置为0 ，调用将立即返回。
   返回：如果成功发布项目，则返回 pdTRUE，否则返回 errQUEUE_FULL。
   ```

3. ### 从队列中接收数据后删除：xQueueReceive 只读取不删除数据：xQueuePeek()

   ```c
   BaseType_t xQueueReceive(QueueHandle_t xQueue,
                            void *pvBuffer,
                            TickType_t xTicksToWait);
   功能：从队列中接收项目。该项目通过复制接收，因此必须提供足够大小的缓冲区。
   中断服务程序中不得使用此函数。请参阅 xQueueReceiveFromISR() 了解可以选择的替代方案
   参数：xQueue 要从中接收项目的队列的句柄。pvBuffer 指向缓冲区的指针，接收到的项目将被复制到这个缓冲区。
   参数：xTicksToWait  如果在调用时队列为空，则任务应阻塞等待项目接收的最长时间。 
   返回：
   如果从队列成功接收到项目，返回 pdTRUE，否则返回 pdFALSE。 
   ```

   

# 信号量

信号量，顾名思义就是传递信号的量，信号量也是一种队列，只不过有时候我们需要传递一个状态，并不需要传递具体信息，所以使用信号量更简洁，方便。

![img](https://note.youdao.com/yws/public/resource/1a5cec5c23012eab66471f690e7c3a1f/xmlnote/WEBc1696fafd45502c8525ec20af22312c7/WEBRESOURCEe12512f9fde06a64e916162fe59015ec/71752)

信号量和队列一样也是一种实现任务通信的机制，可以实现任务之间的同步或临界资源的互斥访问。

所谓的同步，类似于生产与消费的关系，只有生产这生产数据之后，消费者才能操作数据。但是FreeRTOS中，数据的采集和处理往分成多个模块，所以需要我们协调它们的执行顺序，协调执行顺序就是同步。目的是让多个程序严格按照一定的顺序执行，保证程序执行顺序的正确。

互斥：就是同一个资源，防止被多个程序同时访问。比如一个led小灯，同一时刻只能让一个任务取访问，如果多个任务访问，可能会出现状态混乱。同步与互斥是类似的概念，只是侧重点有点小小的区别：同步侧重于协调任务的顺序。而互斥侧重与共享资源的保护。

例如：

任务A在没有获取数据前，B任务想要使用数据，那么就是阻塞等待，这就是同步。

任务A访问共享资源时，B任务也想访问，但是访问会被拒绝，只能阻塞等待或退出，这就是互斥。

所以说同步和互斥是类似的概念，理解就行。

FreeRTOS信号量有以下几种类型：

1. 计数信号量
2. 二值信号量
3. 互斥信号量
4. 递归互斥信号量

计数信号量和二值信号量

计数信号量就是计数值没有限制的信号量

二值信号量就是只有0和1的信号量。

## 互斥信号量

这两个信号量主要用于互斥而产生的，但有一个机制是继承优先级，防止优先级翻转的问题。如下图：

![img](https://note.youdao.com/yws/public/resource/1a5cec5c23012eab66471f690e7c3a1f/xmlnote/WEBc1696fafd45502c8525ec20af22312c7/WEBRESOURCE6005e217045a1fc106956a18304d6102/71787)

继承优先级机制：即当高优先级任务在等待低优先级任务资源时，会临时提高低优先级任务的优先级与高优先级任务相同，主要用来防止优先级翻转的问题。

可以理解为：互斥信号量 = 二值信号量+继承优先级机制

互斥信号量会产生任务的阻塞，不可在中断ISR中使用。

所以在同一任务中，使用take和give保护任务的执行，达到互斥的效果，所以说互斥量也称为互斥锁。

**注意：在使用互斥锁时，give之后一定要留足够的任务切换时间！**

## 递归互斥量;

递归互斥量很好的解决了互斥量的两个缺陷：

1. 假设任务1获取了互斥信号量，那么本应该任务1释放的信号量，但实际上其他任务也能释放信号量，如果其中某个任务释放并获取了信号量，那么就会产生错误。
2. 死锁：在同一任务中获取两次互斥锁，第一次可以获取，第二次因为未释放而进入阻塞，因为无法释放所以死锁。

那么使用递归互斥量就可以很好的解决以上的问题;

谁获取谁释放，解决第一个问题。

获取互斥量的任务可以多次释放，但也需要相同次数的释放。解决第二个缺陷。

递归互斥量也是基于互斥量实现的，所以也一样具有优先级继承机制，解决优先级翻转而产生的高优先级任务最后执行的问题。

# 多任务通信之队列集：xQueue_Set

队列集：集中管理，同一操作！

![img](https://note.youdao.com/yws/public/resource/1a5cec5c23012eab66471f690e7c3a1f/xmlnote/WEBc1696fafd45502c8525ec20af22312c7/WEBRESOURCEc1ce0468bcb99bcc0d9a6bb26cc9600e/71815)

## 创建队列集：xQueueCreateSet():

```c
QueueSetHandle_t xQueueCreateSet(const UBaseType_t uxEventQueueLength);
 显式创建队列集， 然后才能使用它。
 创建后，可以将标准 FreeRTOS 队列和信号量添加到集合中
 （通过调用 xQueueAddToSet()）。然后，使用 xQueueSelectFromSet() 确定集合中包含的队列或信号量中的哪些
 （如果有） 处于队列读取或信号量获取操作将成功的状态。
 参数：uxEventQueueLength 队列集存储集合中包含的队列和 信号量上发生的事件。 
 uxEventQueueLength 指定一次可以排队的最大事件数 。
 返回值：如果成功创建队列集，则返回所创建队列集的句柄 。 否则返回 NULL。
 使用此函数时请关注手册中的注意事项：
 1.队列和信号量在添加到队列集时必须为空 。
 2.阻塞包含互斥锁的队列集不会导致 互斥锁持有者继承已阻塞任务的优先级。
 3.不得对队列集的成员执行接收（若为队列）或获取（若为 信号量）操作，
 除非 调用 xQueueSelectFromSet() 先返回了队列集成员的句柄。
```

**注意：队列集中不推荐添加互斥锁,因为xQueueSelectFromSet()是阻塞的，是同步的，就没有互斥的特性了，再有就是互斥锁在队列集使用时将失去优先级继承机制，所以不推荐在队列集中使用互斥锁。**



## **添加队列或信号量到队列集：xQueueAddToSet**

```c
BaseType_t xQueueAddToSet (QueueSetMemberHandle_t xQueueOrSemaphore,
                          QueueSetHandle_t xQueueSet);
功能：将 RTOS 队列或信号量添加至先前由 xQueueCreateSet() 调用创建的队列集。
参数：
xQueueOrSemaphore 正在添加到队列集的队列或信号量的句柄 （转换为 QueueSetMemberHandle_t 类型）。
xQueueSet 正在添加队列或信号量的队列集句柄 。

返回值：
如果队列或信号量成功添加到队列集 那么返回 pdPASS。 
如果队列无法成功添加到 队列集，因为它已经是其他队列集的成员，那么返回 pdFAIL 。
```

## **.从队列集中选择（包含数据的）队列(或可供获取的)信号量的句柄xQueueSelectFromSet：**

```C
QueueSetMemberHandle_t xQueueSelectFromSet (QueueSetHandle_t xQueueSet,
                                             const TickType_t xTicksToWait);
功能：
xQueueSelectFromSet() 从队列集成员中选择队列或信号量， 它们要么包含数据（若选择队列），要么可供获取 （若选择信号量）。
xQueueSelectFromSet() 能有效 允许任务同时读取一个队列集中的所有 队列和信号量后阻塞（挂起）。
参数：
xQueueSet 任务（可能）阻塞的队列集。
xTicksToWait 调用任务保持阻塞状态（其他任务正在执行），
等待队列集成员做好准备 以便成功读取队列或获取信号量所需的最长时间， 以滴答为单位。
返回值：
xQueueSelectFromSet() 将返回 队列集中包含数据的队列的句柄（转换为 QueueSetMemberHandle_t 类型） 
或队列集中可用信号量的句柄（转换为 QueueSetMemberHandle_t 类型）， 
如果在指定的阻塞时间到期之前不存在这样的队列或信号量， 则返回 NULL。
同理此函数也有在ISR中的操作函数，加上后缀FromISR()即可。提供一种非阻塞的操作用于中断处理函数中。
```

## **从队列集中删除 RTOS 队列或信号量。xQueueRemoveFromSet:**

```c
BaseType_t xQueueRemoveFromSet( QueueSetMemberHandle_t xQueueOrSemaphore,
                              QueueSetHandle_t xQueueSet);
功能：从队列信中删除队列或信号：仅当队列或信号量为空时，才能从队列集中删除 RTOS 队列或信号量 。

参数：
xQueueOrSemaphore  从队列集中删除的队列或信号量的句柄 （转换为 QueueSetMemberHandle_t 类型）。
xQueueSet  包含队列或信号量的队列集的句柄 。
返回：
如果队列或信号量已成功从队列集中删除， 则返回 pdPASS。 
如果队列不在队列集中，或者 队列（或信号量）不为空，则返回 pdFAIL。
```

# 事件组 xEventGroup

事件组是**更加精简的一种类似于队列集的方式**。但是它是一种**无队列的同步机制，**可以实现多个任务等待同一个或多个事件的同步机制。

结构：

使用一个4字节基本整形来表示一个事件组，高8位留，可处理24个事件。每个bit是事件位表示一个关心的事件是否产生，0为没有产生，1为产生了事件。

![image-20240730101245218](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240730101245218.png)

![image-20240730101249886](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240730101249886.png)

## 事件组常用API

### 创建事件组：xEventGroupCreate

```c
EventGroupHandle_t xEventGroupCreate( void );
//创建一个事件组
返回值：成功返回事件组句柄，失败返回NULL;
```

### 设置事件组中的相应的事件位：xEventGroupSetBits

```c
EventBits_t xEventGroupSetBits( EventGroupHandle_t xEventGroup, 
                         const EventBits_t uxBitsToSet );
参数解释
xEventGroup: 事件组的句柄，标识哪个事件组将被设置。
uxBitsToSet: 这是一个掩码，表示调用任务需要设置的位。
多个位可以使用按位或（|）操作符组合在一起。
返回值
返回事件组设置位之前的值。
这是一个 EventBits_t 类型的值，其中包含所有事件组位在设置前的状态。 
```

### 阻塞等待事件的产生

```c
EventBits_t xEventGroupWaitBits(
    EventGroupHandle_t xEventGroup,
    const EventBits_t uxBitsToWaitFor,
    const BaseType_t xClearOnExit,
    const BaseType_t xWaitForAllBits,
    TickType_t xTicksToWait
);
参数解释
xEventGroup: 事件组的句柄，标识哪个事件组将被等待。

uxBitsToWaitFor: 这是一个掩码，表示调用任务需要等待的位。
多个位可以使用按位或（|）操作符组合在一起。

xClearOnExit: 如果为 pdTRUE，则在返回前清除事件组中满足条件的位。

xWaitForAllBits: 
如果为 pdTRUE，则仅在所有位（由 uxBitsToWaitFor 参数指定）都被设置时才返回。
如果为 pdFALSE，则在任意一个位被设置时返回。

xTicksToWait: 调用任务在事件位变为所需状态之前要等待的最大时间（以系统节拍计时）。
如果设置为 portMAX_DELAY，则表示无限期等待。

返回值
返回事件组当前的值。这是一个 EventBits_t 类型的值，其中包含所有事件组位的当前状态。
任务可以根据返回值判断哪些位被设置。
```

# 任务通知

## 什么是任务通知：

FreeRTOS提供了一种超轻量级的任务间同步机制。它们非常高效，并且不需要使用复杂的数据结构或内存分配，即无需队列，也无需要事件组，因此非常适合在嵌入式系统中使用, **它就是任务通知**。

它实现任务一对一的同步通信机制，没有广播功能。只能任务或ISR向某个任务发出通知。

## 与其他同步方式对比

![image-20240730123324254](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240730123324254.png)

freeRTOS官方这样说道：任务通知具有高度灵活性，使用时无需 创建单独[队列](https://www.freertos.org/zh-cn-cmn-s/Embedded-RTOS-Queues.html)、 [二进制信号量](https://www.freertos.org/zh-cn-cmn-s/Embedded-RTOS-Binary-Semaphores.html)、 [计数信号量](https://www.freertos.org/zh-cn-cmn-s/Real-time-embedded-RTOS-Counting-Semaphores.html) 或[事件组](https://www.freertos.org/zh-cn-cmn-s/FreeRTOS-Event-Groups.html)。 通过直接通知解除 RTOS 任务阻塞状态的速度和使用中间对象（如二进制信号量）相比快了 **45%** *****， **使用的 RAM** 也更少 。 



## 常用的任务通知的API:

### 简化版任务通知：当作轻量级信号量的方式：xTaskNotiftuGive, ulTaskNotifyTake

当任务通知 用作[轻量级且更快的二进制或计数信号量 ](https://www.freertos.org/zh-cn-cmn-s/RTOS-task-notifications.html#uses)替代方案时，可以使用宏 xTaskNotifyGive ()归还。通另一个函数xTaskNotifyTake()获取。

```c
1. BaseType_t xTaskNotifyGive( TaskHandle_t xTaskToNotify);
功能：向指定任务发送通知，增加其通知值。这种形式的通知常用于同步操作。
参数：xTaskToNotify: 要通知的任务的句柄。
返回值：成功时返回 pdPASS，失败时返回 errQUEUE_FULL。


2. uint32_t ulTaskNotifyTake( BaseType_t xClearCountOnExit, 
                                TickType_t xTicksToWait );
功能：收任务通知的函数，用于等待任务通知并减少通知值。
参数：
xClearCountOnExit：一个布尔值，指示在接收到通知后是否清除通知值。
    pdTRUE：函数返回时清除通知值。
    pdFALSE：函数返回时不清除通知值。
xTicksToWait: 等待通知的最大时间（以 tick 为单位）。
返回值：
    返回接收到的通知计数值。
```

### 专业版任务通知:xTaskNotify , xTaskNotifyWait等待接收通知

```c
 BaseType_t xTaskNotify( TaskHandle_t xTaskToNotify,                        
                         uint32_t ulValue,                         
                         eNotifyAction eAction );
功能： 用于在任务间发送通知。
它允许你对任务的通知值进行灵活的操作，可以设置、清除、递增通知值，或进行简单的消息传递。
参数
xTaskToNotify: 要通知的任务句柄。
ulValue: 要设置的通知值。具体操作取决于 eAction 的值。
eAction: 通知操作的类型。这个枚举类型决定了如何处理 ulValue。可以是以下值之一：
    eNoAction：不对通知值进行任何操作，仅触发通知。
    eSetBits：将通知值中的指定位设置为 1。可实现轻量级事件组
    eIncrement：将通知值递增 1。可实现轻量级计数信号量
    eSetValueWithOverwrite：将通知值设置为 ulValue，如果任务已有通知值，则覆盖旧值。
    eSetValueWithoutOverwrite：将通知值设置为 ulValue，如果已有值则不修改。
返回值
pdPASS: 如果操作成功。pdFAIL: 如果操作失败（例如，目标任务句柄无效）。       



2.BaseType_t xTaskNotifyWait(uint32_t ulBitsToClearOnEntry, 
                             uint32_t ulBitsToClearOnExit,
                             uint32_t *pulNotificationValue, 
                             TickType_t xTicksToWait);    
功能：用于在任务中等待通知，并在接收到通知后获取通知值。
这个函数允许任务在等待通知的过程中，可以选择是否清除通知值的特定位。
参数
ulBitsToClearOnEntry：在任务进入等待状态时要清除的通知值的位掩码。
如果设置为 pdFalse，则不清除任何位。pdTrue，则清除。
ulBitsToClearOnExit：在任务退出等待状态时要清除的通知值的位掩码。
如果设置为 pdFalse，则不清除任何位。pdTrue，则清除。

pulNotificationValue：指向一个 uint32_t 类型变量的指针，用于接收通知值。
如果不需要接收通知值，可以将其设置为 NULL。

xTicksToWait：等待通知的最长时间（以 tick 为单位）。
如果设置为 portMAX_DELAY，任务将无限期等待直到接收到通知。

返回值
如果接收到通知，则返回 pdPASS，并将通知值存储在 pulNotificationValue 指向的变量中（如果 pulNotificationValue 不为 NULL）。
如果在指定时间内未接收到通知，则返回 pdFAIL。
```

### 实例代码演示：

生产者：

```C
#include "beep_ldr_app.h"
#include "led_app.h"

struct beep_ldr_app beep_ldr = {0};

extern struct  led_app led_app_task;


void beep_ldr_app_function(void* arg)
{
    int data = 0;
    while (1)
    {
        if(isDark())
        {
            /*光线暗时将led灯闪烁的任务挂起*/
            // vTaskSuspend(led_app_task.led_app_handle);
            // beep_start();
            // vTaskDelay(500);
            data = 1;
            xTaskNotify(led_app_task.led_app_handle,data,eSetValueWithOverwrite);

        }
        else
        {
            /*光线亮时led灯闪烁的任务恢复*/
            // vTaskResume(led_app_task.led_app_handle);
            // beep_stop();
            // vTaskDelay(500);
            data = 2;
            xTaskNotify(led_app_task.led_app_handle,data,eSetValueWithOverwrite);
        }
    }
    
}

```

消费者:

```c
#include "led_app.h"


struct led_app led_app_task = {0};

void Led_App_Function(void *arg)
{
    uint32_t data = 0;
    while (1)
    {
        // led_on();
        // vTaskDelay(500);
        // led_off();
        // vTaskDelay(500);
        xTaskNotifyWait(pdTRUE,pdTRUE,&data,portMAX_DELAY);
        if(data == 1)
        {
            led_off();
            vTaskDelay(200);
        }
        else if(data == 2)
        {
            led_on();
            vTaskDelay(200);
        }
    }
    
}

```

# 总结：

![image-20240730133625612](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240730133625612.png)

# STM32中断处理体系结构



## 中断处理路径

![image-20240730135018845](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240730135018845.png)

## NVIC中断控制器的中断优先级：

### 中断号：

在NVIC中对于硬件产生的任何一个中断都分配了一个中断号，中断号是一个唯一的标识符，用于识别每个外设设备的中断。NVIC使用中断号来配置中断的优先级和使用的状态。如在F103数据手册上的介绍：

![image-20240730135111596](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240730135111596.png)

*（在向量表中列出的 "位置"（Position）实际上就是中断号（IRQ number）。这些中断号用于在嵌套向量中断控制器（NVIC）中配置和管理中断）*



### 中断优先级的组成：

NVIC中对中断优先级进行了分组：优先级分别有两部分组成：**抢占优先级+ 子优先级**

**抢占优先级：决定了是否可以抢占。**

**子优先级：决定了优先级的处理顺序。**

**（中断的分组优先级的抢占优先级与子优先级大部分可由用户自行配置。如果未配置则使用默认值，数据越小，优先级越高。）**

例如：当两个相同的分组优先相同，子优先不同的中断，不会发生抢占，而是按子优先级顺序依次执行。



### 中断处理例程ISR:

对中断进行响应的函数，又称中断服务例程，是CPU中断当前任务保存现场后去执行的函数。

在中断处理例程中要执行一些实时性高的操作，不可以出现阻塞。也不允许在ISR使用可能出现阻塞的操作。

对于每一个中断号,MCU的启动文件startup_stmf103c8t6.s的汇编启动文中已经定义好了相应的ISR中断全程函数的句柄。

在启动文件中定义两部分中断的ISR,一部分是系统内置的中断服务例程的句柄，另一部则是外部中断源的中例ISR，大多也都是一些外设中断源的ISR句柄。



### __week弱函数，可实现的函数，是一种函数回调的方式

__weak函数修饰符是ARM编译器中的一种语法形式。此函数可以实新实现，类似于C++虚函数。

本质是个函数回调。在代码中重新实现即可。编译时会直接编译重新实现的函数。

**重点一句话：以上讲解了很多的概念，但是在开发中，只需要搞清楚与中断事件对应的ISR的回调的__weak函数，重新实现相关的ISR逻辑即可，注册ISR的回调的函数中不可以有阻塞。**

### 重写__week函数，实现ISR逻辑：实现按键响应的功能;

![image-20240730145619297](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240730145619297.png)

## FreeRTOS中对中断的处理：

由于材料的物理特性及电学特性，当按键按下的瞬间，会短时间出现电平的高低变化，如图所示：

![image-20240730145653339](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240730145653339.png)

按键抖动的问题，如何消抖，下面介绍几种方式：

### FreeRTOS软件定时器进行延时消抖:

#### 简述FreeRTOS定时器：

在RTOS调度启动时，会自动创建一个空闲idle任务，与一个定时器守护任务。那么定时器的功能就是由这个定时器守护任务执行相应的回调函数实现的。

软件定时器有一个定时周期，还有一个回调函数。根据回调函数执行的频度，有两种类型的软件定时器

单次定时器（one-shot timer）,即回调函数被执行一次后，定时器停止工作。

周期定时器（periodic timer）,回调函数会被循环执行，定时器一直工作。

#### 定时器有休眠和运行两种状态：

**休眠状态**

定时器使用**xTimerCreate**创建后就处于休眠状态，单次定时器执行一次回调函数后进入休眠状态。定时器使用函数**xTimerStop**停止后进行休眠状态。

**运行状态：**

处于运行状态的定时器在超时时间到达后就会执行其回调函数，不定单次定时器，还是周期定时器。

定时器在以下几种状态下处于运行状态。

使用函数**xTimerStart**启动后，定时器进行运行状态。

定时器在运行状态时，被执行函数**xTimerReset**复位起始时间后继续处于运行状态。

**软件定时器的各种操作实际上是在FreeRTOS系统的定时器守护任务中完成的。**

#### 软件定时器的机制与使用配置：

在用户任务里执行的各种定时器操作函数都是通过一个队列发送给这个定时器守护任务的，所以这个执行同步的中间的队列，也被称为定时器指令队列，定时器守护任务读取定时器指令队列中的指令，然后执行相庆的操作：如图所示：

![image-20240730150034714](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240730150034714.png)

**为了保证守护定时器任务可以得到时间片，最好配置守护任务的调度优先级高于用户任务的优先级。**

同时，如果要使用软件定时器，需要在进行如下配置（官方文档介绍）

![image-20240730150048311](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240730150048311.png)

即需要在FreeRTOSConfig.h中进行以上宏定义，并指定合适的值。

### 软件定时器常用的API接口函数：

#### 动态创建定时器

```c
timers.h
 TimerHandle_t xTimerCreate
                 ( const char * const pcTimerName,
                   const TickType_t xTimerPeriod,
                   const UBaseType_t uxAutoReload,
                   void * const pvTimerID,
                   TimerCallbackFunction_t pxCallbackFunction );

功能：创建一个新的软件定时器实例， 并返回一个可以引用定时器的句柄。
参数：
pcTimerName  分配给定时器的可读文本名称。 这样做纯粹是为了协助 调试。
xTimerPeriod  
定时器的周期。 以 tick 为单位指定此周期，
宏 pdMS_TO_TICKS() 可用于将以毫秒为单位指定的时间转换为以 tick 为单位指定的时间。
uxAutoReload  ： pdTRUE重新加载，pdFALSE执行一次结束。
pvTimerID  分配给正在创建的定时器的标识符。传递给回调函数的参数。
pxCallbackFunction  定时器到期时调用的函数
返回值：
如果定时器创建成功， 则返回新创建的定时器的句柄。失败返回NULL;
当于此动态创建定时器，也有静态创建定时器，详阅官方文档。
```

#### 启动定时器阻塞版xTimerStart与非阻塞版xTimerStartFromISR

```c
1. BaseType_t xTimerStart( TimerHandle_t xTimer,
                          TickType_t xBlockTime );
功能：启动定时器，在队列满后会阻塞当前调用任务。
参数：
xTimer  正在启动/重新启动的定时器的句柄。
xBlockTime  在调用 xTimerStart() 时队列已满的情况下， 
指定调用任务应保持阻塞状态 以等待启动命令成功发送到定时器命令队列的时间 （以 tick 为单位）。 
如果在 启动 RTOS 调度器之前调用 xTimerStop()，则会忽略 xBlockTime
成功返回pdPASS,失败返回pdFAIL.

2. BaseType_t xTimerStartFromISR(TimerHandle_t xTimer,
                      BaseType_t *pxHigherPriorityTaskWoken);
功能：可从中断服务例程调用的 xTimerStart() 的非阻塞版本
参数：
xTimer  正在启动/重新启动的定时器的句柄
pxHigherPriorityTaskWoken : 如果守护进程的优先级高于当前任务会返回pdTRUE，如果为pdTRUE，
那么应在中断退出前执行上下文切换，这样可以让高优先级任度马上得到执行。
可以调用内核函数portYIELD_FROM_ISR(xHigherPriorityTaskWoken);出让时间片。
成功返回pdPASS,失败返回pdFAIL
```

#### 定时器运行种重置定时器函数xTimerReset与xTimerResetFromISR

```c
timers.h
 1.BaseType_t xTimerReset( TimerHandle_t xTimer,
                            TickType_t xBlockTime );
功能介绍：xTimerReset() 重新启动先前使用 xTimerCreate() API 函数创建的定时器。 
如果定时器已经启动且已处于活动状态， 那么 xTimerReset() 会使定时器 重新评估其到期时间，
因此到期时间与 xTimerReset() 的调用时间有关。  
如果定时器处于休眠状态，则 xTimerReset() 具有与 xTimerStart() API 函数等效的功能。
参数：
xTimer  正在重置/启动/重新启动的定时器的句柄。
xBlockTime  队列满中的阻塞时间
返回值：
成功返回pdPASS, 失败返回pdFAIL    

2.BaseType_t xTimerResetFromISR(TimerHandle_t xTimer,
                   BaseType_t *pxHigherPriorityTaskWoken);   
功能：与之reset阻塞版相同。
参数：与start一样的参数功能
返回值：与start一样的返回值。    
```

#### 删除定时器 xTimerDelete

```c
 BaseType_t xTimerDelete( TimerHandle_t xTimer,
                             TickType_t xBlockTime );
功能：删除正在使用的定时器，并回收内存。
成功：返回pdPASS,失败返回pdTRUE
```

#### 软件定时器的按键消抖

key_app.c

```c
#include "key_app.h"


extern TimerHandle_t mykyeTimerhandle;


/*中断函数*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    /*判读是不是引脚的中断*/
    if(GPIO_Pin == GPIO_PIN_14)
    {
        BaseType_t status = pdFALSE;
        /*重置启动定时器*/
        xTimerResetFromISR(mykyeTimerhandle,&status);
        if(status == pdTRUE)
        {
            portYIELD_FROM_ISR(status);
        }
    }
}

void myKeyTimer_function(TimerHandle_t xTimer)
{
    if(!isKeyPress(GPIOB,GPIO_PIN_14))
    {
        led_on();
    }
    else
    {
        led_off();
    }
}


```

# UART异步串行总线

## 异步通信：

即通信双方，均独立工作，即有各自的时钟源。

发送方发出数据后，不需要等待接收方即时反馈与确认，可以继续进行其他操作。

因为异步通信没有使用同步的时钟，所以通常需要额外的控制信息，如：起始位，停止位，校验位，来标识数据的开始和结束，确定数据的正确传输和解析。

常见于有两个有独立时钟源的设备间的通信。

如：UART就是一种异步通信方式。

## 同步通信：

即通信双方，在共享的同一时钟下工作，使用同一时钟源。

发送方发出数据后，会等待接收方确认或反馈，然后才能进行继续下一步操作。

因为同步通信使用同一时钟，所以收发双方需要严格按照约定好的时序进行操作。

常见于在同一MCU或MPU下与多个外设设备的通信。

如：I2C，SPI就是一种同步通信方式。

## 工作模式：全双工和半双工

全双工是指：在同一时刻即可发送数据，亦可接收数据。所以一般情况为有两条数据线，发送线与接收线。

半双工是指：在同一时刻只能发送数据或接收数据，所以只需要一条数据线即可。

UART串口是一个全双工的异步的通信接口.

# UART串口

## UART通信协议的时序：从低到高字节发（LSB-->MSB）

![image-20240731101008608](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240731101008608.png)

## UART控制器硬件接口以及内部框图

### 接口连接

通过三个引脚与其他设备连接在一起(见图248)。

任何USART双向通信至少需要两个脚：接收数据输入(RX)和发送数据输出(TX)。

RX：接收数据串行输。通过过采样技术来区别数据和噪音，从而恢复数据。

TX：发送数据输出。

![image-20240731101110296](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240731101110296.png)

### UART控制器硬件框图

![image-20240731101134302](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240731101134302.png)

### UART串口驱动的HAL库API

![image-20240731101221397](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240731101221397.png)

### 阻塞版发送数据

```c
HAL_StatusTypeDef HAL_UART_Transmit (UART_HandleTypeDef * huart, uint8_t * pData, uint16_t Size,
uint32_t Timeout)
功能：通过 UART（通用异步收发传输器）发送数据
参数：
UART_HandleTypeDef *huart:一个指向 UART_HandleTypeDef 结构体的指针，该结构体包含指定 UART 模块的配置信息。
这个结构体包括 UART 外设的实例，以及其配置和状态信息。
uint8_t *pData:一个指向数据缓冲区的指针，缓冲区中包含要发送的数据。
uint16_t Size:要发送的数据的字节数。
uint32_t Timeout:等待传输完成的超时时间（以毫秒为单位）。
如果在这个时间内传输没有完成，函数将返回超时错误。

返回值
该函数返回一个 HAL_StatusTypeDef 类型的值，可以是以下几种之一：
HAL_OK: 函数执行成功。
HAL_ERROR: 执行过程中发生错误。
HAL_BUSY: UART 当前正忙于之前的操作。
HAL_TIMEOUT: 在操作完成前函数超时。
```

### 阻塞版接收数据

```c
HAL_StatusTypeDef HAL_UART_Receive (UART_HandleTypeDef * huart, uint8_t * pData, uint16_t Size,
uint32_t Timeout)
函数功能：通过 UART（通用异步收发传输器）接收数据.
参数说明
UART_HandleTypeDef *huart:一个指向 UART_HandleTypeDef 结构体的指针，
该结构体包含指定 UART 模块的配置信息。这个结构体包括 UART 外设的实例，以及其配置和状态信息。
uint8_t *pData:一个指向数据缓冲区的指针，接收的数据将存储在这个缓冲区中。
uint16_t Size:要接收的数据的字节数。
uint32_t Timeout:等待接收完成的超时时间（以毫秒为单位）。
如果在这个时间内接收没有完成，函数将返回超时错误。
返回值
该函数返回一个 HAL_StatusTypeDef 类型的值，可以是以下几种之一：
HAL_OK: 函数执行成功。
HAL_ERROR: 执行过程中发生错误。
HAL_BUSY: UART 当前正忙于之前的操作。
HAL_TIMEOUT: 在操作完成前函数超时。
```

### 阻塞版uart收发数据实例应用：

驱动：uart_driver.c

```c
#include "uart_driver.h"


/*阻塞方式发送数据*/
int uart_send_by_block(void* buf,int size)
{
    /*调用hal库的串口发送函数*/
    return HAL_UART_Transmit(&huart1,buf,size,portMAX_DELAY);
}

/*阻塞方式接收数据*/
int uart_recv_by_block(void* buf,int maxSize)
{
    HAL_StatusTypeDef status;
    char * ptr = (char *)buf;
    int len = 0;
    while (1)
    {
        /* 接收数据 */
        status = HAL_UART_Receive(&huart1,(uint8_t *)ptr,1,portMAX_DELAY);
        if(*ptr == '\0' || *ptr == '\n' || *ptr == '\r')
        {
            break;
        }
        ptr++;
        len++;
        if(len > maxSize - 1)
        {
            break;
        }
    }
    return status;

}



```

应用程序：uart_app.c

```c
#include "uart_app.h"

void uart_app_function(void* arg)
{
    const char* str = "welcome uart_test,Please input CMD: \n";
    uart_send_by_block((void *)str,strlen(str));

    char buf[128] = {0};
    while (1)
    {
        /* 测试串口收发数据 */
        memset(buf,0,sizeof(buf));
        uart_recv_by_block(buf,sizeof(buf)-1);

        //发送给串口 在pc串口终端进行显示;
        uart_send_by_block(buf,strlen(buf));
    }
    


}

```

## 中断非阻塞版收发数据的接口函数:

### 中断非阻塞版本发送数据：

```c
HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
功能：用于通过 UART（通用异步收发传输器）以中断方式发送数据。
参数说明
UART_HandleTypeDef *huart:一个指向 UART_HandleTypeDef 结构体的指针，
该结构体包含指定 UART 模块的配置信息。这个结构体包括 UART 外设的实例，以及其配置和状态信息。
uint8_t *pData:一个指向数据缓冲区的指针，缓冲区中包含要发送的数据。
uint16_t Size:要发送的数据的字节数。
返回值
该函数返回一个 HAL_StatusTypeDef 类型的值，可以是以下几种之一：
HAL_OK: 函数执行成功。
HAL_ERROR: 执行过程中发生错误。
HAL_BUSY: UART 当前正忙于之前的操作。

该函数是非阻塞的，数据传输将在后台进行。
函数调用后立即返回，传输过程由中断驱动，传输完成后会产生一个完成中断。
需要在相应的 UART 中断服务程序（ISR）中处理传输完成事件。
通常在串口中断会调用 HAL_UART_IRQHandler 句柄。在这个句柄中回调处理完成的中断ISR:
在传输完成后会调用 HAL_UART_TxCpltCallback 回调函数，用户可以在该回调函数中执行后续处理。
```

### 重新实现HAL_UART_TxCpltCallback回调函数，处理完成后的逻辑：

```c
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) 
{    
// 传输完成后的处理代码
    if (huart->Instance == USART1)
     {        
         // UART1 传输完成后的处理
     }
}
```

### 发送Transmit_IT中断调用路径：

![image-20240731160211005](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240731160211005.png)

### 中断非阻塞版本接收数据：

```c
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
功能：用于通过 UART（通用异步收发传输器）以中断方式接收数据
参数说明
UART_HandleTypeDef *huart:一个指向 UART_HandleTypeDef 结构体的指针，该结构体包含指定 UART 模块的配置信息。这个结构体包括 UART 外设的实例，以及其配置和状态信息。
uint8_t *pData:一个指向数据缓冲区的指针，接收的数据将存储在这个缓冲区中。
uint16_t Size:要接收的数据的字节数。
返回值
该函数返回一个 HAL_StatusTypeDef 类型的值，可以是以下几种之一：
HAL_OK: 函数执行成功。
HAL_ERROR: 执行过程中发生错误。
HAL_BUSY: UART 当前正忙于之前的操作。
该函数是非阻塞的，传输过程由中断驱动，数据接收将在后台进行。
函数调用后立即返回，接收完成后会产生一个完成中断。
需要在相应的 UART 中断服务程序（ISR）中处理接收完成事件。通常在 ISR 中调用 HAL_UART_IRQHandler 函数来处理中断。
在接收完成后会调用 HAL_UART_RxCpltCallback 回调函数，用户可以在该回调函数中执行后续处理。
```

### 重新实现HAL_UART_RxCpltCallback回调函数，处理完成后的逻辑：

```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) 
{    
f    // 接收完成后的处理代码
    if (huart->Instance == USART1)
     {        
         // UART1 接收完成后的处理
     }
}
```

### 接收非阻塞中断的调用路径：

![image-20240731160415806](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240731160415806.png)

# DMA版收发数据接口函数：

## DMA简述：

由于很多外设的数据都由CPU进行拷贝，为了提高CPU实时性，进行更加重要的数据逻辑的处理，对于数据拷贝的工作，交于一个专门进行定向内存拷贝工作的Soc内部组件，就是DMA.

F103这个MCU有两个DMA控制器，DMA1有7个通道，DMA2有5个通道。每个通道用来处理来自一个或多个外设对存储器的访问。

**DMA的作用**：对于代替CPU执行一些简单的数据拷贝工作，拷贝完成之后产生一个中断通知，不需要CPU介入传输的具体过程。

## DMA框图



![image-20240802095951138](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240802095951138.png)

![image-20240802095956818](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240802095956818.png)

## DMA版串口收发数据：

### DMA版发送数据：

```c
HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
功能：用于通过 UART（通用异步收发传输器）以 DMA（直接存储器访问）方式发送数据
参数说明
UART_HandleTypeDef *huart:一个指向 UART_HandleTypeDef 结构体的指针，该结构体包含指定 UART 模块的配置信息。这个结构体包括 UART 外设的实例，以及其配置和状态信息。
uint8_t *pData:一个指向数据缓冲区的指针，缓冲区中包含要发送的数据。
uint16_t Size:要发送的数据的字节数。
返回值
该函数返回一个 HAL_StatusTypeDef 类型的值，可以是以下几种之一：
HAL_OK: 函数执行成功。
HAL_ERROR: 执行过程中发生错误。
HAL_BUSY: UART 当前正忙于之前的操作。
该函数是非阻塞的，数据传输将在后台进行。函数调用后立即返回，传输完成后会产生一个 DMA 中断。
需要在相应的 UART 和 DMA 中断服务程序（ISR）中处理传输完成事件。通常在 ISR 中调用 HAL_UART_IRQHandler 函数来处理中断。
在传输完成后会调用 HAL_UART_TxCpltCallback 回调函数，用户可以在该回调函数中执行后续处理。
与中断版一样。也可以处理完成之后的回调函数。
```

在传输完成后会调用 ：HAL_UART_TxCpltCallback 回调函数，用户可以在该回调函数中执行后续处理。

### DMA版接收数据：

```c
HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
功能：用于通过 UART（通用异步收发传输器）以 DMA（直接存储器访问）方式接收数据
参数说明
UART_HandleTypeDef *huart
:一个指向 UART_HandleTypeDef 结构体的指针，该结构体包含指定 UART 模块的配置信息。这个结构体包括 UART 外设的实例，以及其配置和状态信息。
uint8_t *pData
:一个指向数据缓冲区的指针，接收的数据将存储在这个缓冲区中。
uint16_t Size
:要接收的数据的字节数。
返回值
该函数返回一个 HAL_StatusTypeDef 类型的值，可以是以下几种之一：
HAL_OK: 函数执行成功。
HAL_ERROR: 执行过程中发生错误。
HAL_BUSY: UART 当前正忙于之前的操作。
该函数是非阻塞的，数据接收将在后台进行。函数调用后立即返回，接收完成后会产生一个 DMA 中断。
需要在相应的 UART 和 DMA 中断服务程序（ISR）中处理接收完成事件。
通常在 ISR 中调用 HAL_UART_IRQHandler 函数来处理中断。
```

### 调用路径：以发送为例：

![image-20240802100222310](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240802100222310.png)

# uart总结：

![image-20240802102956851](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240802102956851.png)

# uart的Printf实现：

![image-20240802102854504](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240802102854504.png)

```c
工程编译使用MicroLib库：重新实现fputc函数即可。用于后续课程中的DeBug调试。
//重写putchar：
int fputc(int c, FILE* file)
{
        HAL_UART_Transmit(&huart1, (uint8_t*)&c, 1, portMAX_DELAY);
        return c;
}
```

# I2C总线接口以及设备驱动

## I2C通信原理以及时序

IIC通信协议用一句话来描述就是：**主机呼叫从机，从机接受并反馈信号，传输数据后关闭传输。**

### I2C总线概述**(Inter-Integrated Circuit)**

由于早期使用uart通信时，当进行多设备通信时，连接过于繁琐复杂，对于近距设备间通信，成本过高，所以在1982年由飞利浦公司开发出了一种**基于总线的多设备通讯方式**:I²C通信，总线由简洁的**SCL时钟线****与****SDA数据线****组成**，多个设备都挂载到这两根上。I²C协议的设计初衷是通过减少连接线数量和简化硬件接口，实现低成本、高效的**近距离设备通信**。该协议允许多个主设备和从设备共享同一总线，使得系统设计更加简便和灵活。是**低速设备通信的常用总线**。

![image-20240807142830609](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240807142830609.png)

## I2C总线通信协议

下面介绍一下：使用从机E2PROM从机为例，通过时序简要介绍一下IIC通信协议：

![image-20240807142902578](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240807142902578.png)

#### 写数据帧：

![image-20240807142921345](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240807142921345.png)

#### 读数据帧：

![image-20240807142940125](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240807142940125.png)

![image-20240807142944546](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240807142944546.png)

### 时序图：

![image-20240807142952984](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240807142952984.png)

### 控制器框图：

![image-20240807143010261](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240807143010261.png)

## I2C接口通信接口函数分类：

## HAL关于I2C操作模式介绍：

HAL库把对不同的i2c接口设备使用两种类型的接口，分别为Master主模与Memory模式：

### Master模式

在I2C的Master模式下，STM32微控制器作为主设备，与从设备进行通信。主设备负责生成时钟信号并启动通信。HAL库中的I2C Master模式函数主要用于发送和接收数据。

**主要函数包括：**

- **HAL_I2C_Master_Transmit(): 主设备发送数据给从设备。**
- **HAL_I2C_Master_Receive(): 主设备从从设备接收数据。**
- **HAL_I2C_Master_Transmit_IT(): 主设备以中断方式发送数据。**
- **HAL_I2C_Master_Receive_IT(): 主设备以中断方式接收数据。**
- **HAL_I2C_Master_Transmit_DMA(): 主设备以DMA方式发送数据。**
- **HAL_I2C_Master_Receive_DMA(): 主设备以DMA方式接收数据。**

### Memory模式（MeM模式）

在Memory模式下，STM32微控制器作为主设备，访问从设备的内存。这种模式通常用于读取或写入I2C EEPROM或其他具有内部寄存器的设备。

**主要函数包括：**

- **HAL_I2C_Mem_Write(): 向从设备的指定内存地址写入数据。**
- **HAL_I2C_Mem_Read(): 从从设备的指定内存地址读取数据。**
- **HAL_I2C_Mem_Write_IT(): 以中断方式向从设备的指定内存地址写入数据。**
- **HAL_I2C_Mem_Read_IT(): 以中断方式从从设备的指定内存地址读取数据。**
- **HAL_I2C_Mem_Write_DMA(): 以DMA方式向从设备的指定内存地址写入数据。**
- **HAL_I2C_Mem_Read_DMA(): 以DMA方式从从设备的指定内存地址读取数据。**

### I2C总线接口的从机设备的I2C设备地址，以及相应的操作命令码：

陀螺仪加速度计：I2C地址bit:0x68/0x69用于设置不同的I2C设备地址。默认是0x68

![image-20240807143528165](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240807143528165.png)

## 阻塞模式I2C接口函数API介绍：

### Master模式：

**HAL_I2C_Master_Transmit(): 主设备发送数据给从设备。**

```c
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, 
                                            uint8_t *pData, uint16_t Size, uint32_t Timeout);
参数说明
I2C_HandleTypeDef *hi2c: I2C 句柄，包含了 I2C 配置信息。这个句柄是在初始化 I2C 外设时创建的。
uint16_t DevAddress: 从设备的 7 位或 10 位地址。该地址是左对齐的，即地址的最低有效位（LSB）应该是 0。
uint8_t *pData: 指向需要传输的数据缓冲区的指针。
uint16_t Size: 要传输的数据大小（字节数）。
uint32_t Timeout: 指定超时时间（毫秒）。如果在指定时间内传输未完成，则函数返回超时状态。
返回值
HAL_I2C_Master_Transmit 函数的返回值为 HAL_StatusTypeDef 类型，表示函数的执行状态。可能的返回值包括：
HAL_OK: 表示函数执行成功，数据传输完成。
HAL_ERROR: 表示发生错误。
HAL_BUSY: 表示 I2C 外设正忙，无法执行该操作。
HAL_TIMEOUT: 表示操作超时。  
```

**HAL_I2C_Master_Receive(): 主设备从从设备接收数据。**

```C
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, 
                                            uint16_t Size, uint32_t Timeout);
参数说明
I2C_HandleTypeDef *hi2c: I2C 句柄，包含了 I2C 配置信息。这个句柄是在初始化 I2C 外设时创建的。
uint16_t DevAddress: 从设备的 7 位或 10 位地址。该地址是左对齐的，即地址的最低有效位（LSB）应该是 0。
uint8_t *pData: 指向存储接收数据的缓冲区的指针。
uint16_t Size: 要接收的数据大小（字节数）。
uint32_t Timeout: 指定超时时间（毫秒）。如果在指定时间内接收未完成，则函数返回超时状态。
返回值
HAL_I2C_Master_Receive 函数的返回值为 HAL_StatusTypeDef 类型，表示函数的执行状态。可能的返回值包括：
HAL_OK: 表示函数执行成功，数据接收完成。
HAL_ERROR: 表示发生错误。
HAL_BUSY: 表示 I2C 外设正忙，无法执行该操作。
HAL_TIMEOUT: 表示操作超时。
```

### **Memory模式**:

**HAL_I2C_Mem_Write(): 向从设备的指定内存地址写入数据。**

```C
HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, 
                        uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
参数说明
I2C_HandleTypeDef *hi2c：I2C 句柄，包含了 I2C 配置信息。这个句柄是在初始化 I2C 外设时创建的。
uint16_t DevAddress：从设备的地址（7 位或 10 位）。
uint16_t MemAddress：从设备内存的地址，从这个地址开始写入数据。
uint16_t MemAddSize：内存地址的大小，可以是 I2C_MEMADD_SIZE_8BIT 或 I2C_MEMADD_SIZE_16BIT。
uint8_t *pData：指向要写入数据的缓冲区的指针。
uint16_t Size：要写入的数据大小（字节数）。
uint32_t Timeout：指定超时时间（毫秒）。如果在指定时间内操作未完成，则函数返回超时状态。
返回值
HAL_I2C_Mem_Write 函数的返回值为 HAL_StatusTypeDef 类型，表示函数的执行状态。可能的返回值包括：
HAL_OK：表示函数执行成功，数据写入完成。
HAL_ERROR：表示发生错误。
HAL_BUSY：表示 I2C 外设正忙，无法执行该操作。
HAL_TIMEOUT：表示操作超时。
```

**HAL_I2C_Mem_Read(): 从从设备的指定内存地址读取数据。**

```C
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, 
                        uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
参数说明
I2C_HandleTypeDef *hi2c：I2C 句柄，包含了 I2C 配置信息。这个句柄是在初始化 I2C 外设时创建的。
uint16_t DevAddress：从设备的地址（7 位或 10 位）。
uint16_t MemAddress：从设备内存的地址，从这个地址开始读取数据。
uint16_t MemAddSize：内存地址的大小，可以是 I2C_MEMADD_SIZE_8BIT 或 I2C_MEMADD_SIZE_16BIT。
uint8_t *pData：指向存储读取数据的缓冲区的指针。
uint16_t Size：要读取的数据大小（字节数）。
uint32_t Timeout：指定超时时间（毫秒）。如果在指定时间内操作未完成，则函数返回超时状态。
返回值
HAL_I2C_Mem_Read 函数的返回值为 HAL_StatusTypeDef 类型，表示函数的执行状态。可能的返回值包括：
HAL_OK：表示函数执行成功，数据读取完成。
HAL_ERROR：表示发生错误。
HAL_BUSY：表示 I2C 外设正忙，无法执行该操作。
HAL_TIMEOUT：表示操作超时。
```

# MPU6050陀螺仪芯片驱动开发流程：

## 读手册了解芯片配置：初始化配置

![image-20240807224230592](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240807224230592.png)

## MPU6050陀螺仪驱动代码以及获取数据的接口函数：

```c
// mpu6050的初始化函数
int mpu6050_init(struct i2c_mpu6050 *i2c_dev)
{
    HAL_StatusTypeDef status;

    // 使用HAL库的memory方式
    uint8_t write_data = 0x0;
    // 对电源寄存器配置为唤醒
    status = HAL_I2C_Mem_Write(i2c_dev->i2c_adpter, i2c_dev->i2c_addr, i2c_dev->pow1_reg, 1, &write_data, 1, portMAX_DELAY);
    status = HAL_I2C_Mem_Write(i2c_dev->i2c_adpter, i2c_dev->i2c_addr, i2c_dev->pow2_reg, 1, &write_data, 1, portMAX_DELAY);

    // 配置采样
    write_data = 0x7;
    status = HAL_I2C_Mem_Write(i2c_dev->i2c_adpter, i2c_dev->i2c_addr, i2c_dev->smprt_reg, 1, &write_data, 1, portMAX_DELAY);

    // 配置低通滤波
    write_data = 0x6;
    status = HAL_I2C_Mem_Write(i2c_dev->i2c_adpter, i2c_dev->i2c_addr, i2c_dev->dlpf_config_reg, 1, &write_data, 1, portMAX_DELAY);

    // 配置每秒检测的角度范围
    write_data = 0x18;
    status = HAL_I2C_Mem_Write(i2c_dev->i2c_adpter, i2c_dev->i2c_addr, i2c_dev->gyro_config_reg, 1, &write_data, 1, portMAX_DELAY);

    // 配置加速度范围+-4G
    write_data = 0x8;
    status = HAL_I2C_Mem_Write(i2c_dev->i2c_adpter, i2c_dev->i2c_addr, i2c_dev->accel_config_reg, 1, &write_data, 1, portMAX_DELAY);

    return status;
}

// 1.获取mpu6050的温度
int getTemp_value(struct i2c_mpu6050 *i2c_dev)
{
    HAL_StatusTypeDef status;
    uint8_t buf[2] = {0};
    status = HAL_I2C_Mem_Read(i2c_dev->i2c_adpter, i2c_dev->i2c_addr, i2c_dev->temp_reg, 1, buf, 2, portMAX_DELAY);

    //大端转成小端
    uint16_t tmp_data = buf[0] << 8 | buf[1];
    i2c_dev->data = tmp_data;

    return status;
}
```

测试：

```c
    mpu6050_init(&mpu6050);
    while (1)
    {
        getTemp_value(&mpu6050);
        int16_t temp_value = mpu6050.data;
        vTaskDelay(1000);
        printf("temp_value = %.2f\n",temp_value / 340.0 + 36.53);

    }
```

## 编写获取角速度值得驱动接口：

```c
//2.获取mpu6050上的GYRO_x,y,z三轴上的偏转角速度
int getGYRO_xyz_value(struct i2c_mpu6050* i2c_dev,float* GYRO_x,float* GYRO_y,float* GYRO_z)
{
    HAL_StatusTypeDef status;
    uint8_t buf[2] = {0};
    if(GYRO_x)
    {
    status = HAL_I2C_Mem_Read(i2c_dev->i2c_adpter, i2c_dev->i2c_addr, i2c_dev->GYRO_x_reg, 1, buf, 2, portMAX_DELAY);

    //大端转成小端
    int16_t tmp_data = buf[0] << 8 | buf[1];
    *GYRO_x = tmp_data / 16.4 - 1.28;
    }

    if(GYRO_y)
    {
    status = HAL_I2C_Mem_Read(i2c_dev->i2c_adpter, i2c_dev->i2c_addr, i2c_dev->GYRO_y_reg, 1, buf, 2, portMAX_DELAY);

    //大端转成小端
    int16_t tmp_data = buf[0] << 8 | buf[1];
    *GYRO_y = tmp_data / 16.4 - 0.12;
    }

    if(GYRO_z)
    {
    status = HAL_I2C_Mem_Read(i2c_dev->i2c_adpter, i2c_dev->i2c_addr, i2c_dev->GYRO_z_reg, 1, buf, 2, portMAX_DELAY);

    //大端转成小端
    int16_t tmp_data = buf[0] << 8 | buf[1];
    *GYRO_z = tmp_data / 16.4 + 0.91;
    }

    return status;
}

```

测试：

```c
        getGYRO_xyz_value(&mpu6050,&x,&y,&z);
        printf("GYRO_x = %.2f\nGYRO_y = %.2f\nGYRO_z = %.2f \n",x,y,z);
```

## 编写获取加速度值得驱动函数:

```c
int getACC_xyz_value(struct i2c_mpu6050* i2c_dev,float* ACC_x,float* ACC_y,float* ACC_z)
{
    HAL_StatusTypeDef status;
    uint8_t buf[2] = {0};
    if (ACC_x)
    {
        status = HAL_I2C_Mem_Read(i2c_dev->i2c_adpter, i2c_dev->i2c_addr, i2c_dev->ACC_x_reg, 1, buf, 2, portMAX_DELAY);

        // 大端转成小端
        int16_t tmp_data = buf[0] << 8 | buf[1];
        *ACC_x = tmp_data / 8192.0;
    }

    if (ACC_y)
    {
        status = HAL_I2C_Mem_Read(i2c_dev->i2c_adpter, i2c_dev->i2c_addr, i2c_dev->ACC_y_reg, 1, buf, 2, portMAX_DELAY);

        // 大端转成小端
        int16_t tmp_data = buf[0] << 8 | buf[1];
        *ACC_y = tmp_data / 8192.0;
    }

    if (ACC_z)
    {
        status = HAL_I2C_Mem_Read(i2c_dev->i2c_adpter, i2c_dev->i2c_addr, i2c_dev->ACC_z_reg, 1, buf, 2, portMAX_DELAY);

        // 大端转成小端
        int16_t tmp_data = buf[0] << 8 | buf[1];
        *ACC_z = tmp_data / 8192.0;
    }

    return status;
}
```

以上接口均为无人机飞控，以及平衡车必用接口！！！！！

## 融合角速度与加速度，获取趋近实际得roll pich yaw角度：（无技术点，纯数学算法）

```c
// 4.获取mpu6050上的角度:roll_x pitch_y,yaw_z
int getRoll_Pitch_Yaw_value(struct i2c_mpu6050 *i2c_dev, float *Roll_x, float *Pitch_y, float *Yaw_z)
{
    float gx, gy, gz, ax, ay, az;
    gx = gy = gz = ax = ay = az;
    // 获取角速度
    getGYRO_xyz_value(i2c_dev, &gx, &gy, &gz);
    // 获取加速度
    getACC_xyz_value(i2c_dev, &ax, &ay, &az);

    // 计算一下：使用加速度原始值获取pitch与roll
    float ACC_Pitch = atan2(ax, az) * 180 / PI;
    float ACC_Roll = -atan2(ay, az) * 180 / PI;

    // 通过角速度计算Pitch roll yaw
    float GYRO_Pitch = gy * Dt;
    float GYRO_Roll = gx * Dt;
    float GYRO_Yaw = gz * Dt;

    // 互补滤波算法：(实际应用不推荐)
    *Roll_x = ALPHA * (*Roll_x + GYRO_Roll) + (1 - ALPHA) * ACC_Roll;
    *Pitch_y = ALPHA * (*Roll_x + GYRO_Pitch) + (1 - ALPHA) * ACC_Pitch;
    *Yaw_z = *Yaw_z + GYRO_Yaw * 180 / PI;


    return 0;
}


```

# 非阻塞中断与DMA方式

## Master模式

**HAL_I2C_Master_Transmit_IT(): 主设备发送数据给从设备。**

**HAL_I2C_Master_Transmit_DMA(): 主设备发送数据给从设备。**

**中断完成时的回调函数**

```c
1.中断的方式发送：
HAL_StatusTypeDef HAL_I2C_Master_Transmit_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
uint8_t *pData, uint16_t Size);

2.DMA的方式发送：
HAL_StatusTypeDef HAL_I2C_Master_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
uint8_t *pData, uint16_t Size);

/* 主设备发送完成回调函数 */
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c);
```

**HAL_I2C_Master_Receive_IT(): 主设备从从设备接收数据。**

**HAL_I2C_Master_Receive_DMA(): 主设备从从设备接收数据。**

**中断完成时的回调函数：**

```c
1.中断的方式接收：
HAL_StatusTypeDef HAL_I2C_Master_Receive_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
uint8_t *pData, uint16_t Size);

2.DMA的方式接收：
HAL_StatusTypeDef HAL_I2C_Master_Receive_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
uint8_t *pData, uint16_t Size);

/* 主设备接收完成回调函数 */
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c);
```

## Memory模式

**HAL_I2C_Mem_Write_IT(): 向从设备的指定内存地址写入数据。**

**HAL_I2C_Mem_Write_DMA(): 向从设备的指定内存地址写入数据。**

**中断完成时的回调函数：**

```C
1.Mem模式中断发送：
HAL_StatusTypeDef HAL_I2C_Mem_Write_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);

2.Mem模式DMA发送：
HAL_StatusTypeDef HAL_I2C_Mem_Write_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);

/* MEM模式发送完成回调函数 */
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c);
```

**HAL_I2C_Mem_Read_IT(): 从从设备的指定内存地址读取数据。**

**HAL_I2C_Mem_Read_DMA(): 从从设备的指定内存地址读取数据。**

**中断完成时的回调函数**

```c
1.Mem模式中断接收：
HAL_StatusTypeDef HAL_I2C_Mem_Read_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t
MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);


2.MeM模式DMA接收：
HAL_StatusTypeDef HAL_I2C_Mem_Read_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress,
uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);


/* MEM模式接收完成回调函数 */
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c);
```

# OLED显示屏原理以及驱动的移植

## 了解移植SSD1306厂家的显示驱动

OLED显示的坐标：

![image-20240809151230099](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240809151230099.png)

## 显示汉字：

![image-20240809151247939](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240809151247939.png)

取模网站：[LCD/OLED汉字字模提取软件,(HZK12宋体)GB2312中文12*12点阵字库 (23bei.com)](https://www.23bei.com/tool/223.html)

OLED_PrintChinaTxt():

```C
// 编写OLED显示汉字的驱动：
void OLED_PrintChinaTxt(uint16_t x, uint16_t y, char *buf, uint16_t ChinaTxtSize)
{
    // 把坐标设定为8*8
    uint16_t page = y;
    uint16_t col = x * 8;
    if (x > 15 || y > 7)
        return;

    for (int i = 0; i < ChinaTxtSize; i++)
    {
        uint16_t index = i * 32;
        OLED_SetPosition(page, col);
        OLED_WriteNBytes((uint8_t *)&buf[index], 16);

        OLED_SetPosition(page + 1, col);
        OLED_WriteNBytes((uint8_t *)&buf[index + 16], 16);

        col += 16;
    }
}
```

# SPI通信原理与相关概念

## SPI总线的特点

SPI（serial peripheral interface）串行外设接口的缩写，它是Motorola公司推出的一种同步串行接口技术，是一种高速的，全双工的，同步的通信总线。

SPI是支持全双工通信， 通信简单，数据传输速率快，非差分的，一主多从的通信模式。

所谓为非差分与差分的概念是指在远距离传输时，是否通过比较两个根的电平的差来判断是传据是高还是低，一般远距离传输都是使用差分总线，近距离的都是非差分总线。SPI是非差分总线，R485就是一种差分总线。了解即可。

缺点：无应答机制确认是否接收到数据，所以与I2C总线协议比较在数据的可靠性上有一点差距，但是速度快啊。

与I2c相比：

![image-20240902101506746](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240902101506746.png)

## 三根共享线**SCK,MOSI,MISO,及1个或多个片选线：**

### 常规SPI模式

![image-20240902101609521](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240902101609521.png)

### 一个片选的菊花链模式

![image-20240902101633026](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240902101633026.png)



## SPI数据传输方式：

SPI 主设备和从设备都有一个串行移位寄存器，主设备通过向它的 SPI 串行寄存器写入一个字节来发起一次传输。

![image-20240902111949354](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240902111949354.png)

SPI控制器硬件框图：

![image-20240902111958067](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240902111958067.png)

## SPI采样模式：由CPOL(时钟极性)与CPHA(时钟相位)

![image-20240902112052554](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240902112052554.png)

CPOL决定空闲时的电平的高低，CPHA决定采样的时机 

当：CPOL(时钟极性) = 0 时，即空闲时电平为低，第一个边沿意味为由低到高的上升边沿。

当：CPOL(时钟极性) = 1时，即空闲时电平为高，第一个边沿意味为由高到低的下降边沿。

当：CPHA(时钟相位) = 0时，即第一个边沿采样，下一个边沿传输。

当：CPHA(时钟相位) = 1时，即第一个边沿传输，下一个边沿采样。

所以就组合了四种模式：Mode0(CPOL=0, CPHA = 0), Model1(CPOL=0, CPHA = 1), Model2(CPOL=1,CPHA=0), Model3(CPOL=1,CPHA=1);

比如：模式0时序：

![image-20240902112101283](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240902112101283.png)



## SPI总线接口驱动HAL库API:

**要点：1.配置时钟的波特率，2.配置采样模式，3.配置传输高位优先还是低位优先，4.配置CS一般使用一个单独的GPIO作为选择。5.使用SPI功能。**

![image-20240902115511465](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240902115511465.png)

![image-20240902115515050](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240902115515050.png)

### 阻塞方式：

![image-20240902115534645](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240902115534645.png)

### 非阻塞方式：

![image-20240902115605429](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240902115605429.png)

![image-20240902115614366](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240902115614366.png)

![image-20240902115619874](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240902115619874.png)

### DMA方式：

![image-20240902115645778](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240902115645778.png)

![image-20240902115652479](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240902115652479.png)

![image-20240902115657727](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240902115657727.png)

![image-20240902115702580](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240902115702580.png)

# 定时器功能之PWM功能简述

## PWM简述

**PWM（脉冲宽度调制）是一种用于产生模拟信号的技术，通常用于控制电子设备中的电机、灯光、声音等。PWM通过调整脉冲的宽度，以实现对输出信号的调节。**

**PWM是一种通过高低电平产生的梯度高低的方波信号。**

**频率：(Hz)单位，即1秒内产生的方波的个数。**

**周期：（s）秒，即1Hz所占用的时间。**

![image-20240903111432747](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240903111432747.png)

**占空间比：即高电压在整个周期的比值**

**基本上，PWM是通过在****一个周期内改变信号的高电平（有效电平）时间（占空比）来控制输出的。占空比是高电平时间与一个完整周期的比例。通过调整占空比，可以改变输出信号的平均电压，从而实现对设备的功率的控制。

**在嵌入式系统和电子控制领域，PWM广泛应用于各种场景，包括但不限于：**

**电机控制： 通过PWM控制电机的速度和方向。**

**LED亮度控制： 调整LED的亮度。**

**音频控制： 生成模拟音频信号。**

**电源管理： 在电源管理中，PWM可用于调整电压。**

**PWM接口通常由硬件电路实现，而在软件层面，操作系统或驱动程序提供相应的接口来配置和控制PWM。在嵌入式系统中，微控制器、FPGA或其他嵌入式处理器通常集成了PWM功能。**

高级定时器框图：

![image-20240903111509881](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240903111509881.png)

## **PSC分频器（Prescaler）:**

作用：psc分频器用于将定时器的输入时钟降低到合适的频率，分频器的值决定了定时器的计数的步进速度：

工作原理：假设输入的时钟频率为200MHZ，分频值为200，那么分频后的时钟频率就是1MHZ

设置PSC寄存器的值，可以控制定时器计数的速度。



## **自动重载寄器（ARR，Auto-Reload Register）**

作用：ARR决定了定时器计数的最大值。每当计数器达到这个值时，**计数器（CNT）**会自动重置为0，并重新开始计数。

工作原理：定时器从0计数到ARR的值，然后重新开始计数，这样形成一个周期性计数过程。ARR的值决定了PWM信号的周期。

设置ARR寄存器的值，可以控制PWM信号的周期。



## **计数器（CNT，Counter）**

作用：计数器是定时器的核心器件，它按设定的时钟频率计数。

工作原理：计数器的值从0开始，每经过一个定时器的时钟周期（PSC分频器决定），计数器的值加+1.当计数器达到ARR的值时，**计数据重置为0，并触发中断或一个更新事件。**

计数过程：计数器不断地从0计数到ARR值，再从0开始重新计数，这个循环决定了PWM信号的周期性。



## **捕获/比较寄存器（CCR，Capture/Compare Register）**

作用：CCR用于设置PWM信号的点空比，它存储一个值，当计数器的值与CCR的值匹配时，定时器输出信号的电平会发生变化。

工作原理：在PWM模式下，**当计数器的值小于CCR时，输出为高电平**;  **当计数器的值等于或大于CCR的值时，输出信号变低电平。**

**设置CCR寄存器的值，可以控制PWM信号的高电平持续的时间，从而调速占空比。如图所示：**

![image-20240903111612648](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240903111612648.png)

## 常用HAL库定时器PWM功能输出接口函数：

### 配置定时器：**HAL_TIM_PWM_ConfigChannel**

```c
HAL_StatusTypeDef HAL_TIM_PWM_ConfigChannel(TIM_HandleTypeDef *htim,
const TIM_OC_InitTypeDef *sConfig,
uint32_t Channel);
功能：配置某个定时器占空时间及输出通道
参数：
htim：指定要配置的定时器对象的指针
Channel：哪个通道，取值有TIM_CHANNEL_1到TIM_CHANNEL_4，TIM_CHANNEL_ALL
sConfig：要进行配置的具体参数的结构体：
typedef struct
{
    uint32_t OCMode; /* 模式，通常设置为 TIM_OCMODE_PWM1 即计数值小于CRR值时输出有效电平*/
        TIM_OCMODE_TIMING         记数
        TIM_OCMODE_ACTIVE         当CNT == CRR时 输出有效电平
        TIM_OCMODE_INACTIVE       当CNT == CRR时 输出无效电压
        TIM_OCMODE_TOGGLE         当CNT == CRR时输出电压翻转一次
        TIM_OCMODE_PWM1           CNT < CRR时输出有效电平（此为PWM常用模式）
        TIM_OCMODE_PWM2           CNT > CRR时输出有效电平
        TIM_OCMODE_FORCED_ACTIVE  强制输出无效电平
        TIM_OCMODE_FORCED_INACTIVE 强制输出有效电平
    uint32_t Pulse; /* 有效电平的宽度，单位为时钟个数，此为占空时间 */
    uint32_t OCPolarity; /* 输出信号有效电平是高还是低，TIM_OCPOLARITY_HIGH或
TIM_OCPOLARITY_LOW */
    uint32_t OCNPolarity; /* 互补输出信号有效电平是高还是低，TIM_OCPOLARITY_HIGH或
TIM_OCPOLARITY_LOW */
    uint32_t OCFastMode; /* 是否使用快速模式，通常设置为 TIM_OCFAST_DISABLE即下一次更新，如果使能即马上更新 */
    uint32_t OCIdleState; /* 空闲是引脚的输出电平 */
    uint32_t OCNIdleState; /* 空闲时互补引脚的输出电平 */
} TIM_OC_InitTypeDef;
返回值
HAL_StatusTypeDef:
这个函数返回 HAL 库定义的状态码，表示函数执行的结果：
HAL_OK: 表示配置成功。
HAL_ERROR: 表示发生错误。
HAL_BUSY: 表示定时器正忙，无法执行配置。
```

### 启动定时器：**HAL_TIM_PWM_Start**

```c
HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
功能：启动定时器 PWM 输出的函数。它使能指定定时器通道的 PWM 信号输出。
参数：
htim:指向定时器句柄的指针。这个句柄包含了定时器的配置信息和状态信息，通常由初始化函数（如 HAL_TIM_Base_Init 或 HAL_TIM_PWM_Init）生成。
uint32_t Channel:指定要启动的定时器通道。可能的值包括：
    TIM_CHANNEL_1
    TIM_CHANNEL_2
    TIM_CHANNEL_3
    TIM_CHANNEL_4
    对于高级定时器还会其它的通道。详情看手册。
返回值：
    HAL_StatusTypeDef:
    HAL_OK: 操作成功。
    HAL_ERROR: 操作失败。
    HAL_BUSY: 定时器正忙，无法启动 PWM。
```

### **停止定时器：HAL_TIM_PWM_Stop**

```c
HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);
功能：停止定时器 PWM 输出的函数。它会停止指定定时器通道的 PWM 信号输出，并且关闭相关的输出通道。如果想重新启动pwm，可以再次调用HAL_TIM_PWM_Start函数。
参数：
参数说明
htim:指向定时器句柄的指针。这个句柄包含了定时器的配置信息和状态信息，通常由初始化函数（如 HAL_TIM_Base_Init 或 HAL_TIM_PWM_Init）生成。
Channel:指定要停止的定时器通道。可能的值包括：
    TIM_CHANNEL_1
    TIM_CHANNEL_2
    TIM_CHANNEL_3
    TIM_CHANNEL_4
返回值：
HAL_StatusTypeDef:
HAL_OK: 操作成功。
HAL_ERROR: 操作失败。
HAL_BUSY: 定时器正忙，无法停止 PWM。
```

# 定时器基本定时器功能之：高精度定时（微妙级）

我们常用的延时函数中无论是HAL_Delay还是vTaskDelay()函数都是毫秒级的定时，我们可以借助定时器实现一个微秒级更高精度的延时函数。

![image-20240906203346067](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240906203346067.png)

## 基本定时器的功能API:

```c
__HAL_TIM_GetCounter(TIM_HandleTypeDef *htim);
//功能：获取htim定时器的计数器CNT的值。
//返回值：CNT的当前计数值

__HAL_TIM_GetAutoreload(TIM_HandleTypeDef *htim);
//功能：获取htim定时器的ARR自动装载寄存器的值。
//返回值：htim的ARR的值。
其它的获取寄存器中值的宏函数可以自行百度或看官方HAL库关于定时器方面的手册。
```

## 实现微妙级定时功能的思路：

**前提：**t_old原计数值 与 t_now当下计数值之间不可以超过一个溢出周期的前提下。

因为原计数与当下计数值之间在不存在延时操作的情况下，不可能出现超过一个溢出周期的。**所以思路如下：**

![image-20240906203451673](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240906203451673.png)

## 高精度延时驱动（微秒级）：

```c
#include "tim2.h"
#include "tim.h"
#include <stdbool.h>
//实现更高精度的延时函数（微秒级）
void tim2_u_delay(uint32_t us)
{
    //1.获取ARR自动装载寄存器中的值：
    uint32_t load_ARR = __HAL_TIM_GET_AUTORELOAD(&htim2);
    //2.获取一个起始检测点的计数值：
    uint32_t t_start_CNT = __HAL_TIM_GET_COUNTER(&htim2);
    //3.检测周期：
    uint32_t t_current_CNT = 0;
    uint32_t ticks = 0;
    while (true)
    {
        /* 检测 */
        t_current_CNT = __HAL_TIM_GET_COUNTER(&htim2);
        if(t_current_CNT >= t_start_CNT)
        {
            ticks += (t_current_CNT - t_start_CNT);
        }else{
            ticks += (load_ARR) - t_start_CNT + 1 + t_current_CNT;
        }
        //更新起始点start:
        t_start_CNT = t_current_CNT;
        if(ticks >= us)
        {
            break;
        }
    }
    
}
```

关于计数器达到自动装载寄存器值后发生的溢出中断的回调函数，是一个__weak虚函数：HAL_TIM_PeriodElapsedCallback()大家根据需要自行实现其业务实现即可:注意，使用此中断回调需要在start时用start_IT函数来启动中断功能。在cubeMX中也要配置NVIC的中断功能才可以使用。

```c
__weak void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
//功能：计数器达到ARR寄存器值后的溢出中断处理函数。
//此函数为弱函数，定义可由业务需要自行实现。
```

# 定时器输入功能之：输入捕获功能

## 输入捕获原理与作用：

定时器的输入捕获（Input Capture）功能是 STM32 等微控制器中的一种非常重要的功能，主要用于测量外部信号的时间特性，如周期、脉宽、频率等。以下是对输入捕获功能的详细介绍：

### 基本概念：

输入捕获功能允许定时器捕获外部输入信号的边沿，并将捕获时的定时器计数器值（CNT）存储在指定的捕获/比较寄存器中（例如，CCR1、CCR2）。这使得微控制器可以精确地测量外部信号的时间特性。

### 工作原理：

- 输入通道：定时器通常有多个输入捕获通道（例如，TIMx_CH1, TIMx_CH2），每个通道可以连接到不同的外部引脚。
- 边沿选择：可以选择捕获信号的上升沿、下降沿，或两者都捕获。捕获发生时，定时器会将当前计数值（CNT）锁存在相应的捕获/比较寄存器（CCR）中，并生成一个捕获事件（CCxIF中断标志）。
- 捕获计数器值：在捕获事件发生时，定时器计数器的值会被锁存到对应的捕获寄存器中。例如，如果通道1捕获事件发生，则当前定时器计数值会被存储到CCR1寄存器中。

![image-20240907160549855](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240907160549855.png)

![image-20240907160556022](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240907160556022.png)

### 典型应用：

- 脉宽测量：通过捕获信号的上升沿和下降沿，可以测量信号的高电平或低电平持续时间。
- 周期测量：通过两次上升沿或下降沿之间的时间差，可以测量信号的周期。
- 频率测量：通过周期测量，可以计算出信号的频率。

## 输入捕获HAL库接口函数API:

#### 启动输入捕获：HAL_TIM_IC_Start

```c
HAL_StatusTypeDef HAL_TIM_IC_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
1功能：启动定时器通道的输入捕获功能：
2功能：启动定时器通道的输入捕获功能并且开启捕获中断。
参数：
htim:即要启动的定时器的句柄。
Channel：启动输入捕获的通道是哪个。

返回值：
    HAL_OK: 操作成功。
    HAL_ERROR: 操作失败。
    HAL_BUSY: 定时器资源忙
```

#### 停止输入捕获：HAL_TIM_IC_Stop

```c
HAL_StatusTypeDef HAL_TIM_IC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
1.功能：停止定时器通道的输入捕获功能：
2功能：停止定时器通道的输入捕获功能并且关闭捕获中断。
参数：
htim:即要停止的定时器的句柄。
Channel：停止输入捕获的通道是哪个。
返回值：
    HAL_OK: 操作成功。
    HAL_ERROR: 操作失败。
    HAL_BUSY: 定时器资源忙
```

#### **输入捕获配置通道:HAL_TIM_IC_ConfigChannel，相应的输入参数也可直接在CubMX直接配置。**

```c
HAL_StatusTypeDef HAL_TIM_IC_ConfigChannel(TIM_HandleTypeDef *htim, TIM_IC_InitTypeDef *sConfig, uint32_t Channel);
功能：配置输入捕获通道
参数：
htim: 指向 TIM_HandleTypeDef 结构体的指针，该结构体包含了定时器的配置信息。
sConfig: 指向 TIM_IC_InitTypeDef 结构体的指针，该结构体用于配置输入捕获通道的参数。
Channel: 要配置的定时器通道。可以是 TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3, 或 TIM_CHANNEL_4。

TIM_IC_InitTypeDef 结构体用于设置输入捕获通道的参数，主要包括：
ICPolarity: 输入捕获极性。
    可以是 TIM_ICPOLARITY_RISING（上升沿）或 TIM_ICPOLARITY_FALLING（下降沿），或者 TIM_ICPOLARITY_BOTHEDGE（两边缘）。
ICSelection: 输入捕获选择。
    可以是 TIM_ICSELECTION_DIRECTTI（直接输入）或 TIM_ICSELECTION_INDIRECTTI（间接输入）。
ICPrescaler: 输入捕获预分频器。
    可以是 TIM_ICPSC_DIV1（无分频），TIM_ICPSC_DIV2（除2），TIM_ICPSC_DIV4（除4），或 TIM_ICPSC_DIV8（除8）。
ICFilter: 输入捕获滤波器值，用于过滤输入信号中的噪声。
返回值
    HAL_OK: 配置成功。
    HAL_ERROR: 配置失败。
```

当捕获成功之后的中断回调函数：

```c
__weak void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
//此函数是捕获之后的中断回调函数，是一个弱函数。
//当捕获发生后，可以重新定义相应的逻辑。
//注意：此中断回调函数，需要要执行HAL_TIM_IC_Start_IT时才会被调用。
```

# 定时器从模式功能

## 定时器从模式控制器原理与作用：

![image-20240907161245790](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240907161245790.png)

## 从模式控制器作为从机使用的几种常用模式：

**从模式控制器作为从机，可以控制定时器的启动，停止， 复位，增加，减少的功能。**

### 从模式禁止：即使不使用从模式控制，不会影响计数单元的计数功能。

### ResetMod复位模式：使用TRGI的上升沿来复位CNT的计数值，同时产生Updata溢出事件。与普通的溢出功能类似。如下图：

![image-20240907161639619](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240907161639619.png)

### GateMode模式：使用TRGI控制计数单元的开关：

![image-20240907161715986](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240907161715986.png)

### 触发模式：使用TRGI的上升沿来启动定时器

![image-20240907162351724](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240907162351724.png)

### 外部时钟模式1：把TRGI作为定时器的时钟：

![image-20240907162437998](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240907162437998.png)

## 编码器模式1~3：

这三种模式均为连接编码器，所谓的编码器就是用于测量角度和转速的传感器。

编码器模式（Encoder Mode）是一种专门用于处理旋转编码器信号的模式。旋转编码器是一种可以将机械旋转运动转换为电信号的装置，通常用于测量旋转角度、位置、速度等。编码器通常输出两路相位相差90度的信号，称为 A 相和 B 相。

### 编码器的工作原理

在编码器模式下，定时器会使用输入的两路编码器信号（A 相和 B 相）来增加或减少定时器计数器的值。STM32 定时器支持三种编码器模式：

- 编码器模式 1：仅使用编码器的 A 相信号进行计数。A 相的每个边沿（上升沿或下降沿）都会触发计数器的加减操作，具体加或减取决于 B 相信号的电平。如图所示：

![image-20240907163246700](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240907163246700.png)

- 编码器模式 2：仅使用编码器的 B 相信号进行计数。B 相的每个边沿（上升沿或下降沿）都会触发计数器的加减操作，具体加或减取决于 A 相信号的电平。如图所示：

![image-20240907163254264](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240907163254264.png)

- 编码器模式 3：同时使用 A 相和 B 相信号进行计数。每个 A 相和 B 相的边沿（无论上升沿还是下降沿）都会触发计数器的加减操作，从而实现更高的计数分辨率。如图所示：

![image-20240907163320673](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240907163320673.png)

三种模式对比，如图所示：

![image-20240907163330085](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240907163330085.png)

## 从模式控制器作为主机时的几种常用模式：

**从模式控制器作为主机控制其它模块：比如另一个定时器，ADC或DAC等。**

### Enable使能：通过TRGO把时基单元的开关关态输出出去：如图;

![image-20240907163502930](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240907163502930.png)

### Update更新：每产生一个update事件就向TRGO输出一个脉冲，如图：

![image-20240907163600267](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240907163600267.png)

**注意：从模式与主机模式可以共同使用。**

# 总结：

![image-20240907163630911](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240907163630911.png)

# **第一节、CAN异步通信总线简述：**

CAN总线（Controller Area Network Bus）也叫控制器局域网总线。CAN总线由博世公司开发的一种简洁易用、传输速度快，易扩展，可靠性高的串行通信总线，广泛用于汽车，嵌入式，工业控制领域。

## **1. CAN的特性：**

两根通信线（CAN_H、CAN_L）,线路少，无需共地

```c
CAN_H 和 CAN_L 是控制器局域网（CAN）总线中的两根通信线，用于传输数据
    CAN_H（CAN High）：这是CAN总线的高电平线。在信号传输时，当总线上发送一个“显性”信号（逻辑0）时，CAN_H的电压会上升到较高的电平（通常是3.5V左右）。
    CAN_L（CAN Low）：这是CAN总线的低电平线。在发送“显性”信号时，CAN_L的电压会降低到较低的电平（通常是1.5V左右）。
两条线之间的电压差用于表示信号的逻辑状态：
    显性状态（Dominant State，逻辑0）：CAN_H电压高于CAN_L电压，通常为CAN_H ≈ 3.5V，CAN_L ≈ 1.5V。
    隐性状态（Recessive State，逻辑1）：CAN_H和CAN_L的电压接近相等，通常都在2.5V左右。
通过这种差分信号传输，CAN总线具有很强的抗干扰能力，即使在工业环境中也能可靠地工作
```

差分信号通信，抗干扰能力强

高速CAN: 125k-1Mbps  < 40m;

低速CAN:10K-125Kbps, < 1km;

异步，无需要时钟线，通信速率由设备各自约定。

半双工，可挂载多设备，多设备同时发送数据时通过仲裁判断先后顺序。

11位/29位报文ID，用于区分消息功能，同时决定优先级

可配置1-8字节的有效载荷。

可以实现广播式和请求式两种传输方式

应答，CRC校验，位填充，位同步，错误处理等特性。

## **2.与uart, i2c, spi的对比：**

![image-20241027180322775](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180322775.png)

**CAN的通信方式硬件上类似于uart 中的rs485 + i2c,但是在协议的设计细节上有所不同。**

## **3.CAN硬件电路：**

每个设备通过CAN收发器挂载在CAN总线网络上， CAN控制器引出的TX, RX与CAN收发器相连， CAN收发器引出的CAN_H和CAN_L分别与总线的CAN_H和CAN_L相连。

高速CAN使用闭环网络，CAN_H和CAN_L两端添加的是120Ω的终端电阻。

低速CAN使用开环网络，CAN_H和CAN_L其中一端添加2.2kΩ的终端电阻。

![image-20241027180337164](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180337164.png)

## **4.CAN传输约定的差分电平标准：**

CAN总线采用差分信号，即两线电压差（VCAN_H-VCAN_LV）来传输数据位1/0

高速CAN规定：电压差为0V时表示逻辑1（隐性电平），电压差为2V时表示0（显性电平）

低速CAN规定：电压差为-1.5V时表示逻辑1（隐性电平），电压差为3V时表示逻辑0（显性电平）

![image-20241027180346193](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180346193.png)

**一句话总结：**

**CAN收发器释放总线时，即发生隐性状态，即为1， 当CAN收发器驱动总线时，即向CAN_H拉高为3.5V,将CAN_L拉低为1.5V，差为2V，即为0。**

## **5.CAN收发器的内部结构：**

![image-20241027180410279](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180410279.png)

![image-20241027180415657](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180415657.png)

# **第二节、CAN总线通信协议**

## **1.了解CAN总线通信协议中数据的封装格式，简称帧：**

所谓的帧就是之前我们在其它通信协议上了解的发送数据的格式或者说时序。

为了方便收发双方封装与解析，CAN标准对于通信数据进行以5种类型的封装格式。下面介绍这五种数据格式。

他们分别发送设备主动发送数据的数据帧， 接收高主动请求数据时的远程帧也叫遥控帧，当某个设备检测出错误时向其它设备通知的错误帧，接收设备通知其尚未做好接收准备的过载帧，用于将数据帧及远程帧与前几种封装格式分开的帧间隔。

下面依次进行介绍，不过大家要**熟悉的是数据帧与远程帧**，**其它的几种帧格式主要用于异常或错误的处理，大家了解即可**。

![image-20241027180437326](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180437326.png)

### **1.1数据帧：**

数据帧的格式如图所示：

当发送方主动发送数据有效传输数据时，则使用以下格式进行发送数据，属于广播式的发送数据，以下为此报文格式：

CAN数据帧的格式有标准格式与扩展格式，扩展格式为后面新增的报文格式。因为又扩展了报文ID的长度。

### **1. 标准帧：**

空闲时：CAN总线处理隐性电平1的状态。

SOF：起始标记位，永远是显性电平0，标记传输开始。后面跟11位设备ID

IDentifier: 11位的报文标识符ID，包含此报文的优先级，按仲裁规则，报文ID小的优先级越高。报文ID还用于表后面的数据的功能，如传输具体数据，还是错误，还是远程请求等。不同功能的数据帧，其报文ID都各不相同。

RTR: 占据1bit, 在数据帧里必须为显性0，是远程请求标志位，即位为隐性1时，则此数据帧为远程请求帧，或者说是遥控帧。

这个RTR如果是0就是类似于发送，如果类似于读取，与i2c的7位从机地址 + 1位的读写标记位神似。

IDE: 用于区分是标准帧还是扩展帧。标准帧为0，扩展帧为1.

R0:1bit是保留位，用于以后的扩展使用，目前为保留。

DLC: 4bit，用于表示传输的数据是几个字节有效数据，一般为1~8字节。

Data: 有效的数据载荷，一般为1~8字节。

CRC：15bit循环冗余校验，校验数据是否正确。类似于串口的奇偶校验。

CRC界定符 / ACK界定符：为应答位前后发送方和接收方释放总线留下时间。

ACK槽：接收方应答位，由接收方置位。如果有应答，则会逻辑0;

EOF：帧结束，表示数据位已经传输完毕。

### **2. 扩展帧：**

只是对报文ID进行了扩展，基它与标准帧一样。下面介绍一下扩展帧的内容：

SRR: 用于替换RTR位的，由于在CAN仲裁过程中，标准帧的要优先于扩展帧，所以这个替换RTR的位的SRR永远保持1，所以标准帧优先于扩展帧传输。

IDE：如果是扩展帧，所以此位此永远保持1

在扩展帧IDE后跟着的是扩展之后的18bit报文ID, 这些扩展的更多的数据位，可以表示更多的数据类型或消息类别。

在扩展帧后面的数格封装格式就与标准帧是一样的了。

### **1.2远程帧（遥控帧）：**

没有有效数据载荷的数据帧，这个数据帧更像 是一种远程请求读取的标记。我们来看一下CAN的远程请求帧的封装格式：如图所示：

远程请求帧，无有效数据段，RTR为隐性电平1， 其它部分与数据帧相同。

![image-20241027180505206](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180505206.png)

CAN总线的数据主要靠发送方自觉广播，一般发送方会定一个周期，定时广播自己的数据。但是如果发送方没有及时发出数据，或者说这个数据的使用频率太低，如果广播太频繁，大家也都不同，这样就会浪费总线的资源，所以CAN采用了一种请求的方式，发送方可以不主动广播这个数据，而是如果有接收设备需要的话，接收设备可以发出一个远程请求帧，远程请求帧包含报文ID，其实远程请求帧也是广播出来的，每个设备也都可以接收这个远程请求帧。远程请求帧的报文ID应该与被请求的数据帧的报文ID完全一致。当发送方有与些ID相同的数据帧时，就可以进行广播。

### **远程请求帧工作机制：**

- 当一个节点需要特定的数据时，它发送一个远程请求帧，这个帧中包含的报文ID与期望接收到的数据帧的ID一致。

- 当网络中的其他节点收到这个远程请求帧后，如果有节点负责发送包含相同报文ID的数据帧，它将响应这个请求，发送相应的数据帧。

- 例如，如果节点A想要获取节点B的某个传感器数据，它会发送一个远程请求帧，其中的报文ID与该传感器数据帧的ID一致。节点B收到后，会发送包含传感器数据的数据帧。

远程请求帧使用了请求式的数据传输，可以实现在某些数据使用频次少的场景中，可以最大的节约CAN的总线资源。

如果数据使用频率高，再用远程请求帧他的实时式就不好了。如果使用频率高的数据，可以直接使用常用的广播式。

### **1.3错误帧：**

当CAN节点检测到通信过程中出现错误时，它会主动发送错误帧，以通知总线上的所有其他节点该错误的存在。一旦发现“位错误” 或 “填充错误” 或 “CRC错误” 或“格式错误” 或“应答错误”，这些设备便主发出错误帧来破坏数据，同时终止当前的发送设备。

![image-20241027180524072](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180524072.png)

错误帧由两部分组成：

1. 主动错误标志（Active Error Flag）：由发送错误帧的节点发送的6个连续显性（dominant）位（逻辑0）组成。如果节点处于错误被动状态（error passive state），则发送6个隐性（recessive）位（逻辑1），称为被动错误标志（Passive Error Flag）。主动错误标志位段还会有额外的0到6个显性位。这些额外的位是为了防止总线上出现特定的错误情况，例如在总线上可能由于同步问题需要添加额外的位来确保错误信号的正确传播。
2. 错误分隔符（Error Delimiter）：由8个连续的隐性位组成，用于标识错误帧的结束。

### **1.4过载帧：**

当接收方收到大量数据而没有完全处理时，其可以发出过载帧，延缓发送方的数据发送，以平衡总线负载，避免数据丢失。

![image-20241027180532574](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180532574.png)

### **1.5帧间隔：**

将数据帧和远程请求帧与前面的帧分离开。在连续发送数据帧时，其中间会有一小段帧间隔，用来分隔不同的数据帧。主要是用来保护CAN总线上的数据稳定与仲载，给收发两设的设备留出足够的时间来处理数据。

![image-20241027180542780](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180542780.png)

### **2.位填充：**

位填充（Bit Stuffing）是CAN（Controller Area Network）通信协议中用于保持同步和数据完整性的一种技术。位填充规则规定，发送方在传输数据时，如果出现了连续的5个相同电平位，必须插入一个相反电平的位。这种机制确保了总线上不会出现过长的同一电平，避免接收器失去同步。接收方检测到填充位时，会自动移除填充位，恢复原始数据。

![image-20241027180557092](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180557092.png)

### **3.接收方的采样规则：**

CAN总线没有时钟线，总线的所有设备通过约定波特率真的方式确定每一个数据位的时长，发送方以约定的位时长每隔固定时间输出一个数据位。

接收方以约定的位时长每隔固定的时间采样总线的电平，输入一个数据位。理想状态下，接收方能依次彩样到发磅方发出的每个数据位，且彩样点处于数据位中心附近。所以下面我们介绍一下数据发送与接收的规则：位同步。

## **第三节、位同步（数据采样机制）：**

在CAN（控制器局域网）通信中，**位同步（Bit Synchronization）** 是指接收节点调整自己的时钟以匹配发送节点的时钟，从而确保数据帧中的每个位都能正确解码。位同步的目的是保证所有网络节点在相同的时间点采样数据位，这对CAN网络的可靠通信非常关键。

## **1.位同步的实现**

**CAN通信中的位同步是基于边沿跳变的。**当CAN节点检测到边沿跳变时（例如从显性到隐性或反之），它会进行重同步调整，而不是在每个bit都进行独立的同步。这种方法使CAN网络能够在不同节点之间保持一致的时序，即使存在一些时钟漂移或抖动。

CAN网络中每个位传输所需的时间，被称为位时间：它被划分为多个段（如Sync_Seg, Prop_Seg, Phase_Seg1, Phase_Seg2）来精确控制每个位的传输。位时间是由各节点的时钟决定的，但这不意味着每个节点的时钟必须是精确同步的。CAN网络通过同步机制来协调节点之间的时钟差异。

位时间被进一步划分为多个时间段，常见的段有以下几种：

1. 同步段（Synchronization Segment, Sync_Seg）：每个位时间的开始部分，用于同步接收节点的时钟。Sync_Seg 通常很短，约为一个时间量子（Time Quantum）。
2. 传播时间段（Propagation Time Segment, Prop_Seg）：用于补偿信号在节点间传输的延迟时间。
3. 相位缓冲段1（Phase Buffer Segment 1, Phase_Seg1）：用来补偿节点间的相位差，并进行时钟同步调整。
4. 相位缓冲段2（Phase Buffer Segment 2, Phase_Seg2）：类似于 Phase_Seg1，也用于同步调整。

![image-20241027180614820](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180614820.png)

## **2.位同步的机制**

CAN协议采用以下两种位同步机制：

1. 硬同步（Hard Synchronization）：硬同步发生在帧的开始，即遇到帧起始位（SOF，Start of Frame）时。此时，接收节点的时钟会立即重新同步到传输的SOF信号。硬同步通常在总线处于空闲状态且新的帧开始时触发。

![image-20241027180625688](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180625688.png)

1. 重新同步（Resynchronization）：在帧传输过程中，CAN控制器持续监控位边界。当检测到边沿时，如果接收位的实际边界与期望的时钟边界之间存在偏差，接收节点会调整其时钟以最小化这个偏差。这种动态调整使得网络中的节点能够在整个帧传输过程中保持同步。

![image-20241027180636200](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180636200.png)

## **3.位同步的重要性**

1. 确保数据的准确性：位同步确保所有CAN网络节点在正确的时刻采样数据位，从而减少误码率，提高数据传输的准确性。
2. 支持多个节点通信：在分布式CAN网络中，多个节点可能具有不同的时钟源。位同步允许这些节点协调工作，即使它们的内部时钟有轻微的差异，也能保证可靠的通信。
3. 避免信号干扰：通过位同步，CAN网络可以更好地抵抗由于电气干扰、信号反射等原因引起的误差。

## **4.波特率计算：**

![image-20241027180708493](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180708493.png)

## **5.STM32中的位时序：**

![image-20241027180716995](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180716995.png)

![image-20241027180721206](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180721206.png)

![image-20241027180724117](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180724117.png)

# **第四节、CAN总线资源分配规则（仲裁）：**

当多个设备同时发送数据时，CAN总线的资源如何管理呢？CAN协议中关于资源的分配也是一个问题。所以我们要再了解一下资源分配的仲裁机制

CAN总线只有一对差分信号线，同一时间只能有一个设备操作总线发送数据，若多个设同时有发送的需求，该如如分配总线资源？

解决的问题的思路：制定资源分配规则，依次满足多个设备的发送需求，确保同一时间只有一个设备操作总线。下面我们来看一看这个分配规则是什么？

如果当一个设备正在总线发送数据帧时，另一个设备也想发送数据帧，那面就会使用先占先得的方面，先占用总线先发送。这种情况比较好理解，就是先人后道的社会常识。

如果两个设备波形都是在同一时刻发出，那么就需要使用另一种规则：非破坏性仲裁了。

下面我们对这两种方式进行学习：

## **1. 规则1：先占先得**

如果当前已经有设备正在操作总线发送数据帧或远程请求帧，则其它任何设备不能再同时发送数据帧或远程请求帧（可以发送错误帧或过载帧破坏当前数据）

任何设备**检测到连续11个隐性电平，即认为总线空闲**，只有总线空闲时，设备才能发磅数据帧、或 远程请求帧。

一旦有设备正在发送数据帧、或远程请求帧，总线就会变为活跃状态，必须不会出现连续11个隐性电平，其他设备自然也不会破坏当前发送。

若总线活跃状态其他设备有发送需求，则需要等待总线变为空闲，才能执行发送需求。

*在CAN中设备占用总线，并没有类似于中断那样的抢占方式，因为抢占，在线与的条件下，谁的数据也不会发出去。*

## **2.规则2：非破坏性仲裁**

当多个设备都检测到当前总线出现了空闲，哪又如何处理呢？那么我个来了解一下规则2非破坏性仲裁：

若多个设备的发送需求同时到来，或因等待而同时到来，则CAN总线协议会根据ID号（仲裁段）进行非破坏性仲裁，ID号小的（优先级高）取到总线控制权，ID号大的（优先级低）仲裁失利后将转入接收状态，等待下一次总线空闲时再尝试发送。

实现非破坏性仲裁需要两个要求：

线与特性：总线上任何一个设备发送显性电平0时，总线就会呈现显性电平0状态，只有当所有设备都发送隐性电平1时，总线才呈现隐性电平1状态。

回读机制：每个设备发出一个数据位后，都会读回总线当前的电平状态，以确定自己发出的电平是否被真实的发送出去了，根据线与特性，发出0读回必然是0， 发出1读回的不一定是1.

如果设备比较多，那么仲裁是会经常发生的。

![image-20241027180747201](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180747201.png)

注意：如果数据帧和远程请求帧ID号一样时，数据帧的优先级高于远程请求帧。相同ID号的数据帧是不可以出现的。相同ID号的远程请求帧也是不可以同时出现的。

# **第五节、错误处理**

错误处理，是CAN总线额外的安全保障机制，目的是：当某个设备检测到总线的帧传输出错时，可以主动发出错误帧，通知每个设备这个帧出错了，所有设备丢弃此数据帧，不要被有错误的数据帧所影响。其实没有错误处理的设计，CAN总线仍然可以实现完整的数据收发。但是事实情况中CAN总线并不会总控我们的想象工作，在一些干扰很强的地方，数据位的传输随时可能出错，当传输出错时，如何进行后续的操作，这也是很重要的设计。

所以我们来了解一下在面对各种可能的错误情况，CAN协议中是如何处理错误的？

CAN协议规定了5种可能出现的错误类型。

这五种错误类型为：位错误，填充错误，CRC错误，格式错误，应答错误。

![image-20241027180803523](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180803523.png)

# **第六节、CAN控制器工作原理：**

## **1. CAN控制器 ：**

STM32内置了CAN控制器，关于数据的协议封装格式，不用我们自己去封装了，只要把数据交给CAN控制器，那么CAN控制器就会按照CAN的协议格式把数据发出去，接收也是如此。STM内置CAN控制器支持CAN2.0A与CAN2.0B，可以自动发送CAN报文和按照过滤器自动接收指定的CAN报文，程序只需要处理报文数据而无需要关注总线协议及低层电平细节。

CAN控制器支持的波特率最高可达1Mbps/s

有3个可配置优先级的发送缓冲（也称发送邮箱,一个邮箱保存一个等待发送的报文，也就是保存3个优先级报文）

2个3级深度的接收FIFO（也称接收邮箱，一共2*3 = 6条 经过过滤器验证的报文）

14个过滤器（互联型28个）

时间触发通信、自动离线恢复、自动唤醒，禁 止自动重传、接收FIFO溢出处理方式可配置，发送优先级可配置，双CAN模式。

![image-20241027180821543](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180821543.png)

## **2.CAN控制器框图：**

![image-20241027180829259](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180829259.png)

CAN控制器收发框简易图

![image-20241027180836325](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180836325.png)

## **3.bxCAN发送处理：**

![image-20241027180848425](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180848425.png)

## **4.bxCAN接收管理：**

![image-20241027180856588](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180856588.png)

## **5.标识符ID过滤器：**

某个CAN设备节点向总线发出的数据，到底是否是另一个CAN设备节点需要的数据呢？CAN外设节点使用过滤器判定是否是自己使用的数据。

stm32f103的CAN外设（互联型）有28个过滤器，非互联型有14个过滤器,所以接下来介绍一下过滤器的原理。

标识符过滤器框图：

每个过滤器的核心由两个32位寄存器组成：R1[31:0]和R2[31:0]

FSCx:位宽设置，置0为16位， 置1为32位

FBMx:模式设置，置0为屏蔽模式，置1为列表模式。

FFAx:关联设置，置0为FIFO0， 置1为FIFO1

FACTx:激活设置，置0为禁用，置1为启用。

![image-20241027180909066](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180909066.png)

屏蔽模式主要用于连续的ID的多个报文，列表模式主要用于精确某个几个报文。

![image-20241027180915562](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180915562.png)

## **6.bxCAN工作模式：**

![image-20241027180926373](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180926373.png)

## **7.Debug测试模式：**

### **1. 静默模式：只接收不发送**

![image-20241027180938568](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180938568.png)

### **2.环回模式：自发自收，用于单端CAN测试**

![image-20241027180946779](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180946779.png)

### **3.环回静默模式：**

![image-20241027180958618](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241027180958618.png)

# **第七节、CAN控制器HAL库驱动API:**

## **1. 配置CAN模块过滤器：**

```c
HAL_StatusTypeDef HAL_CAN_ConfigFilter(CAN_HandleTypeDef *hcan, const CAN_FilterTypeDef *sFilterConfig);
功能：CAN 过滤器的配置对于选择接收哪些 CAN 消息，通过设置过滤器可以减少接收到的无关消息。
参数说明
    hcan: 指向 CAN 句柄的指针（CAN_HandleTypeDef 类型），该句柄包含了 CAN 外设的配置信息和状态信息。
    sFilterConfig: 指向 CAN 过滤器配置结构体（CAN_FilterTypeDef 类型）的指针，用于设置 CAN 过滤器的参数。
CAN_FilterTypeDef 结构体
        该结构体用于定义 CAN 过滤器的配置参数。以下是该结构体中的常用成员变量及其说明：
FilterBank: 过滤器银行编号。STM32 通常有多个过滤器银行，编号从 0 开始。
FilterMode: 过滤器模式，可以是以下两种之一：
    CAN_FILTERMODE_IDMASK: 标识符屏蔽模式。在这种模式下，只有当 CAN 消息的标识符与过滤器标识符匹配时，消息才会被接收。
    CAN_FILTERMODE_IDLIST: 标识符列表模式。在这种模式下，可以设置一个标识符列表，如果消息的标识符与列表中的任何一个匹配，消息将被接收。
FilterScale: 过滤器的位宽，可以是以下之一：
    CAN_FILTERSCALE_16BIT: 过滤器使用 16 位宽度。
    CAN_FILTERSCALE_32BIT: 过滤器使用 32 位宽度。
FilterIdHigh和 FilterIdLow: 用于设置过滤器的标识符。根据选择的 FilterScale，这两个字段的含义有所不同：
    在 16 位模式下，它们分别代表两个 16 位标识符。
    在 32 位模式下，它们合在一起表示一个 32 位标识符。
FilterMaskIdHigh和 FilterMaskIdLow: 用于设置标识符掩码。这些掩码用于过滤标识符，只有符合掩码要求的标识符才会被接收。
FilterFIFOAssignment: 指定消息进入的接收 FIFO，可以是 CAN_RX_FIFO0 或 CAN_RX_FIFO1。
FilterActivation: 启用或禁用过滤器。设置为 ENABLE 激活过滤器，设置为 DISABLE 禁用过滤器。
SlaveStartFilterBank: 对于双 CAN 配置的设备，指定 CAN2 使用的起始过滤器银行。
返回值
    HAL_OK: 过滤器配置成功。
    HAL_ERROR: 配置过程发生错误。
    HAL_BUSY: CAN 状态机忙，配置无法进行。
    HAL_TIMEOUT: 配置过程超时。
```

## **2. 启用与停止CAN控制器：**

```c
/* Control functions **********************************************************/
HAL_StatusTypeDef HAL_CAN_Start(CAN_HandleTypeDef *hcan);
功能：启用CAN外设控制器
参数：hcan:CAN控制器对象指针
HAL_StatusTypeDef HAL_CAN_Stop(CAN_HandleTypeDef *hcan);
功能：停止CAN外设控制器
参数：hcan:CAN控制器对象指针
返回值:
HAL_OK：操作成功。
HAL_ERROR：操作失败。
```

### **3.休眠与唤醒及检查是否休眠：**

```c
HAL_StatusTypeDef HAL_CAN_RequestSleep(CAN_HandleTypeDef *hcan);
功能：请求休眠CAN模块，以便减少功耗。
参数：hcan:CAN控制器对象指针
HAL_StatusTypeDef HAL_CAN_WakeUp(CAN_HandleTypeDef *hcan);
功能：唤醒CAN模块
参数：hcan:CAN控制器对象指针
返回值:
HAL_OK：操作成功。
HAL_ERROR：操作失败。

uint32_t HAL_CAN_IsSleepActive(const CAN_HandleTypeDef *hcan);
功能: 检查CAN外设是否处于睡眠模式。
参数:
hcan：指向CAN句柄的指针。
返回值:
    1：CAN外设处于睡眠模式。
    0：CAN外设不在睡眠模式。
典型用途: 用于验证CAN是否成功进入睡眠模式。
```

### **4.向CAN发送邮箱添加一个传输消息**

```c
HAL_StatusTypeDef HAL_CAN_AddTxMessage(CAN_HandleTypeDef *hcan, const CAN_TxHeaderTypeDef *pHeader, const uint8_t aData[], uint32_t *pTxMailbox);
功能: 向CAN发送邮箱添加一个传输消息。
参数:
    hcan：指向CAN句柄的指针。
    pHeader：指向CAN传输头的指针（包含标识符、DLC等）。
    aData：指向要发送的数据的指针（字节数组）。
    pTxMailbox：指向可用邮箱的指针（返回分配的邮箱编号）。
返回值:
    HAL_OK：操作成功。
    HAL_ERROR：操作失败。
典型用途: 用于发送CAN数据帧。
```

### **5.发送辅助函数：获取空闲传输邮箱的数量，中止传输，传输信息是否排队，与发送消息时间戳**

```c
uint32_t HAL_CAN_GetTxMailboxesFreeLevel(const CAN_HandleTypeDef *hcan);
功能: 获取空闲传输邮箱的数量。
参数:
    hcan：指向CAN句柄的指针。
返回值: 空闲的传输邮箱数量（0到3）。
典型用途: 检查是否有可用的邮箱来发送新消息。

HAL_StatusTypeDef HAL_CAN_AbortTxRequest(CAN_HandleTypeDef *hcan, uint32_t TxMailboxes);
功能: 取消一个挂起的传输请求。
参数:
hcan：指向CAN句柄的指针。
TxMailboxes：要取消的传输邮箱（位掩码，例如CAN_TX_MAILBOX0）。
返回值:
HAL_OK：操作成功。
HAL_ERROR：操作失败。
典型用途: 在不需要发送数据或需要紧急中止传输时使用。

uint32_t HAL_CAN_IsTxMessagePending(const CAN_HandleTypeDef *hcan, uint32_t TxMailboxes);
功能: 检查指定邮箱中的传输消息是否仍在排队等待发送。
参数:
hcan：指向CAN句柄的指针。
TxMailboxes：要检查的邮箱编号。
返回值:
    1：有消息等待发送。
    0：没有消息等待发送。
典型用途: 用于检测邮箱中的消息是否已成功发送。

uint32_t HAL_CAN_GetTxTimestamp(const CAN_HandleTypeDef *hcan, uint32_t TxMailbox);
功能: 获取指定邮箱中最后发送消息的时间戳。
参数:
    hcan：指向CAN句柄的指针。
    TxMailbox：指定的邮箱编号（0、1、2）。
返回值: 发送消息的时间戳值。
典型用途: 用于时间同步或记录发送时间。
```

### **6.从指定的接收FIFO中接收CAN消息**

```c
HAL_StatusTypeDef HAL_CAN_GetRxMessage(CAN_HandleTypeDef *hcan, uint32_t RxFifo, 
                                            CAN_RxHeaderTypeDef *pHeader, uint8_t aData[]);
功能: 从指定的接收FIFO中接收CAN消息。
参数:
    hcan：指向CAN句柄的指针。
    RxFifo：接收FIFO编号（0或1）。
    pHeader：指向接收消息头的指针（存储标识符、DLC等）。
    aData：指向接收到的数据的指针（字节数组）。
返回值:
    HAL_OK：操作成功。
    HAL_ERROR：操作失败。
典型用途: 用于从接收FIFO读取收到的CAN数据帧。    
```

### **7.接收辅助函数：获取指定接收FIFO中的消息数量**

```c
uint32_t HAL_CAN_GetRxFifoFillLevel(const CAN_HandleTypeDef *hcan, uint32_t RxFifo);
功能: 获取指定接收FIFO中的消息数量。
参数:
    hcan：指向CAN句柄的指针。
    RxFifo：FIFO编号（0或1）。
返回值: 接收FIFO中消息的数量。
典型用途: 用于检查接收FIFO中是否有新消息等待处理。
```

### **8.中断方式CAN的收发管理与中断回调函数：**

```c
/* Interrupts management ******************************************************/
HAL_StatusTypeDef HAL_CAN_ActivateNotification(CAN_HandleTypeDef *hcan, uint32_t ActiveITs);
功能: 启用指定的CAN中断通知。
参数:
    hcan：指向CAN句柄的指针（CAN_HandleTypeDef类型）。
    ActiveITs：要启用的中断类型，采用位掩码方式。可选项包括以下几种（可以组合使用）：
    CAN_IT_RX_FIFO0_MSG_PENDING：RX FIFO 0消息挂起中断。
    CAN_IT_RX_FIFO0_FULL：RX FIFO 0满中断。
    CAN_IT_RX_FIFO1_MSG_PENDING：RX FIFO 1消息挂起中断。
    CAN_IT_RX_FIFO1_FULL：RX FIFO 1满中断。
    CAN_IT_TX_MAILBOX_EMPTY：TX邮箱空中断。
    CAN_IT_ERROR_WARNING：错误警告中断。
    CAN_IT_ERROR_PASSIVE：错误被动中断。
    CAN_IT_BUSOFF：总线关闭中断。
    CAN_IT_LAST_ERROR_CODE：最后错误代码中断。
    CAN_IT_ERROR：一般错误中断。
返回值:
    HAL_OK：操作成功。
    HAL_ERROR：操作失败。
典型用途: 在CAN通信过程中，启用接收消息、传输完成或错误状态的中断，以便应用程序可以通过中断回调函数来处理这些事件。

HAL_StatusTypeDef HAL_CAN_DeactivateNotification(CAN_HandleTypeDef *hcan, uint32_t InactiveITs);
功能: 禁用指定的CAN中断通知。
参数:
    hcan：指向CAN句柄的指针。
    InactiveITs：要禁用的中断类型，采用位掩码方式。参数与HAL_CAN_ActivateNotification中的ActiveITs参数类似。
返回值:
    HAL_OK：操作成功。
    HAL_ERROR：操作失败。
典型用途: 在不需要接收特定事件通知时禁用相应的CAN中断，例如在进入低功耗模式或改变通信策略时。

void HAL_CAN_IRQHandler(CAN_HandleTypeDef *hcan);
CAN中断的句柄，此函数最终会回调以下的回调处理函数
/* Callbacks functions ********************************************************/
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_TxMailbox0AbortCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_TxMailbox1AbortCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_TxMailbox2AbortCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_SleepCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_WakeUpFromRxMsgCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan);
```

### CAN_driver.c

```c
#include "can_driver.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lcdinit.h"
#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*偶数可以发送出去 奇数不能*/

/*设置CAN过滤器*/
int CAN_Filter_Init(void)
{
    /*设置CAN过滤器规则*/
    CAN_FilterTypeDef can_filter;
    can_filter.FilterBank = 0; //过滤器组
    can_filter.FilterMode = CAN_FILTERMODE_IDMASK; //屏蔽位模式
    can_filter.FilterScale = CAN_FILTERSCALE_32BIT; //32位

    //设置规则
    can_filter.FilterIdHigh = 0x0020; //高16位
    can_filter.FilterIdLow = 0x0000; //低16位
    can_filter.FilterMaskIdHigh = 0x0020; //屏蔽位高16位
    can_filter.FilterMaskIdLow = 0x0006; //屏蔽位低16位


    //设置过滤器的报文在哪一个FIFO中接收
    can_filter.FilterFIFOAssignment = CAN_RX_FIFO0;
    can_filter.FilterActivation = CAN_FILTER_ENABLE;

    //把设置好的过滤器参数设置到can模块中
    HAL_CAN_ConfigFilter(&hcan1, &can_filter);

    //启动can模块
    HAL_CAN_Start(&hcan1);
    vTaskDelay(10);

    return 0;
}

/*发送CAN数据*/
int CAN_sendMessage(uint32_t msgid, uint8_t *txData, uint8_t len)
{
    CAN_TxHeaderTypeDef can_tx_header;
    uint32_t tx_mailbox;

    //配置发送报文的格式 stdid + 数据 + RTR + IDE + DLC
    can_tx_header.StdId = msgid; //标准ID
    can_tx_header.RTR = 0; //数据帧
    can_tx_header.IDE = 0; //标准帧
    can_tx_header.DLC = len; //数据长度

    //查看是否有空闲邮箱
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0); //没有空闲邮箱就等待

    //发送数据
    HAL_CAN_AddTxMessage(&hcan1, &can_tx_header, txData, &tx_mailbox);

    //发送成功 在LCD显示结果
    char buf[32] = {0};
    sprintf(buf, "send ok! id=%d", can_tx_header.StdId);
    LCD_ShowString(10,0,(const uint8_t *)buf,WHITE,BLACK,12,0);

    vTaskDelay(100);

    return 0;

}

/*接收CAN数据*/
int CAN_receiveMessage(uint8_t *rxData){
    
    CAN_RxHeaderTypeDef can_rx_header;

    //判断接收邮箱是否有数据
    if (HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) == 0)
    {
        //没有收到数据
        LCD_ShowString(10,10,(const uint8_t *)"meg not receive",WHITE,BLACK,12,0);
        LCD_ShowString(10,20,"                        ",WHITE,BLACK,12,0);
        
    }
    else
    {
        //收到数据
        LCD_ShowString(10,10,(const uint8_t *)"                 ",WHITE,BLACK,12,0);
        HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &can_rx_header, rxData);
        char buf[32] = {0};
        sprintf(buf, "id=%d,data=%d", can_rx_header.StdId,*rxData);
        LCD_ShowString(10,20,(const uint8_t *)buf,WHITE,BLACK,12,0);
    }
    vTaskDelay(100);

    return 0;
}

```

