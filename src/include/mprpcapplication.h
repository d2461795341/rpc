//确保头文件只被编译一次
#pragma once
#include<mprpcconfig.h>

//mprpc框架初始化类
class MprpcApplaction{
public:
    static void Init(int argc, char **argv);
    static MprpcApplaction& GetInstance();
    static MprpcConfig& GetConfig();
private:
    static MprpcConfig m_config;
    MprpcApplaction(){};
    MprpcApplaction(const MprpcApplaction&) = delete;
    MprpcApplaction(MprpcApplaction&&) = delete;
};