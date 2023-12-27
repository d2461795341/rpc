//确保头文件只被编译一次
#pragma once
#include<bits/stdc++.h>
#include<iostream>
#include<google/protobuf/service.h>


class MprpcController:public google::protobuf::RpcController{
public:
    MprpcController();
    void Reset();
    bool Failed() const;
    std::string ErrorText() const;
    void SetFailed(const std::string& reason);

    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure* callback);

private:
    bool m_failed; //RPC方法执行过程中的状态
    std::string m_errText; //RPC方法执行过程中的错误信息
};