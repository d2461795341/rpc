#include<iostream>
#include<bits/stdc++.h>
#include"test.pb.h"
using namespace std;
using namespace fixbug;

int main(){
    //封装了login请求对象的数据
    LoginRequest req;
    req.set_name("zhang san");
    req.set_pwd("123456");

    //对象数据序列化 
    string send_str;
    if(req.SerializeToString(&send_str)){
        cout<<send_str<<endl;
    }

    //对象反序列化
    LoginRequest received_req;
    if(received_req.LoginRequest::ParseFromString(send_str)){
        cout<<received_req.name()<<endl;
        cout<<received_req.pwd()<<endl;
    }


    //对象中还包含了类对象
    LoginResponse rsp;
    rsp.mutable_result()->set_errorcode(366);
    rsp.mutable_result()->set_errormsg("ResultCode1");
    rsp.set_success(false);

    rsp.mutable_another_result()->set_errorcode(378);
    rsp.mutable_another_result()->set_errormsg("ResultCode2");

    string rsp_str;
    if(rsp.SerializeToString(&rsp_str)){
        cout<<rsp_str<<endl;
    }
    
    //对象反序列化
    LoginResponse received_rsp;
    if(received_rsp.ParseFromString(rsp_str)){
        cout<<received_rsp.mutable_result()->errorcode()<<endl;
        cout<<received_rsp.mutable_result()->errormsg()<<endl;
        cout<<received_rsp.mutable_another_result()->errorcode()<<endl;
        cout<<received_rsp.mutable_another_result()->errormsg()<<endl;
        cout<<received_rsp.success()<<endl;
    }


    //对象中包含了类列表
    GetFriendListResponse fl_rsp;
    
    User* user1=fl_rsp.add_friend_list();
    user1->set_name("zs");
    user1->set_age(15);
    user1->set_sex(User::MAN);

    User* user2=fl_rsp.add_friend_list();
    user2->set_name("ls");
    user2->set_age(15);
    user2->set_sex(User::WOMAN);

    cout<<fl_rsp.friend_list_size()<<endl;

    return 0;
}
//g++ -o main main.cc test.pb.cc -lprotobuf 