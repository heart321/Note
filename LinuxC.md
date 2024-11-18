# 线程的概述

线程是轻量级的进程（LWP：light weight process），在Linux环境下线程的本质仍是进程。在计算机上运行的程序是一组指令及指令参数的组合，指令按照既定的逻辑控制计算机运行。操作系统会以进程为单位，分配系统资源，可以这样理解，进程是资源分配的最小单位，线程是操作系统调度执行的最小单位。

先从概念上了解一下线程和进程之间的区别：

- 进程有自己独立的地址空间, 多个线程共用同一个地址空间

  - 线程更加节省系统资源, 效率不仅可以保持的, 而且能够更高


  - 在一个地址空间中多个线程独享: 每个线程都有属于自己的栈区, 寄存器(内核中管理的)

  - 在一个地址空间中多个线程共享: 代码段, 堆区, 全局数据区, 打开的文件(文件描述符表)都是线程共享的

- 线程是程序的最小执行单位, 进程是操作系统中最小的资源分配单位

  - 每个进程对应一个虚拟地址空间，一个进程只能抢一个CPU时间片

  - 一个地址空间中可以划分出多个线程, 在有效的资源基础上, 能够抢更多的CPU时间片

![image-20240821154517966](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20240821154517966.png)

CPU的调度和切换: 线程的上下文切换比进程要快的多

上下文切换：进程/线程分时复用CPU时间片，在切换之前会将上一个任务的状态进行保存, 下次切换回这个任务的时候, 加载这个状态继续运行，任务从保存到再次加载这个过程就是一次上下文切换。

线程更加廉价, 启动速度更快, 退出也快, 对系统资源的冲击小。

在处理多任务程序的时候使用多线程比使用多进程要更有优势，但是线程并不是越多越好，如何控制线程的个数呢？

文件IO操作：文件IO对CPU是使用率不高, 因此可以分时复用CPU时间片, 线程的个数 = 2 * CPU核心数 (效率最高)

处理复杂的算法(主要是CPU进行运算, 压力大)，线程的个数 = CPU的核心数 (效率最高)

# 创建线程

## 线程函数

每一个线程都有一个唯一的线程ID，ID类型为pthread_t，这个ID是一个无符号长整形数，如果想要得到当前线程的线程ID，可以调用

```c
pthread_t pthread_self(void);	// 返回当前线程的线程ID
```

在一个进程中调用线程创建函数，就可得到一个子线程，和进程不同，需要给每一个创建出的线程指定一个处理函数，否则这个线程无法工作。

```c
#include <pthread.h>
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine) (void *), void *arg);
// Compile and link with -pthread, 线程库的名字叫pthread, 全名: libpthread.so libptread.a

```

参数:

thread: 传出参数，是无符号长整形数，线程创建成功, 会将线程ID写入到这个指针指向的内存中

attr: 线程的属性, 一般情况下使用默认属性即可, 写NULL

start_routine: 函数指针，创建出的子线程的处理动作，也就是该函数在子线程中执行。

arg: 作为实参传递到 start_routine 指针指向的函数内部

返回值：线程创建成功返回0，创建失败返回对应的错误号

## 创建线程

下面是创建线程的示例代码，在创建过程中一定要保证编写的线程函数与规定的函数指针类型一致：void *(*start_routine) (void *):

```c
// pthread_create.c 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

// 子线程的处理代码
void* working(void* arg)
{
    printf("我是子线程, 线程ID: %ld\n", pthread_self());
    for(int i=0; i<9; ++i)
    {
        printf("child == i: = %d\n", i);
    }
    return NULL;
}

int main()
{
    // 1. 创建一个子线程
    pthread_t tid;
    pthread_create(&tid, NULL, working, NULL);

    printf("子线程创建成功, 线程ID: %ld\n", tid);
    // 2. 子线程不会执行下边的代码, 主线程执行
    printf("我是主线程, 线程ID: %ld\n", pthread_self());
    for(int i=0; i<3; ++i)
    {
        printf("i = %d\n", i);
    }
    
    // 休息, 休息一会儿...
    // sleep(1);
    
    return 0;
}


```

编译测试程序，会看到如下错误信息：

```makefile
$ gcc pthread_create.c 
/tmp/cctkubA6.o: In function `main':
pthread_create.c:(.text+0x7f): undefined reference to `pthread_create'
collect2: error: ld returned 1 exit status

```

错误原因是因为编译器链接不到线程库文件（动态库），需要在编译的时候通过参数指定出来，动态库名为 libpthread.so需要使用的参数为 -l，根据规则掐头去尾最终形态应该写成：-lpthread（参数和参数值中间可以有空格）。正确的编译命令为：

```c
# pthread_create 函数的定义在某一个库中, 编译的时候需要加库名 pthread
$ gcc pthread_create.c -lpthread
$ ./a.out 
子线程创建成功, 线程ID: 139712560109312
我是主线程, 线程ID: 139712568477440
i = 0
i = 1
i = 2
```

在打印的日志输出中为什么子线程处理函数没有执行完毕呢（只看到了子线程的部分日志输出）？
主线程一直在运行, 执行期间创建出了子线程，说明主线程有CPU时间片, 在这个时间片内将代码执行完毕了, 主线程就退出了。子线程被创建出来之后需要抢cpu时间片, 抢不到就不能运行，如果主线程退出了, 虚拟地址空间就被释放了, 子线程就一并被销毁了。但是如果某一个子线程退出了, 主线程仍在运行, 虚拟地址空间依旧存在。

得到的结论：在没有人为干预的情况下，虚拟地址空间的生命周期和主线程是一样的，与子线程无关。

目前的解决方案: 让子线程执行完毕, 主线程再退出, 可以在主线程中添加挂起函数 sleep();

## 线程退出

在编写多线程程序的时候，如果想要让线程退出，但是不会导致虚拟地址空间的释放（针对于主线程），我们就可以调用线程库中的线程退出函数，只要调用该函数当前线程就马上退出了，并且不会影响到其他线程的正常运行，不管是在子线程或者主线程中都可以使用。

```c
#include <pthread.h>
void pthread_exit(void *retval);
参数: 线程退出的时候携带的数据，当前子线程的主线程会得到该数据。如果不需要使用，指定为NULL
```

下面是线程退出的示例代码，可以在任意线程的需要的位置调用该函数：

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

// 子线程的处理代码
void* working(void* arg)
{
    sleep(1);
    printf("我是子线程, 线程ID: %ld\n", pthread_self());
    for(int i=0; i<9; ++i)
    {
        if(i==6)
        {
            pthread_exit(NULL);	// 直接退出子线程
        } 
        printf("child == i: = %d\n", i);
    }
    return NULL;
}

int main()
{
    // 1. 创建一个子线程
    pthread_t tid;
    pthread_create(&tid, NULL, working, NULL);

    printf("子线程创建成功, 线程ID: %ld\n", tid);
    // 2. 子线程不会执行下边的代码, 主线程执行
    printf("我是主线程, 线程ID: %ld\n", pthread_self());
    for(int i=0; i<3; ++i)
    {
        printf("i = %d\n", i);
    }

    // 主线程调用退出函数退出, 地址空间不会被释放
    pthread_exit(NULL);
    
    return 0;
}

```

## 线程回收

### 线程函数

线程和进程一样，子线程退出的时候其内核资源主要由主线程回收，线程库中提供的线程回收函叫做pthread_join()，这个函数是一个阻塞函数，如果还有子线程在运行，调用该函数就会阻塞，子线程退出函数解除阻塞进行资源的回收，函数被调用一次，只能回收一个子线程，如果有多个子线程则需要循环进行回收。

另外通过线程回收函数还可以获取到子线程退出时传递出来的数据，函数原型如下：

```c
#include <pthread.h>
// 这是一个阻塞函数, 子线程在运行这个函数就阻塞
// 子线程退出, 函数解除阻塞, 回收对应的子线程资源, 类似于回收进程使用的函数 wait()
int pthread_join(pthread_t thread, void **retval);

参数:
thread: 要被回收的子线程的线程ID

retval: 二级指针, 指向一级指针的地址, 是一个传出参数, 这个地址中存储了pthread_exit() 传递出的数据，如果不需要这个参数，可以指定为NULL

返回值：线程回收成功返回0，回收失败返回错误号。

```

### 回收子线程数据

在子线程退出的时候可以使用pthread_exit()的参数将数据传出，在回收这个子线程的时候可以通过phread_join()的第二个参数来接收子线程传递出的数据。接收数据有很多种处理方式，下面来列举几种：

#### 使用子线程栈

通过函数pthread_exit(void *retval);可以得知，子线程退出的时候，需要将数据记录到一块内存中，通过参数传出的是存储数据的内存的地址，而不是具体数据，由因为参数是void*类型，所有这个万能指针可以指向任意类型的内存地址。先来看第一种方式，将子线程退出数据保存在子线程自己的栈区：

```c
// pthread_join.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

// 定义结构
struct Persion
{
    int id;
    char name[36];
    int age;
};

// 子线程的处理代码
void* working(void* arg)
{
    printf("我是子线程, 线程ID: %ld\n", pthread_self());
    for(int i=0; i<9; ++i)
    {
        printf("child == i: = %d\n", i);
        if(i == 6)
        {
            struct Persion p;
            p.age  =12;
            strcpy(p.name, "tom");
            p.id = 100;
            // 该函数的参数将这个地址传递给了主线程的pthread_join()
            pthread_exit(&p);
        }
    }
    return NULL;	// 代码执行不到这个位置就退出了
}

int main()
{
    // 1. 创建一个子线程
    pthread_t tid;
    pthread_create(&tid, NULL, working, NULL);

    printf("子线程创建成功, 线程ID: %ld\n", tid);
    // 2. 子线程不会执行下边的代码, 主线程执行
    printf("我是主线程, 线程ID: %ld\n", pthread_self());
    for(int i=0; i<3; ++i)
    {
        printf("i = %d\n", i);
    }

    // 阻塞等待子线程退出
    void* ptr = NULL;
    // ptr是一个传出参数, 在函数内部让这个指针指向一块有效内存
    // 这个内存地址就是pthread_exit() 参数指向的内存
    pthread_join(tid, &ptr);
    // 打印信息
    struct Persion* pp = (struct Persion*)ptr;
    printf("子线程返回数据: name: %s, age: %d, id: %d\n", pp->name, pp->age, pp->id);
    printf("子线程资源被成功回收...\n");
    
    return 0;
}

```

## 线程分离

在某些情况下，程序中的主线程有属于自己的业务处理流程，如果让主线程负责子线程的资源回收，调用pthread_join()只要子线程不退出主线程就会一直被阻塞，主要线程的任务也就不能被执行了。

在线程库函数中为我们提供了线程分离函数pthread_detach()，调用这个函数之后指定的子线程就可以和主线程分离，当子线程退出的时候，其占用的内核资源就被系统的其他进程接管并回收了。线程分离之后在主线程中使用pthread_join()就回收不到子线程资源了。

```c
#include <pthread.h>
// 参数就子线程的线程ID, 主线程就可以和这个子线程分离了
int pthread_detach(pthread_t thread);

```

下面的代码中，在主线程中创建子线程，并调用线程分离函数，实现了主线程和子线程的分离：

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

// 子线程的处理代码
void* working(void* arg)
{
    printf("我是子线程, 线程ID: %ld\n", pthread_self());
    for(int i=0; i<9; ++i)
    {
        printf("child == i: = %d\n", i);
    }
    return NULL;
}

int main()
{
    // 1. 创建一个子线程
    pthread_t tid;
    pthread_create(&tid, NULL, working, NULL);

    printf("子线程创建成功, 线程ID: %ld\n", tid);
    // 2. 子线程不会执行下边的代码, 主线程执行
    printf("我是主线程, 线程ID: %ld\n", pthread_self());
    for(int i=0; i<3; ++i)
    {
        printf("i = %d\n", i);
    }

    // 设置子线程和主线程分离
    pthread_detach(tid);

    // 让主线程自己退出即可
    pthread_exit(NULL);
    
    return 0;
}

```

## 线程取消

线程取消的意思就是在某些特定情况下在一个线程中杀死另一个线程。使用这个函数杀死一个线程需要分两步：

- 在线程A中调用线程取消函数pthread_cancel，指定杀死线程B，这时候线程B是死不了的。

- 在线程B中进程一次系统调用（从用户区切换到内核区），否则线程B可以一直运行。

```c
#include <pthread.h>
// 参数是子线程的线程ID
int pthread_cancel(pthread_t thread);
参数：要杀死的线程的线程ID
返回值：函数调用成功返回0，调用失败返回非0错误号。
```

关于系统调用有两种方式：

- 直接调用Linux系统函数

- 调用标准C库函数，为了实现某些功能，在Linux平台下标准C库函数会调用相关的系统函数

## 线程ID的比较

在Linux中线程ID本质就是一个无符号长整形，因此可以直接使用比较操作符比较两个线程的ID，但是线程库是可以跨平台使用的，在某些平台上 pthread_t可能不是一个单纯的整形，这中情况下比较两个线程的ID必须要使用比较函数，函数原型如下：

```c
#include <pthread.h>
int pthread_equal(pthread_t t1, pthread_t t2);


```

参数：t1 和 t2 是要比较的线程的线程ID
返回值：如果两个线程ID相等返回非0值，如果不相等返回0


