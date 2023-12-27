#include<iostream>
#include<bits/stdc++.h>
#include"user.pb.h"
#include"mprpcapplication.h"
#include"rpcprovider.h"
#include"mprpcconfig.h"
using namespace std;
using namespace fixbug;

//UserService原本是一个本地服务，提供了两个进程内的本地方法Login和GetFriendLists
class UserService:public UserServiceRpc{    //rpc服务的提供者callee
public:
    bool Login(string name, string pwd){
        cout<<"Login"<<endl;
        cout<<"name:"<<name<<" pwd:"<<pwd<<endl;
        return true;
    }

    bool Register(uint32_t id, string name, string pwd){
        cout<<"Register"<<endl;
        cout<<"id:"<<id<<"name:"<<name<<" pwd:"<<pwd<<endl;
        return true;
    }
    /*重写基类UserServiceRpc的虚函数 下面的方法由框架直接调用
    1. caller ===>Login(LoginRequest) => muduo => callee
    2. callee ===>Login(LoginRequest) => 交到下面重写的这个Login方法上了
    */
    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done){
        string name = request->name();
        string pwd = request->pwd();
        bool login_result = Login(name, pwd);   //做本地业务
        //返回响应
        ResultCode *resultcode =response->mutable_result();
        resultcode->set_errorcode(0);
        resultcode->set_errormsg("");
        response->set_success(login_result);
        //执行回调操作 执行响应消息的序列化和发送
        done->Run();
    }

    void Register(::google::protobuf::RpcController* controller,
                       const ::fixbug::RegisterRequest* request,
                       ::fixbug::RegisterResponse* response,
                       ::google::protobuf::Closure* done){
        uint32_t id = request->id();
        string name = request->name();
        string pwd = request->pwd();    
        bool register_result = Register(id, name, pwd);   //做本地业务
        //返回响应
        ResultCode *resultcode =response->mutable_result();
        resultcode->set_errorcode(0);
        resultcode->set_errormsg("");
        response->set_success(register_result);
        //执行回调操作 执行响应消息的序列化和发送
        done->Run();
    }
};

int main(int argc, char **argv){

    //调用框架的初始化操作 provider -i config.conf
    MprpcApplaction::Init(argc, argv); 
    
    //provider是一个rpc网络服务对象 把UserService发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new UserService());

    //启动rpc服务节点
    provider.Run();
    return 0;
}