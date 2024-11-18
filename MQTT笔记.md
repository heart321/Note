# MQTTClient应用

1. 连接MQTT服务器，

   ```c++
   //连接mqtt服务器
   void Widget::on_Button_Connect_clicked()
   {
       //连接之前判断mqtt服务器未连接
       if(Client->state() == QMqttClient::Disconnected)
       {
           //设置服务器IP
           Client->setHostname(ui->MQTT_IP->text());
           //设置端口号
           Client->setPort(static_cast<quint16>(ui->MQTT_Port->text().toUInt()));//进行格式转换
           //设置用户名
           if(!ui->MQTT_UseName->text().isNull())//用户名不为空
           {
               Client->setUsername(ui->MQTT_UseName->text());
           }
           //设置密码
           if(!ui->MQTT_Password->text().isNull())//密码不为空
           {
               Client->setPassword(ui->MQTT_Password->text());
           }
           
          //用于指定客户端与代理断开连接时是否清除之前的会话状态。
          //当"Clean Session"设置为true时，表示每次连接都是一个新的会话，之前的订阅、发布等状态都会被清除。
          //当"Clean Session"设置为false时，表示客户端与代理断开连接后，会话状态会被保留，包括之前的订阅和发布消息。
           Client->setCleanSession(false);
           
           //设置心跳时间间隔120S
           Client->setKeepAlive(120);
           //设置mqtt版本
           Client->setProtocolVersion(QMqttClient::ProtocolVersion::MQTT_3_1_1);
           //连接mqtt服务器
           Client->connectToHost();
           //等待服务器连接成功
           loop.exec();
       }
   }
   ```

   

2. Lambda表达式

   ```c++
   [capture](params) opt -> ret {body;};
       - capture: 捕获列表
       - params: 参数列表
       - opt: 函数选项(可省略）
       - ret: 返回值类型（可省略）
       - body: 函数体
   capture : 捕获一定范围内的变量
   [] - 不捕捉任何变量
   [&] - 捕获外部作用域中所有变量, 并作为引用在函数体内使用 (按引用捕获)
   [=] - 捕获外部作用域中所有变量, 并作为副本在函数体内使用 (按值捕获):拷贝的副本在匿名函数体内部是只读的
   [=, &foo] - 按值捕获外部作用域中所有变量, 并按照引用捕获外部变量 foo
   [bar] - 按值捕获 bar 变量, 同时不捕获其他变量
   [&bar] - 按引用捕获 bar 变量, 同时不捕获其他变量
   [this] - 捕获当前类中的this指针
   //通常常用[=]
   
   opt :（可以省略）
   mutable: 可以修改按值传递进来的拷贝（注意是能修改拷贝，而不是值本身）
   exception: 指定函数抛出的异常，如抛出整数类型的异常，可以使用throw();
   
   connect(Client,&QMqttClient::disconnected,[&](){
           ui->textEdit_Rev->append("MQTT服务器断开！");
       });//Lambda表达式连接断开槽函数
   ```

3. 订阅主题

   ```C++
   //订阅消息 topic:订阅消息的主题 qos:订阅的等级
   void Widget::Subscribe(const QString &topic, int qos)
   {
       QMqttSubscription *substate = Client->subscribe(QMqttTopicFilter(topic),qos);
       QTimer time;
       int timerout = 50;
       connect(&time,&QTimer::timeout,&loop,&QEventLoop::quit);
       time.start(timerout);
       loop.exec();
       
       if(substate->state() == QMqttSubscription::Subscribed)
       {
           ui->textEdit_Rev->append(QString("Subscribe:%1 订阅成功").arg(topic));
       }
         
   }
   
   //设置订阅主题的类内函数
   void Widget::Set_Subtopic(QString topic)
   {
       this->Sub_Topic = topic;
   }
   
   
   //订阅主题的槽函数
   void Widget::on_Button_SubTopic_clicked()
   {
       if(Client->state() == QMqttClient::Connected)
       {
           Set_Subtopic(ui->lineEdit_SubTopic->text());
           Subscribe(this->Sub_Topic,0);
       }
       else
       {
           ui->textEdit_Rev->append("未连接服务器！");
       }
   }
   
   ```

   

4. 发布主题

   ```C++
   /*发布消息
    * topic:发布消息的主题
    * message:发布的消息
    * qos:发布的等级
    */
   int Widget::Publish(const QString &topic, const QString &message, int qos)
   {
       auto result = Client->publish(QMqttTopicName(topic),message.toUtf8(),qos,true);
       return result;
   }
   
   //发布主题的类内函数
   void Widget::Set_Pubtopic(QString topic)
   {
       this->Pub_Topic = topic;
   }
   
   //发布主题的槽函数
   void Widget::on_Button_PubTopic_2_clicked()
   {
       if(Client->state() == QMqttClient::Connected)
       {
           Set_Pubtopic(ui->lineEdit_PubTopic->text());
           int id = Publish(Pub_Topic,"",0);
           if(id >= 0)
           {
               ui->textEdit_Rev->append("*********************************************");
               ui->textEdit_Rev->append(QString("PUBTopic: %1").arg(Pub_Topic));
               ui->textEdit_Rev->append("*********************************************");
           }
           else
           {
               ui->textEdit_Rev->append("未连接服务器！");
           }
       }
   }
   
   //发送消息的槽函数
   void Widget::on_Button_SendData_clicked()
   {
       if(Client->state() == QMqttClient::Connected)
       {
           Set_Pubtopic(ui->lineEdit_PubTopic->text());
           int id = Publish(Pub_Topic,ui->lineEdit_SendText->text(),0);
           if(id >= 0)
           {
               ui->textEdit_Rev->append("*********************************************");
               ui->textEdit_Rev->append(QString("PUBTopic: %1\namessage: %2").arg(Pub_Topic).arg(ui->lineEdit_SendText->text()));
               ui->textEdit_Rev->append("*********************************************");
           }
           else
           {
               ui->textEdit_Rev->append("未连接服务器！");
           }
       }
   }
   
   //接收消息并显示的槽函数
   void Widget::MQTT_RevData_Success(const QByteArray &message, const QMqttTopicName &topic)
   {
       QTextCodec *Code = QTextCodec::codecForName("GBK");//编码格式选择GBK
       ui->textEdit_Rev->append("******************************************************");
       ui->textEdit_Rev->append(QString("Topic: %1 \MESSAGE: %2").arg(topic.name()).arg(Code->toUnicode(message)));
       ui->textEdit_Rev->append("******************************************************");
   }
   
   ```

5. 总结

   先编译mqtt库文件，将mqtt库移植进Qt工程中(因为Qt本身并没有mqtt的库)。接下来就是利用QMqttClinet类实现连接服务器，发布订阅，发送消息等功能就行。
