# Ubuntu文件系统目录

linux下“/”就是根目录，所有的目录都是由根目录衍生出来的。

/bin            存放二进制可执行文件，这些命令在单用户模式下也能够使用。可以被root和一般的账号使用。

/boot         Ubuntu内核和启动文件，比如vmlinxz-xxx gurb引导装载程序。

/dev           设备驱动文件

/etc            存放一些系统配置文件，比如用户账号和密码文件，各种服务的起始地址。

/home       系统默认的用户主文件，一般创建用户账户的时候，默认的用户主文件都会放到此目录下。

/lib             存放库文件。

/media      此目录下放置可热插拔设备，比如SD卡，或者U盘就是挂载到这个目录中。

/mnt          用户可使用的挂载点，如果要挂载一些额外的设备，那么就可以挂载到此处。

/opt            可选的文件和程序存放目录，给第三方软件放置的目录。

/root  		root用户目录，也就是系统管理员目录。

/sbin 		和/bin类似，也是存放一些二进制可执行文件。sbin下面的一般是系统开机过程中所需要的命令。

/srv 			服务相关目录。比如网络服务。

/sys 			记录内核信息。虚拟文件系统。

/tmp  		临时目录。

/var 			存放一些变化的文件，比如日志文件。

/usr			usr不是user的缩写，而是UNIX Software Resource的缩写，存放系统用户的有关文件，会占用很大的空间。

/proc 		虚拟文件系统，数据放置到内存中，存放系统的运行信息。

# Make工具和Makfile的引入

当源码文件比较多的时候就不适合通过直接输入gcc命令来编译，这时候就需要一个自动化的编译工具.

​    make：一般说GNU Make，是一个软件，用于将源代码文件编译为可执行的二进制文件，make工具主要用于完成自动化编译。make工具编译的时候需要Makefile文件提供编译文件。

​    Makefile：make工具所使用的文件，Makefile指明了编译规则。

Makefile规则格式：

```makefile
目标...：依赖文件集合....
	命令1
	命令2
	....
```

比如：

```makefile
main : mian.o input.o output.o
	gcc -o main main.o input.o output.o
main.o:main.c
	gcc -c main.c
input.o:input.c
	gcc -c input.c
output.o:output.c
	gcc -c output.c
	
clean:
	rm *.o
	rm main
```

命令列表的首行缩进使用TAB!

Makefile变量:

```makefile
#Makefile变量的使用
objcts = main.o input.o output.o

main:$(objcts)
	gcc -o main $(objcts)
main.o:main.c
	gcc -c main.c
input.o:input.c
	gcc -c input.c
output.o:output.c
	gcc -c output.c
	
	
clean:
	rm *.o
	rm main
```

Makefile自动化变量：

  $@

  表示目标集，“集”的意思就是组合，全部，有多个目标，$@就是目标集合。

  $^

  所有依赖目标的集合，注意，这里说的是“依赖”，也就是目标的组成元素。

  $<

  依赖目标中的第一个目标名字，也就是上面说的$^中的第一个元素。

```makefile
#Makefile自动化变量
objcts = main.o input.o output.o

main:$(objcts)
	gcc -o main $(objcts)
%.o:%.c
	gcc -c @<
	
	
clean:
	rm *.o
	rm main
```

