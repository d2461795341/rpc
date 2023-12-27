#include<iostream>
#include"mprpcapplication.h"
#include"user.pb.h"
#include"mprpcchannel.h"
#include"mprpccontroller.h"


using namespace fixbug;
int main(int argc, char** argv){
    //整个程序启动后，想使用mprpc框架来享受rpc服务调用，一定需要先调用框架初始化函数；
    MprpcApplaction::Init(argc,argv);


    //演示调用远程发布的rpc方法Login
    fixbug::UserServiceRpc_Stub stub(new MyrpcChannel());   
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    fixbug::LoginResponse response;
    MprpcController controller;
    stub.Login(&controller, &request, &response, nullptr);   //rpcchannel来做所有rpc方法的参数序列化和网络发送
    //一次rpc调用完成，读调用的结果
    if(response.result().errorcode()==0){
        std::cout<<"rpc login response:"<<response.success()<<std::endl; 
    }   
    else{
        std::cout<<"rpc login error:"<<response.result().errormsg()<<std::endl; 
    }

    //演示调用远程发布的rpc方法Login
    fixbug::RegisterRequest req;
    req.set_id(2000);
    req.set_name("li si");
    req.set_pwd("77777");
    fixbug::RegisterResponse res;
    stub.Register(nullptr, &req, &res, nullptr);   //rpcchannel来做所有rpc方法的参数序列化和网络发送
    //一次rpc调用完成，读调用的结果
    if(controller.Failed()){
        std::cout<<controller.ErrorText()<<std::endl;
    }
    else{
        if(res.result().errorcode()==0){
            std::cout<<"rpc register response:"<<res.success()<<std::endl; 
        }   
        else{
            std::cout<<"rpc register error:"<<res.result().errormsg()<<std::endl; 
        }
    }
    return 0;


}