#include"rpcprovider.h"
#include"mprpcapplication.h"
#include"logger.h"
#include<rpcheader.pb.h>
#include"zookeeperutil.h"

void RpcProvider::NotifyService(google::protobuf::Service *service){

    serviceInfo service_info;

    //获取了服务对象的描述信息
    const google::protobuf::ServiceDescriptor *pserviceDesc=service->GetDescriptor();
    //获取服务的名字
    std::string service_name = pserviceDesc->name();
    std::cout<<"service_name:"<<service_name<<std::endl;
    //获取服务对象的service的方法的数量
    int methodCnt =pserviceDesc->method_count();
    for(int i=0;i<methodCnt;i++){ 
        //获取服务对象指定下标的方法对象的描述
        const google::protobuf::MethodDescriptor* pmethodDesc = pserviceDesc->method(i);
        std::string method_name =pmethodDesc->name();
        LOG_INFO("method_name: %s", method_name.c_str());
        service_info.m_methodMap.insert({method_name, pmethodDesc});
    }
    service_info.m_service=service;
    m_serviceMap.insert({service_name, service_info});

}

void RpcProvider::Run(){
    std::string ip=MprpcApplaction::GetInstance().GetConfig().Load("rpcserver_ip");
    uint16_t port=atoi(MprpcApplaction::GetInstance().GetConfig().Load("rpcserver_port").c_str());
    muduo::net::InetAddress address(ip, port);
    //创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");
    //绑定连接断开回调
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    //绑定读写回调
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
    //设定muduo库的线程数量
    server.setThreadNum(4);

    //把当前rpc节点上要发布的服务注册到zk上，让rpc client可以从zk上发现服务
    ZkClient zkCli;
    zkCli.Start();

    for(auto &sp:m_serviceMap){
        //service_name
        std::string service_path = "/"+sp.first;
        zkCli.Creat(service_path.c_str(),nullptr,0);
        for(auto &mp: sp.second.m_methodMap){
            //service_name/method_name
            std::string method_path = service_path+"/"+mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data,"%s:%d",ip.c_str(), port);
            //ZOO_EPHEMERAL表示znode为临时节点
            zkCli.Creat(method_path.c_str(),method_path_data, strlen(method_path_data),ZOO_EPHEMERAL);
        }
    }



    std::cout<<"RpcProvider start service at ip:"<<ip<<" port:"<<port<<std::endl;
    server.start();
    m_eventLoop.loop();
} 
 
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn){
    if(!conn->connected()){
        //rpc_server和rpc_client的连接断开了
        conn->shutdown();   
    }
}

/*
在框架内部，rpcprovider和rpcconsumer要协商好之间通信用的protobuf数据类型

header由三部分组成
service name
method_name
args_size (防止tcp粘包问题，即args后面紧跟了下一个service_name)!!!!!


传入的数据
header_size(4个字节) service name+method_name+args_size的长度
header 由service name+method_name+args_size组成
args_str 



*/
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn,
                            muduo::net::Buffer* buffer,
                            muduo::Timestamp){
    //网络上接受的远程rpc调用请求的字符流 包含 函数名 参数
    std::string rev_buf = buffer->retrieveAllAsString();    

    //从字符流中读取前4个字节内容                        
    uint32_t header_size=0;
    rev_buf.copy((char*)&header_size, 4, 0);

    //根据head_size读取数据头的原始字节流
    std::string rpc_header_str = rev_buf.substr(4,header_size);
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    //反序列化数据，得到rpc请求的详细信息
    mprpc::RpcHeader rpcheader;
    if(rpcheader.ParseFromString(rpc_header_str)){
        //数据头反序列化成功
        service_name=rpcheader.service_name();
        method_name=rpcheader.method_name();
        args_size=rpcheader.args_size();
    }
    else{
        //数据头反序列化失败
        std::cout<<"rpc_header_str:"<<rpc_header_str<<" paser error!"<<std::endl;
    }
    
    std::string args_str = rev_buf.substr(4+header_size,args_size);
    //打印调试信息
    std::cout<<"==================================================================="<<std::endl;
    std::cout<<"header_size: "<<header_size<<std::endl;
    std::cout<<"rpc_header_str: "<<rpc_header_str<<std::endl;
    std::cout<<"service_name: "<<service_name<<std::endl;
    std::cout<<"method_name: "<<method_name<<std::endl;
    std::cout<<"args_size: "<<args_size<<std::endl;
    std::cout<<"args_str: "<<args_str<<std::endl;
    std::cout<<"==================================================================="<<std::endl;

    //获取service和method对象
    auto it=m_serviceMap.find(service_name);
    if(it==m_serviceMap.end()){
        std::cout<<service_name<< "does not exist"<<std::endl;
        return;
    }
    google::protobuf::Service *service=it->second.m_service;
    auto me = it->second.m_methodMap.find(method_name);

    if(me==it->second.m_methodMap.end()){
        std::cout<<method_name<< "does not exist"<<std::endl;
        return;
    }
    const google::protobuf::MethodDescriptor *method=me->second;

    //生成rpc方法调用请求request和响应response参数
    //这里生成的request和response都是调用method对应的样子
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(args_str)){
        std::cout<<"args_str:"<<args_str<<" paser error!"<<std::endl;
        return;
    }
    request->ParseFromString(args_str);
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    //给下面metho方法的调用，绑定一个Closure回调函数
     google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>(this, &RpcProvider::SendRpcResponse, conn, response);

    //在框架上根据远端rpc请求，调用当前rpc节点上发布的方法
    //new  UserService.Login(controller, request, response, done) 
    service->CallMethod(method, nullptr, request, response,done);
    }


    //Closure的回调函数，用于序列化rpc的响应和网络发送
    void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response){
        std::string response_str;
        if(response->SerializeToString(&response_str)){  //response进行序列化
            //序列化成功后，rpc通过网络通过结果发回调用方
            conn->send(response_str); 
            conn->shutdown();   //模拟Http的短链接服务,由rpcprovider主动断开连接
        }
        else{
            std::cout<<"seralize response_str error!"<<std::endl;
             conn->shutdown();
        }
    }