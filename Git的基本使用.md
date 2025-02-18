# Git的基本使用

## 初始化邮箱和名称

```sh
git config --global user.name "chen xuan"  			//用户名
git config --global user.email chenxuan_321@126.com //邮箱
git config --global credential.helper store     	//保存用户名和密码
git config --global --list                  		//查看用户名和邮箱
```

省略(Local):本地配置，只对本地仓库有效

--global:全局配置，对所有仓库生效

--system:系统配置，对所有用户生效



## 新建版本库 仓库

### 方式一 在本地创建仓库

```shell
git init   //创建git仓库
```

### 方式二 在github远程创建仓库

```shell
git clone 仓库地址
```

## 

## git的工作目录

![image-20241004140705471](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241004140705471.png)



```shell
git status 	//查看仓库的状态
git add   	//添加到暂存区
			可以使用通配符，例如：git add *.txt
			也可以使用目录，例如：git add .
git commit 	//提交
			只提交暂存区中的内容，不会提交工作区中的内容
git log		//查看仓库提交历史记录
			可以使用--oneline参数来查看简介的提交记录
```



## git reset的三种模式

![image-20241004142531991](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241004142531991.png)



## git diff 

查看工作区、暂存区、本地仓库之间的差异

查看不同版本之间的差异

查看不同分支之间的差异

![image-20241004143523317](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241004143523317.png)



## git rm

```
rm file; git add file 	//先从工作区删除文件，然后再暂存删除内容
git rm <file>			//把文件从工作区和暂存区同时删除
git rm --cached <file> 	//把文件从暂存区删除，但保留在当前工作区中
git rm -r * 			//递归删除某个目录下的所有子目录和文件 删除后不要忘记提交
```



## .gitignore 忽略不提交的文件

![image-20241004144306860](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241004144306860.png)



## 使用Github来管理代码

```
git push //可以将本地仓库的文件推送给远程仓库
git pull //将远程仓库的文件拉取到本地
```

![image-20241004151143639](C:\Users\15819\AppData\Roaming\Typora\typora-user-images\image-20241004151143639.png)



## 关联本地仓库和远程仓库

```shell
git remote -v //查看当前仓库关联的远程仓库
git remote add //关联当前仓库和远程仓库
git remote rm //删除远程仓库
```



## git分支

```shell
git branch  //创建一个新的分支 也可以查看分支列表

git checkout //切换到不同的分支   也可以恢复文件  当文件名称和分支名称相同时会出现歧义
git switch 	//切换分支

//合并分支
git merge  // 合并指定的分支 在主分支上执行

git rebase // 变基  两种方式的并顺序不同

//查看分支合并图
git log --graph --oneline --decorate --all

//删除分支
git branch -D


```

# 关联远程仓库

```
//添加远程仓库
git remote add name url

//查看仓库绑定的远程仓库
git remote -v

```

