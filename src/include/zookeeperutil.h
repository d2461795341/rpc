#pragma once

#include<semaphore.h>
#include<zookeeper/zookeeper.h>
#include<string>

//封装的zk客户端类
class ZkClient{
public:
    ZkClient();
    ~ZkClient();
    void Start();
    void Creat(const char*path, const char *data, int datalen, int state=0);
    std::string GetData(const char* path);
private:
    //zk客户端的句柄
    zhandle_t* m_zhandle;

};