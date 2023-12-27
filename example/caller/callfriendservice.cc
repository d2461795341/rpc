#include<iostream>
#include"mprpcapplication.h"
#include"friend.pb.h"
#include"mprpcchannel.h"
#include"mprpccontroller.h"

using namespace fixbug;
int main(int argc, char** argv){
    //整个程序启动后，想使用mprpc框架来享受rpc服务调用，一定需要先调用框架初始化函数；
    MprpcApplaction::Init(argc,argv);


    //演示调用远程发布的rpc方法Login
    fixbug::FriendServiceRpc_Stub stub(new MyrpcChannel());   
    fixbug::GetFriendListRequest request;

    request.set_id(500);
    fixbug::GetFriendListResponse response;

    MprpcController controller;
    stub.GetFriendList(&controller, &request, &response, nullptr);   //rpcchannel来做所有rpc方法的参数序列化和网络发送
    //一次rpc调用完成，读调用的结果
    if(controller.Failed()){
        std::cout<<controller.ErrorText()<<std::endl;
    }
    else{
        if(response.result().errorcode()==0){
            for(int i=0;i<response.friendlist_size();i++)
            std::cout<<"index: "<<(i+1)<<" name: "<<response.friendlist()[i]<<std::endl; 
        }   
        else{
            std::cout<<"rpc login error:"<<response.result().errormsg()<<std::endl; 
        }
    }

    return 0;


}