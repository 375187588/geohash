# GeoHash

## 简介

通过Http接口实现实时位置更新和查询的GeoHash，支持区域查询，支持当前点更新。(默认支持100万个定位对象)
支持加载插件实现数据初始化过程。不必修改相应的框架代码。

## 特性

实现实时位置更新的GeoHash实现，支持区域查询，支持当前点更新。
利用共享内存实现，万一进程挂掉，重启进程数据直接读取共享内存，不会丢失，毫秒级加载。
如果想使用，请自己实现LoadModule子目录的数据加载行为。
具体请参考LoadModule子目录下的LoadModule.cpp
这样就不需要你修改任何框架代码的情况下，正常使用此服务。

第一次启动进程，进程会调用这个插件实现共享内存初始化，如果共享内存已经存在，则什么都不做。

##编译生成
打开LoadModule
运行 
make
然后在根目录
make
最后检查在build子目录下
查看
geoarea.ini  libgeoload.so  mapchinapos
这三个文件是否已经存在
完成后运行mapchinapos即可。

## 用法

###往当前数据中添加当前点

curl -XPOST "http://127.0.0.1:10000/GeoHash/Add/" -d "{\"Msisdn\":\"13412345637\",\"Latitude\":\"39.928667\",\"Longitude\":\"116.389550\",\"Time\":\"1486447081\"}"

###查询区域中的所有当前点

curl -XPOST "http://127.0.0.1:10000/GeoHash/Search/" -d "{\"Latitude\":\"39.928367\",\"Longitude\":\"116.389550\",\"Radius\":\"100000.0\"}"

###删除一个区域中的指定点

curl -XPOST "http://127.0.0.1:10000/GeoHash/Delete/" -d "{\"Msisdn\":\"13412345637\"}"

###查询当前共享内存中对象使用情况
curl -XPOST "http://127.0.0.1:10000/GeoHash/Pool/" -d "{\"aaa\":\"1\"}"

##插件开发指南

运行插件解决两个问题。
* (1)当共享内存第一次创建，从其他的数据源填充数据到当前内存。也就是LoadModuleData()函数，在这里实现数据加载。
* (2)独立的Runtime_Thread()函数，这里可以做进程监控，也可以在里面进行一些实时的数据加载，比如从kafka取得当前定位消息后台直接入库。不走http服务。

