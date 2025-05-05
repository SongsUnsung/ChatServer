#ifndef GROUP_H
#define GROUP_H

#include "groupuser.hpp"
#include<string>
#include<vector>

class Group
{
public:
    Group(int id=-1,std::string name="",std::string desc="")
    {
        this->id=id;
        this->name=name;
        this->desc=desc;
    }

    void setId(int id){this->id=id;}
    void setName(std::string name){this->name=name;}
    void setDesc(std::string desc){this->desc=desc;}

    int getId()const{return this->id;}

    const std::string& getName()const{return this->name;}
    const std::string& getDesc()const{return this->desc;}

    std::vector<GroupUser>&getUsers(){return this->users;}


private:
    int id;
    std::string name;
    std::string desc;
    std::vector<GroupUser>users;
};

#endif