#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include<vector>
#include "user.hpp"

class FriendModel
{
public:
    void insert(int userid,int friendid);

    std::vector<User>query(int userid);
};

#endif