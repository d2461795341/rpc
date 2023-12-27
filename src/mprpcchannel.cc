#include<mprpcchannel.h>
#include<rpcheader.pb.h>
#include<mprpcapplication.h>
#include<iostream>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<errno.h>
#include<netinet/in.h>
#include<unistd.h>
#include"mprpccontroller.h"
#include"zookeeperutil.h"
/*
header_size(4个字节) service name+method_name+args_size的长度
header 由service name+method_name+args_size组成
args_str 
*/
void MyrpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                            google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                            google::protobuf::Message* response, google::protobuf::Closure* done){
    const google::protobuf::ServiceDescriptor* service = method->service();
    std::string service_name= service->name();
    std::string method_name= method->name();

    //获取序列化参数字符串的长度 args_size
    uint32_t args_size;
    std::string args_str;
    if(request->SerializeToString(&args_str)){
        args_size = args_str.size();
    }
    else{
        controller->SetFailed("serialize request error!");
        return;
    }
    //定义rpc的请求header
    mprpc::RpcHeader rpcheader;
    rpcheader.set_service_name(service_name);
    rpcheader.set_method_name(method_name);
    rpcheader.set_args_size(args_size);
    
    u_int32_t header_size;
    std::string rpc_header_str;
    if(rpcheader.SerializeToString(&rpc_header_str)){
        header_size=rpc_header_str.size();
    }
    else{
        controller->SetFailed("serialize rpcheader error!");
        return;
    }
    //组织待发送的rpc请求字符串
    /*
        header_size(4个字节) service name+method_name+args_size的长度
        header 由service name+method_name+args_size组成
        args_str 
    */
    std::string send_rpc_str;
    send_rpc_str.insert(0,std::string((char*)&header_size,4)); //header_size(4个字节)
    send_rpc_str.append(rpc_header_str);
    send_rpc_str.append(args_str);
    std::cout<<"==================================================================="<<std::endl;
    std::cout<<"header_size: "<<header_size<<std::endl;
    std::cout<<"rpc_header_str: "<<rpc_header_str<<std::endl;
    std::cout<<"service_name: "<<service_name<<std::endl; 
    std::cout<<"method_name: "<<method_name<<std::endl;
    std::cout<<"args_size: "<<args_size<<std::endl;
    std::cout<<"args_str: "<<args_str<<std::endl;
    std::cout<<"==================================================================="<<std::endl;

    //使用tcp编程， 完成rpc方法的远程调用
    int clientfd=socket(AF_INET,SOCK_STREAM,0);
    if(-1==clientfd){
        char errtxt[512]={0};
        sprintf(errtxt, "create socket error! errno: %d",errno);
        controller->SetFailed(errtxt);
        return;
    }
    //读取配置文件rpcserver的信息
    // std::string ip=MprpcApplaction::GetInstance().GetConfig().Load("rpcserver_ip");
    // uint16_t port=atoi(MprpcApplaction::GetInstance().GetConfig().Load("rpcserver_port").c_str());

    ZkClient zkCli;
    zkCli.Start();
    std::string method_path = "/" + service_name+"/"+method_name;
    std::string host_data = zkCli.GetData(method_path.c_str());
    if(host_data == ""){
         controller->SetFailed(method_path+"is not exist!");
         return;
    }
    int idx = host_data.find(":");
    if(idx==-1){
        controller->SetFailed(method_path+"address not exist!");
        return;
    }
    std::string ip = host_data.substr(0,idx);
    uint16_t port = atoi(host_data.substr(idx+1, host_data.size()-idx).c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port= htons(port);
    server_addr.sin_addr.s_addr= inet_addr(ip.c_str());

    //连接rpc服务节点
    if(-1==connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr))){
        char errtxt[512]={0};
        sprintf(errtxt, "connect error! errno: %d",errno);
        controller->SetFailed(errtxt);
        close(clientfd);
        return;
    }

    //发送rpc请求
    if(-1==send(clientfd,send_rpc_str.c_str(),send_rpc_str.size(),0)){
        char errtxt[512]={0};
        sprintf(errtxt, "send error! errno: %d",errno);
        controller->SetFailed(errtxt);
       close(clientfd);
       return;
    }

    //接收rpc请求的响应值
    char recv_buf[1024]={0};
    int recv_size =0;
    if(-1==(recv_size=recv(clientfd,recv_buf,1024,0))){
        char errtxt[512]={0};
        sprintf(errtxt, "receive error! errno: %d",errno);
        controller->SetFailed(errtxt);
        close(clientfd);
        return;
    }
    //反序列化rpc调用的响应结果
    //std::string response_str(recv_buf,0,recv_size); bug出现问题，recv_buf中遇到\0后面的数据就存不下来了
    if(!response->ParseFromArray(recv_buf,recv_size)){
        char errtxt[1055]={0};
        sprintf(errtxt, "parse error! response_str: %s",recv_buf);
        controller->SetFailed(errtxt);
        close(clientfd);
        return;
    }
    close(clientfd);
};