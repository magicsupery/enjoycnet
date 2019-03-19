## 项目简介

* 基于libgo+asio的协程网络库，支持tcp，udp，kcp，http，ssl等协议，并且协议是可以扩展的；
* 通过option进行配置，用来配置网络参数以及stream流的变更；
* 通过handler处理socket流的数据，用户可以定义自己的handler
