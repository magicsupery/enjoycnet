## 项目简介

* 基于enjoycco的协程网络库，用于通过扩展proto和codec来实现丰富的同步网络库功能；
* 目前默认提供了TCP支持和简单的HttpCodec


## 压测情况（HTTPServer）
* 服务器环境--linux虚拟机
    8核
    vendor_id	: GenuineIntel
    cpu family	: 6
    model		: 42
    model name	: Intel Xeon E312xx (Sandy Bridge)
    stepping	: 1
    microcode	: 0x1
    cpu MHz		: 1999.999
    cache size	: 4096 KB
    
* 结果
QPS：40w


