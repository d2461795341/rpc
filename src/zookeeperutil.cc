#include"zookeeperutil.h"
#include"mprpcapplication.h"
#include<semaphore.h>
#include<iostream>

void global_watcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx){
    if(type==ZOO_SESSION_EVENT){    //和会话相关的消息类型
        if(state==ZOO_CONNECTED_STATE){
            sem_t *sem = (sem_t*)zoo_get_context(zh);
            sem_post(sem); 
        }
    }
}


ZkClient::ZkClient():m_zhandle(nullptr){

}
ZkClient::~ZkClient(){
    if(m_zhandle){
        zookeeper_close(m_zhandle); //关闭句柄，关闭资源
    }
}
void ZkClient::Start(){
    /*
    std::string host = MprpcApplaction::GetInstance().GetConfig().Load("zookeeper_ip");
    std::string port = MprpcApplaction::GetInstance().GetConfig().Load("zookeeper_port");
    std::string connstr = host+":"+port;
    */
   std::string connstr = "192.168.109.130:2181";
    /*
    zookeeper_mt:多线程版本
    zookeeper的API客户端程序提供了三个线程
    1.API调用线程 zookeeper_init
    2.网络I/O线程 发送心跳
    3.watcher回调线程 global_watcher
    */
    m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if(!m_zhandle){
        std::cout<<"zookeeper_init error!"<<std::endl;
        exit(EXIT_FAILURE);
    }

    sem_t sem;
    sem_init(&sem,0,0);
    zoo_set_context(m_zhandle,&sem);

    sem_wait(&sem);
    std::cout<<"zookepper_init success!"<<std::endl;
    zookeeper.set_debug_level(zookeeper.LOG_LEVEL_ERROR) //设置zookeeper的日志等级

}
void ZkClient::Creat(const char*path, const char *data, int datalen, int state){
    char path_buffer[128];
    int bufferlen=sizeof(path_buffer);
    int flag;
    flag = zoo_exists(m_zhandle, path, 0, nullptr);
    if(ZNONODE == flag){
        flag = zoo_create(m_zhandle, path, data, datalen, &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufferlen);
    }
    if(flag==ZOK){
        std::cout<<"znode create success... path:"<<path<<std::endl;
    }
    else{
        std::cout<<"flag:"<<flag<<std::endl;
        std::cout<<"znode create error... path:"<<path<<std::endl;
        exit(EXIT_FAILURE);
    }
}

//根据指定的path，获取znode节点的值
std::string ZkClient::GetData(const char* path){
    char buffer[64];
    int bufferlen = sizeof(buffer);
    int flag = zoo_get(m_zhandle, path,0, buffer, &bufferlen, nullptr);
    if(flag!=ZOK){
        std::cout<<"get znode error... path:"<<path<<std::endl;
        return "";
    }
    else{
        return buffer;
    }

}