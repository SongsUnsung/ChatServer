#include "chatservice.hpp"
#include "public.hpp"


#include<vector>
#include <muduo/base/Logging.h>

using namespace muduo;
using namespace muduo::net;
using std::vector;

ChatService* ChatService::instance()
{
    static ChatService service;
    return &service;
}

ChatService::ChatService()
{
    _msgHandlerMap.insert({LOGIN_MSG,std::bind(&ChatService::login,this,_1,_2,_3)});
    _msgHandlerMap.insert({REG_MSG,std::bind(&ChatService::reg,this,_1,_2,_3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG,std::bind(&ChatService::oneChat,this,_1,_2,_3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG,std::bind(&ChatService::addFriend,this,_1,_2,_3)});
}

void ChatService::reset()
{
    _userModel.resetState();
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
            {
                std::lock_guard<std::mutex>lock(_connMutex);
                _userConnMap.insert({id,conn});
            }
            user.setState("online");
            _userModel.updateState(user);

            json response;
            response["msgid"]=LOGIN_MSG_ACK;
            response["errno"]=0;
            response["id"]=user.getId();
            response["name"]=user.getName();

            //查询用户是否有离线消息
            vector<string>vec=_offlineMsgModel.query(id);
            if(!vec.empty())
            {
                response["offlinemsg"]=vec;

                _offlineMsgModel.remove(id);
            }

            vector<User>userVec=_friendModel.query(id);
            if(!userVec.empty())
            {
                vector<string>vec2;
                for(User &user:userVec)
                {
                    json js;
                    js["id"]=user.getId();
                    js["name"]=user.getName();
                    js["state"]=user.getState();
                    vec2.push_back(js.dump());
                }
                response["friends"]=vec2;
            }

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

void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{   
    User user;
    {
        std::lock_guard<std::mutex>lock(_connMutex);
        for(auto it=_userConnMap.begin();it!=_userConnMap.end();++it)
        {
            if(it->second==conn)
            {   
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }
    if(user.getId()!=-1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
}


void ChatService::oneChat(const TcpConnectionPtr &conn,json &js,muduo::Timestamp time)
{
    int toid=js["to"].get<int>();

    {
        std::lock_guard<std::mutex>lock(_connMutex);
        auto it=_userConnMap.find(toid);
        if(it!=_userConnMap.end())
        {
            it->second->send(js.dump());
            return;
        } 
        //{"msgid":1,"id":13,"password":"123456"}
        //{"msgid":1,"id":15,"password":"666666"}
        //{"msgid":5,"id":13,"from":"zhang san","to":15,"msg":"hello81"}
        //

    }

    _offlineMsgModel.insert(toid,js.dump());

}

void ChatService::addFriend(const TcpConnectionPtr &conn,json &js,muduo::Timestamp time)
{
    int userid=js["id"].get<int>();
    int friendid=js["friendid"].get<int>();

    _friendModel.insert(userid,friendid);
}


void ChatService::createGroup(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
    int userid=js["id"].get<int>();
    string name=js["groupname"];
    string desc=js["groupdesc"];

    Group group(-1,name,desc);
    if(_groupModel.createGroup(group))
    {
        _groupModel.addGroup(userid,group.getId(),"creator");
    }
}

void ChatService::addGroup(const TcpConnectionPtr& conn,json &js,Timestamp time)
{
    int userid=js["id"].get<int>();
    int groupid=js["groupid"].get<int>();
    _groupModel.addGroup(userid,groupid,"normal");
}

void ChatService::groupChat(const TcpConnectionPtr& conn,json &js,Timestamp time)
{
    int userid=js["id"].get<int>();
    int groupid=js["groupid"].get<int>();
    vector<int>useridVec=_groupModel.queryGroupUsers(userid,groupid);
    
    std::lock_guard<std::mutex>lock(_connMutex);

    for(int id:useridVec)
    {
        auto it=_userConnMap.find(id);
        if(it!=_userConnMap.end())
        {
            it->second->send(js.dump());
        }
        else{
            _offlineMsgModel.insert(id,js.dump());
        }
    }
}