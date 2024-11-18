# Qt数据库操作

## 1.概述

​	Qt框架中对数据库操作提供了很好的支持，我们可以通过Qt提供的类非常方便地和本地或者远程数据库进行连接。众所周知，数据库是 C-S（client-server）结构的，我们要连接的数据库属于服务器端，通过Qt编写的应用程序属于客户端。

如果想用通过Qt访问数据库，首先我们需要在项目中添加数据库模块，模块名为sql 。找到项目文件`xxx.pro`，把模块名加进去：

```c++
QT += core gui sql
```

这个`sql`模块其实对应的就是一个动态库，动态库中有数据库相关类的二进制源码，在使用动态库中的数据库类的时候，动态库被加载到内存，根据程序猿的需求来提供相应的服务

- QSqlDatabase ：通过这个类添加/删除/复制/关闭数据库实例
- QSqlQuery ：数据库查询类
- QSqlRecord ：数据库记录（通常是数据库中表或视图中的一行）的功能和特征。
- QSqlField ：数据库表或视图中单个列的特征，例如数据类型和列名。
- QSqlQueryModel ：执行SQL语句和遍历结果集的高级接口。它构建在底层QSqlQuery之上，可以用来为视图类(如QTableView)提供数据。
- QSqlError ：数据操作失败可以通过这个类获取相关的错误信息。

在Qt中操作数据库是非常简单的，但是有一个前提条件就是`需要使用者能够自己通过SQL语句实现数据库表数据的添、删、查、改`。在Qt中不论我们连接的何种类型的关系型数据库，在我们使用的时候其操作流程是一致的：

1. 创建数据库实例并初始化
2. 连接数据库
3. 对数据库进行一系列的添、删、查、改操作（编写并执行SQL语句）
4. 关闭数据库

## 2.类的使用

### 2.1QSqlDatabase

QSqlDatabase类提供了一个连接访问数据库的接口。一个QSqlDatabase的实例就表示一个连接。该连接通过受支持的数据库驱动程序之一提供对数据库的访问，这些驱动程序派生自QSqlDriver。

常见的API:

静态函数：

```c++
//得到可以使用的数据库驱动的名字
[static] QStringList QSqlDatabase::drivers();
```

在高版本的Qt中，有些数据库的驱动/插件（本质是动态库）需要自己编译，比如：MySQL、Oracle。如果没有对应的驱动/插件是无法进行数据库连接的。

```c++
//添加一个数据库实例
[static] QSqlDatabase QSqlDatabase::addDatabase(
    const QString &type, 
    const QString &connectionName = QLatin1String( defaultConnection ));
type： 指定要连接什么样的数据库，就是数据库驱动对应的驱动名
connectionName：数据库连接名，默认叫: defaultConnection，我们可以在应用程序中添加多个数据库连接（也就是多个实例），每个连接都对应一个唯一的名字。
函数的返回值就是得到的数据库实例对象。
```

```c++
//通过数据库连接名得到数据库实例对象
[static] QSqlDatabase QSqlDatabase::database(
    const QString &connectionName = QLatin1String( defaultConnection ), 
    bool open = true);

connectionName: 通过addDatabase()函数的第二个参数指定的连接名
open: 实例的状态
    true: 得到的实例设置为打开状态
	false: 得到的实例设置为关闭状态
```

```c++
//判断连接名对应的连接是否已经存在了，给函数参数指定数据库连接名函数就可以返回连接对应的状态了：true（打开） 或者 false（关闭）。
[static] bool QSqlDatabase::contains(
    const QString &connectionName = QLatin1String( defaultConnection ));
```

公共成员函数：

设置数据库的名称：

```C++
void QSqlDatabase::setDatabaseName(const QString &name);
```

设置数据库的主机名：(一般是指定服务器的IP地址)

```C++
void QSqlDatabase::setHostName(const QString &host);
```

设置数据库服务器绑定的端口：

```C++
void QSqlDatabase::setPort(int port);
//如果数据库服务器绑定的是默认端口，可以省略设置端口的操作。
```

设置连接的数据库中某个用户的用户名:

```C++
void QSqlDatabase::setUserName(const QString &name);
```

设置连接的数据库中某个用户对应的密码：

```c++
void QSqlDatabase::setPassword(const QString &password);
```

连接数据库（必须要先设置连接信息，然后再连接数据库）:

```C++
bool QSqlDatabase::open();
//数据库连接成功返回true，连接失败返回false
```

判断数据库是否打开了，返回值和`open()`函数相同。

```C++
bool QSqlDatabase::isOpen() const;
```

关闭数据库连接

```C++
void QSqlDatabase::close();
```

返回有关数据库上发生的最后一个错误的信息。

```C++
QSqlError QSqlDatabase::lastError() const;
QString QSqlError::text() const;
//该函数返回的是一个QSqlError对象，调用QSqlError类提供的text()方法就可以得到最后一个错误对应的信息描述了。
```

### 事务操作

创建一个事务

```C++
bool QSqlDatabase::transaction();
```

提交事务

```C++
bool QSqlDatabase::commit();
```

事务回滚

```C++
bool QSqlDatabase::rollback();
```

### 2.2QSqlQuery

`QSqlQuery`封装了从`QSqlDatabase`上执行的SQL查询中创建、导航和检索数据所涉及的功能。既可以执行`SELECT`、`INSERT`、`UPDATE`、`DELETE`等DML(数据操作语言)语句，也可以执行`CREATE TABLE`等DDL(数据定义语言)语句。

**常用成员函数**

构造函数：

```C++
QSqlQuery::QSqlQuery(
    const QString &query = QString(), 
    QSqlDatabase db = QSqlDatabase());
query：要执行的SQL语句
db：数据库实例对象，如果没有指定db，或者是无效的，则使用应用程序的默认数据库。
```

执行一个SQL语句

```c++
bool QSqlQuery::exec();    // ①
bool QSqlQuery::exec(const QString &query);   // ②
①：没有参数，执行的SQL语句是在构造函数中指定的。
②：有参数，参数对应的字符串就是要执行的SQL语句。
```

检索查询得到的结果集中的下一条记录(如果可用)，并将查询定位到检索到的记录上。

```C++
bool QSqlQuery::next();

该函数检索结果集中的每一条记录的规则如下:

如果当前位于结果集第一个记录之前，例如，在执行查询之后，将尝试检索第一个记录。

如果当前位于结果集最后一条记录之后，结果集已经检索完毕并返回false。

如果结果位于结果集中间的某个位置，则尝试检索下一个记录。

函数调用之后，如果检索到有效记录返回true，否则返回false。
```

获取当前记录中字段的值

```C++
QVariant QSqlQuery::value(int index) const;  // ①
// 通过字段名直接取出字段值
QVariant QSqlQuery::value(const QString &name) const; // ②

①：通过字段的索引得到当前字段的值，编号从0开始。
②：通过字段的名字得到当前字段的值。
```

由于数据库表中的字段可以有多种数据类型，因此将这多种类型通过QVariant类进行了包装从而实现了整齐划一。我们可以通过调用`QVariant类`的API函数得到其内部实际类型的数据。

## 3.实例代码

```C++
#include "widget.h"
#include "ui_widget.h"

#include <QWidget>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    QStringList list = QSqlDatabase::drivers();
    qDebug() << list;

    db.setHostName("47.108.58.186");
    db.setUserName("root");
    db.setDatabaseName("123");
    db.setPassword("3321");
    db.setPort(3306);


    //连接数据库服务器
    if(!db.open())
    {
        qDebug() << "失败原因：" << db.lastError().text();
    }
    else
    {
        qDebug() << "连接成功！！！";
    }


    QSqlQuery query;
    //事务 对数据库进行写操作
    QString sql = "insert into students values(123,'man',20)";
    //创建一个事务
    db.transaction();
    bool flag = query.exec(sql);
    if(flag)
    {
        db.commit();
    }
    else
    {
        db.rollback();
    }



    //执行sql语句

    sql = "select * from students";
    query.exec(sql);

    while (query.next()) {
        //取出每个字段的值
        qDebug() << query.value(0).toUInt()
                 << query.value("name").toString()
                 << query.value(2).toUInt();

    }

    db.close();

}

Widget::~Widget()
{
    delete ui;
}


```

