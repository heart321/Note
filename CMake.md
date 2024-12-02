# CMake

## 1.CMake概述

CMake是一个项目构建工具，并且是跨平台的。关于项目构建我们所熟知的还有Makefile（通过 make 命令进行项目的构建），大多是IDE软件都集成了make，比如：VS 的 nmake、linux 下的 GNU make、Qt 的 qmake等，如果自己动手写 makefile，会发现，makefile 通常依赖于当前的编译平台，而且编写 makefile 的工作量比较大，解决依赖关系时也容易出错。

而 CMake 恰好能解决上述问题， 其允许开发者指定整个工程的编译流程，在根据编译平台，自动生成本地化的Makefile和工程文件，最后用户只需make编译即可，所以可以把CMake看成一款自动生成 Makefile的工具，其编译流程如下图：

![image-20241129222559727](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241129222559727.png)

- 蓝色虚线代表makefile构建项目的过程。
- 红色实现表示使用cmake构建项目的过程。

CMake有点：

- 跨平台
- 能够管理大型项目
- 简化编译构建过程和编译过程
- 可扩展:可以为CMake编写特定功能的模块，扩充cmake的功能

## 2.CMake的使用

CMake支持大写、小写、混合大小写的命令。如果在编写CMakeLists.txt文件时使用的工具有对应的命令提示，那么大小写随缘写即可。

### 2.1注释

#### 2.1.1注释行

CMake使用**#**进项行注释，可放在任何位置。

```cmake
#这是一个CMakeLists.txt文件
cmake_minimum_required(VERSION 3.0.0)
```

#### 2.1.2注释块

CMake 使用**#[[ ]]**进行块注释。



### 2.2只有源文件

#### 2.2.1共处一室

- 测试文件

- add.c

  ```c++
  #include <stdio.h>
  #include "head.h"
  
  int add(int a, int b)
  {
      return a+b;
  }
  
  ```

- sub.c

  ```c++
  #include <stdio.h>
  #include "head.h"
  
  // 你好
  int subtract(int a, int b)
  {
      return a-b;
  }
  
  ```

- mult.c

  ```c++
  #include <stdio.h>
  #include "head.h"
  
  int multiply(int a, int b)
  {
      return a*b;
  }
  
  ```

- div.c

  ```c++
  #include <stdio.h>
  #include "head.h"
  
  double divide(int a, int b)
  {
      return (double)a/b;
  }
  
  ```

- head.h

  ```c++
  #ifndef _HEAD_H
  #define _HEAD_H
  // 加法
  int add(int a, int b);
  // 减法
  int subtract(int a, int b);
  // 乘法
  int multiply(int a, int b);
  // 除法
  double divide(int a, int b);
  #endif
  
  ```

- main.c

  ```c++
  #include <stdio.h>
  #include "head.h"
  
  int main()
  {
      int a = 20;
      int b = 12;
      printf("a = %d, b = %d\n", a, b);
      printf("a + b = %d\n", add(a, b));
      printf("a - b = %d\n", subtract(a, b));
      printf("a * b = %d\n", multiply(a, b));
      printf("a / b = %f\n", divide(a, b));
      return 0;
  }
  
  ```

文件目录如下：

```tcl
$ tree
.
├── add.c
├── div.c
├── head.h
├── main.c
├── mult.c
└── sub.c
```

**添加CMakeLists.txt文件**

```

```

