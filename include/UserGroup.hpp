#ifndef H_USERGROUP
#define H_USERGROUP

#include <SmingCore.h>

#include "User.hpp"

class UserGroup : MsgReciever {
private:
    int id;
    String name;
    Vector<User*> users;
public:
    UserGroup(int id, String name) : id(id), name(name) {};
    ~UserGroup();

    int getId();
    String getName();

    void addUser(User* user);

    void removeUser(User* user);

    bool containsUser(int id);


    void recieveMessage(int from, String content);

    void kick(int who);


    MsgType getType();

    boolean isEmpty();

};

#endif