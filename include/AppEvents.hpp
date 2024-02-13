#ifndef H_APPEVENTS
#define H_APPEVENTS


#include <SmingCore.h>


namespace client {
    enum Type {
        REGISTER
    };

    struct Register {
        String name;
    };
};

namespace server {
    enum Type {
        REGISTERED, OK, ERROR, MESSAGE, KICK, JOIN, LEAVE
    };

    struct Registered {
        int id;
    };

    struct Ok {};
    struct Error {};

    struct Message {
        int from;
        String message;
    };

    struct Kick {
        int who;
    };

    struct Join {
        int id;
    };

    struct Leave {
        int id;
    };
};

struct ClientEvent {
    client::Type type;
    void* event;
};

struct ServerEvent {
    server::Type type;
    void* event;
};

#endif