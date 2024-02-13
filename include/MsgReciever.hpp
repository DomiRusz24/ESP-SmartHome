#ifndef H_MSGRECIEVER
#define H_MSGRECIEVER


#include <SmingCore.h>
#include "MsgType.hpp"

class MsgReciever {
public:
    virtual ~MsgReciever(){}
    virtual void recieveMessage(int from, String content) = 0;
    virtual void kick(int who) = 0;
    virtual int getId() = 0;
    virtual String getName() = 0;
    virtual MsgType getType() = 0;
};

#endif