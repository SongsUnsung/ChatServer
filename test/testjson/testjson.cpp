#include "json.hpp"

using json=nlohmann::json;

#include<iostream>
#include<vector>
#include<map>
#include<string>

using namespace std;

void func1(){
    json js;
    js["msg_type"]=2;
    js["from"]="zhang san";
    js["to"]="li si";
    js["msg"]="hello,what are you doing now?";

    //转成字符串，可以通过网络发送

    string sendBuf=js.dump();
    cout<<sendBuf.c_str()<<endl;
}

void func2(){
    json js;

    js["id"]={1,2,3,4,5};

    js["name"]="zhang san";

    js["msg"]["zhang san"]="hello world";
    js["msg"]["liu shou"]="hello china";
    js["msg"] = {{"zhang san", "hello world"}, {"liu shuo", "hello china"}};
    cout << js << endl;
}

void func3()
{
    json js;
    // 直接序列化一个vector容器
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(5);
    js["list"] = vec;
    // 直接序列化一个map容器
    map<int, string> m;
    m.insert({1, "黄山"});
    m.insert({2, "华山"});
    m.insert({3, "泰山"});
    js["path"] = m;
    string sendBuf=js.dump();

    cout<<sendBuf.c_str()<<endl;
}

string func4()
{
  
    json js;
    js["msg_type"]=2;
    js["from"]="zhang san";
    js["to"]="li si";
    js["msg"]="hello,what are you doing now?";

    //转成字符串，可以通过网络发送

    string sendBuf=js.dump();
    return sendBuf;
    
}
int main()
{
    func1();
    func2();
    func3();

    string recvBuf=func4();
    json jsbuf=json::parse(recvBuf);
    cout<<jsbuf["msg_type"]<<endl;
    cout<<jsbuf["from"]<<endl;
    cout<<jsbuf["to"]<<endl;
    cout<<jsbuf["msg"]<<endl;
    return 0;
}