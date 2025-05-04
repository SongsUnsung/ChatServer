#include "chatservice.hpp"
#include "public.hpp"



#include <muduo/base/Logging.h>

using namespace muduo;
using namespace muduo::net;

ChatService* ChatService::instance()
{
    static ChatService service;
    return &service;
}

ChatService::ChatService()
{
    _msgHandlerMap.insert({LOGIN_MSG,std::bind(&ChatService::login,this,_1,_2,_3)});
    _msgHandlerMap.insert({REG_MSG,std::bind(&ChatService::reg,this,_1,_2,_3)});

}

MsgHandler ChatService::getHandler(int msgid)
{
    //记录错误日志，msgid没有对应的事件处理回调
    auto it =_msgHandlerMap.find(msgid);
    if(it==_msgHandlerMap.end())
    {
        return [=](const TcpConnectionPtr& conn,json &js,Timestamp time)
        {
            LOG_ERROR<<"msgid:"<<msgid<<"can not find handler!";
        };
    }
    else 
    {
        return _msgHandlerMap[msgid];
    }
}
//处理登录业务
void ChatService::login(const TcpConnectionPtr& conn,json &js,Timestamp time)
{
    //LOG_INFO<<"Do login service.";
    int id=js["id"].get<int>();
    string pwd=js["password"];

    User user=_userModel.query(id);
    if(user.getId()!=-1&&user.getPwd()==pwd){
        if(user.getState()=="online")
        {
            json response;
            response["msgid"]=LOGIN_MSG_ACK;
            response["errno"]=2;
            response["errmsg"]="该账号已登录";
            conn->send(response.dump());
        }
        else
        {
            user.setState("online");
            _userModel.updateState(user);

            json response;
            response["msgid"]=LOGIN_MSG_ACK;
            response["errno"]=0;
            response["id"]=user.getId();
            response["name"]=user.getName();
            conn->send(response.dump());
        }
        
    }
    else
    {
        json response;
        response["msgid"]=LOGIN_MSG_ACK;
        response["errno"]=1;
        response["errmsg"]="用户名或者密码错误";
        conn->send(response.dump());
    }
    //{"msgid":1,"id":15,"password":"123456"}
    //{"msgid":1,"id":13,"password":"123456"}

}
//处理注册业务
void ChatService::reg(const TcpConnectionPtr& conn,json &js,Timestamp time)
{
    //LOG_INFO<<"Do reg service.";
    string name=js["name"];
    string pwd=js["password"];

    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state=_userModel.insert(user);
    if(state)
    {
        json responsee;
        responsee["msgid"]=REG_MSG_ACK;
        responsee["errno"]=0;
        responsee["id"]=user.getId();
        conn->send(responsee.dump());
    }
    else
    {
        json responsee;
        responsee["msgid"]=REG_MSG_ACK;
        responsee["errno"]=1;
        conn->send(responsee.dump());
    }
    //{"msgid":2,"name":"zhu c","password":"123456"}

}