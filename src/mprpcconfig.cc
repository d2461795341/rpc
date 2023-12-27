#include<mprpcconfig.h>

//解析加载配置文件
void MprpcConfig::LoadConfigFile(const char* config_file){
    FILE *pf=fopen(config_file, "r");
    if(pf==nullptr){
        std::cout<<config_file<<"does not exist"<<std::endl;
        exit(EXIT_FAILURE);
    }
    //1.注释 2.正确的配置项 3.去掉开头的空格
    while ((!feof(pf)))
    {
        char buf[512]={0};
        fgets(buf,512,pf);
        //去掉字符串前多余空格
        std::string read_buf(buf);
        Trim(read_buf);
        //判断#注释
        if(read_buf[0]=='#'||read_buf.empty()){
            continue;
        }

        //解析配置项
        int idx=read_buf.find('=');
        if(idx==-1){
            continue;
        }
        std::string key;
        std::string value;
        key=read_buf.substr(0,idx);
        Trim(key);
        int end_idx=read_buf.find('\n',idx);
        value=read_buf.substr(idx+1,end_idx-idx-1);
        Trim(value);
        um_config.insert({key,value});
    }
    
}
//查询配置项信息
std::string MprpcConfig::Load(std::string key){
    if(um_config.find(key)!=um_config.end()){
        return um_config[key];
    }
    else{
        return "";
    }
}

void MprpcConfig::Trim(std::string &src_buf){
   int idx = src_buf.find_first_not_of(' ');
    if(idx!=-1){
        src_buf=src_buf.substr(idx,src_buf.size()-idx);
    }
    idx = src_buf.find_last_not_of(' ');
    if(idx!=-1){
        src_buf=src_buf.substr(0,idx+1);
    }
}