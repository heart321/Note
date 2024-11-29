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

