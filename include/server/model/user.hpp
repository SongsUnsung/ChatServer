#ifndef USER_H
#define USER_H

#include <string>

class User{
public:
    User(int id=-1,std::string name="",std::string pwd="",std::string state="offline")
    {
        this->_id=id;
        this->_name=name;
        this->_password=pwd;
        this->_state=state;
    }

    void setId(int id){this->_id=id;}
    void setName(std::string name){this->_name=name;}
    void setPwd(std::string pwd){this->_password=pwd;}
    void setState(std::string state){this->_state=state;}

    int getId()const{return this->_id;}
    const std::string& getName()const{return this->_name;}
    const std::string& getPwd()const{return this->_password;}
    const std::string& getState()const{return this->_state;}

private:
    int _id;
    std::string _name;
    std::string _password;
    std::string _state;
};

#endif