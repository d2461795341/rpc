//确保头文件只被编译一次
#pragma once
#include<google/protobuf/service.h>
#include<google/protobuf/message.h>
#include<google/protobuf/descriptor.h>

class MyrpcChannel:public google::protobuf::RpcChannel{
public:
    //所有通过stub代理对象的rpc方法，都走到这里，统一做rpc方法调用的数据序列化和网络发送
    void CallMethod(const google::protobuf::MethodDescriptor* method,
                            google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                            google::protobuf::Message* response, google::protobuf::Closure* done);
private:
};