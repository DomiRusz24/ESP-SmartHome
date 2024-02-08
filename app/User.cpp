#include "User.h"

User::~User() {
    this->getWebsocket()->setUserData(nullptr);
    this->webSocket = nullptr;
}


void User::sendEvent(ServerEvent event) {
    if (event.type == server::Type::OK) {
        getWebsocket()->send("OK");
    } else if (event.type == server::Type::ERROR) {
        getWebsocket()->send("ERROR");
    } else if (event.type == server::Type::REGISTERED) {
        server::Registered* registered = (server::Registered*) event.event;
        String message = "REGISTERED ";
        message+= registered->id;
        getWebsocket()->sendString(message);
        delete registered;
    } else if (event.type == server::Type::MESSAGE) {
        server::Message* message = (server::Message*) event.event;
        String output = "MESSAGE FROM ";
        output+= message->from;
        output+= ": ";
        output+= message->message;
        getWebsocket()->sendString(output);
        delete message;
    } else if (event.type == server::Type::KICK) {
        server::Kick* kick = (server::Kick*) event.event;
        String output = "KICK ";
        output+= kick->who;
        getWebsocket()->sendString(output);
        delete kick;
    } else if (event.type == server::Type::JOIN) {
        server::Join* join = (server::Join*) event.event;
        String output = "JOIN ";
        output+= join->id;
        getWebsocket()->sendString(output);
        delete join;
    } else if (event.type == server::Type::LEAVE) {
        server::Leave* leave = (server::Leave*) event.event;
        String output = "LEAVE ";
        output+= leave->id;
        getWebsocket()->sendString(output);
        delete leave;
    }
}

void User::sendError() {
    sendEvent(ServerEvent{server::Type::ERROR, (void*) 0});
}