#include "mprpcapplication.h"
#include<iostream>
#include<getopt.h>
#include<bits/stdc++.h>


MprpcConfig MprpcApplaction::m_config;

void ShowArgsHelp(){
    std::cout<<"format: command -i <configfile>"<<std::endl;
}
void MprpcApplaction::Init(int argc, char **argv){
     if(argc<2){
        ShowArgsHelp();
        exit(EXIT_FAILURE);
     }
     int c=0;
     std::string config_file;
     //如果你的程序还期望处理 -h（无参数的选项），你可以将字符串设置为 "i:h"
     while((c=getopt(argc, argv, "i:"))!=-1){
        switch (c)
        {
        //config_file的值为<configfile>
        case 'i':
            config_file =optarg;
            break;
        //没有找到 -i
        case '?':
            ShowArgsHelp();
            break;
        //-i后面的值为空
        case ':':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
     }
     //开始加载配置文件 rpcserver_ip rpcserver_port zookeeper_ip zookeeper_port
     m_config.LoadConfigFile(config_file.c_str());
     std::cout<<"rpcserver_ip:"<<m_config.Load("rpcserver_ip")<<std::endl;
     std::cout<<"rpcserver_port:"<<m_config.Load("rpcserver_port")<<std::endl;
     std::cout<<"zookeeper_ip:"<<m_config.Load("zookeeper_ip")<<std::endl;
     std::cout<<"zookeeper_port:"<<m_config.Load("zookeeper_port")<<std::endl;
}
MprpcApplaction& MprpcApplaction:: GetInstance(){
    static MprpcApplaction app;
    return app;
}
MprpcConfig& MprpcApplaction::GetConfig(){
    return m_config;
}