//确保头文件只被编译一次
#pragma once
#include<bits/stdc++.h>
#include<iostream>

//rpcserver_ip rpcserver_port zookeeper_ip zookeeper_port
//框架读取配置类
class MprpcConfig{
public:
    //解析加载配置文件
    void LoadConfigFile(const char* config_file);
    //查询配置项信息
    std::string Load(std::string key);
private:
    std::unordered_map<std::string, std::string> um_config;
    void Trim(std::string &src_buf);
};