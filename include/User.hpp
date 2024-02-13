#ifndef H_USER
#define H_USER

#include <SmingCore.h>

#include "AppEvents.hpp"
#include "MsgReciever.hpp"

enum UserState {
    UNKNOWN,
    REGISTERED
};

class User : MsgReciever {
private:
    int id;
    String name;
    WebsocketConnection* webSocket;
    UserState state;
public:
    User(int id, WebsocketConnection* webSocket) : id(id), name("UNKNOWN"), webSocket(webSocket), state(UserState::UNKNOWN) {};
    ~User();

    int getId() { return id; }
    String getName() { return name; }
    void setName(String name) {
        this->name = name;
    }
    void sendEvent(ServerEvent event);
    void sendError();
    WebsocketConnection* getWebsocket() { return webSocket; }
    UserState getState() { return state; }
    void recieveMessage(int from, String content) {
        sendEvent(ServerEvent{server::Type::MESSAGE, new server::Message{from, content}});
    }

    void kick(int who) {
        sendEvent(ServerEvent{server::Type::KICK, new server::Kick{who}});
    }

    MsgType getType() {
        return MsgType::USER;
    }
};

#endif