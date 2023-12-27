//确保头文件只被编译一次
#pragma once
#include "google/protobuf/service.h"
#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<muduo/net/InetAddress.h>
#include<muduo/net/TcpConnection.h>
#include<iostream>
#include<functional>
#include<google/protobuf/descriptor.h>
#include<bits/stdc++.h>

//mprpc框架初始化类
class  RpcProvider{
public:
    //虚函数与重写的理解！！
    //本地方法注册为rpc方法 
   void NotifyService(google::protobuf::Service *service);
   //启动rpc服务节点，远程提供rpc服务
   void Run();

private:
    //智能指针！！！！！
    std::unique_ptr<muduo::net::TcpServer> m_tcpserverPter;
    muduo::net::EventLoop m_eventLoop; 
    struct serviceInfo{
        google::protobuf::Service *m_service;   //保存服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap;   //保存服务对象中的服务方法
    };
    //存储注册成功的服务对象及服务方法信息
    std::unordered_map<std::string, serviceInfo> m_serviceMap;

    void OnConnection(const muduo::net::TcpConnectionPtr& conn);
    void OnMessage(const muduo::net::TcpConnectionPtr&,
                            muduo::net::Buffer*,
                            muduo::Timestamp);
    //Closure的回调操作，用于序列化Rpc的响应和网络发送
    void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response);
};