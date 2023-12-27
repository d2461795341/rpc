#include<iostream>
#include<bits/stdc++.h>
#include"friend.pb.h"
#include"mprpcapplication.h"
#include"rpcprovider.h"
#include"mprpcconfig.h"
#include"logger.h"
using namespace std;
using namespace fixbug;

class FriendService:public FriendServiceRpc{    //rpc服务的提供者callee
public:
    std::vector<std::string> GetFriendList(uint32_t id){
        std::vector<std::string> friendlist;
        friendlist.push_back("zhang san");
        friendlist.push_back("li si");
        friendlist.push_back("wang wu");
        return friendlist;
    }

    
    void GetFriendList(::google::protobuf::RpcController* controller,
                       const ::fixbug::GetFriendListRequest* request,
                       ::fixbug::GetFriendListResponse* response,
                       ::google::protobuf::Closure* done){
        uint32_t id = request->id();
        std::vector<std::string> friendlist = GetFriendList(id);   //做本地业务
        //返回响应
        ResultCode *resultcode =response->mutable_result();
        resultcode->set_errorcode(0);
        resultcode->set_errormsg("");
        for(auto i: friendlist){
            string* tmp =response->add_friendlist();
            *tmp=i;
        }
        //执行回调操作 执行响应消息的序列化和发送
        done->Run();
    }

};

int main(int argc, char **argv){

    LOG_INFO("first log message!");
    LOG_ERRO("%s:%s:%d",__FILE__,__FUNCTION__,__LINE__);
    //调用框架的初始化操作 provider -i config.conf
    MprpcApplaction::Init(argc, argv); 
    
    //provider是一个rpc网络服务对象 把FriendService发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new FriendService());

    //启动rpc服务节点
    provider.Run();
    return 0;
}