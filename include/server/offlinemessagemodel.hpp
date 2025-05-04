#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H

#include<string>
#include<vector>

class OfflineMsgModel
{
public:
    void insert(int userid,std::string msg);

    void remove(int userid);

    std::vector<std::string>query(int userid);
};

#endif