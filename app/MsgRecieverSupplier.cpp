#include "MsgRecieverSupplier.h"

void MsgRecieverSupplier::removeReciever(MsgReciever* reciever) {
    if (recievers.contains(reciever)) {
        recievers.removeElement(reciever);
        if (reciever->getType() == MsgType::USER) {
            cleanGroups((User*)reciever);
        }

        for (MsgReciever* other : *getRecievers()) {
            if (other == reciever) continue;
            if (other->getType() == MsgType::USER) {
                User* user = (User*)other;
                user->sendEvent(ServerEvent{
                    server::Type::LEAVE, new server::Leave{reciever->getId()}});
            }
        }

        delete reciever;
    }
}

void MsgRecieverSupplier::cleanGroups(User* user) {
    for (MsgReciever* reciever : recievers) {
        if (reciever->getType() == MsgType::GROUP) {
            UserGroup* group = (UserGroup*)reciever;
            group->removeUser(user);
            if (group->isEmpty()) {
                removeReciever(reciever);
            }
        }
    }
}

UserGroup* MsgRecieverSupplier::createGroup(String name) {
    UserGroup* group = new UserGroup(latestId, name);
    latestId++;
    recievers.add((MsgReciever*) group);

    return group;
}

User* MsgRecieverSupplier::addReciever(WebsocketConnection* webSocket) {
    User* user = new User(latestId, webSocket);

    for (MsgReciever* other : *getRecievers()) {
        if (other->getType() == MsgType::USER) {
            User* otherUser = (User*)other;
            otherUser->sendEvent(ServerEvent{server::Type::JOIN,
                                             new server::Leave{user->getId()}});
        }
    }

    recievers.add((MsgReciever*)user);
    latestId++;
    return user;
}

MsgReciever* MsgRecieverSupplier::getRecieverById(int id) {
    for (MsgReciever* reciever : recievers) {
        if (reciever->getId() == id) {
            return reciever;
        }
    }

    return nullptr;
}

Vector<MsgReciever*>* MsgRecieverSupplier::getRecievers() { return &recievers; }