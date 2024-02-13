#ifndef H_MSGSUPPLIER
#define H_MSGSUPPLIER

#include "UserGroup.hpp"

class MsgRecieverSupplier {
   private:
    Vector<MsgReciever*> recievers;
    int latestId = 0;

   public:
    MsgRecieverSupplier() {}

    void addReciever(MsgReciever* reciever);

    User* addReciever(WebsocketConnection* webSocket);

    UserGroup* createGroup(String name);

    void removeReciever(MsgReciever* reciever);

    void cleanGroups(User* user);

    MsgReciever* getRecieverById(int id);

    Vector<MsgReciever*>* getRecievers();
};

#endif