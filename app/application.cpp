#include <Network/Http/Websocket/WebsocketResource.h>
#include <SmingCore.h>

HttpServer httpServer;

#include "UserGroup.hpp"
#include "MsgRecieverSupplier.hpp"

// If you want, you can define WiFi settings globally in Eclipse Environment
// Varia1bles
#ifndef WIFI_SSID
#define WIFI_SSID "Masterdev"  // Put your SSID and password here
#define WIFI_PWD "cn2+cx@79"
#endif

unsigned totalActiveSockets = 0;

void onIndex(HttpRequest &request, HttpResponse &response) {
    auto tmpl = new TemplateFileStream(F("index.html"));
    // auto& vars = tmpl->variables();
    // vars["counter"] = String(counter);
    response.sendNamedStream(tmpl);
}

MsgRecieverSupplier supplier;

void onUser(HttpRequest &request, HttpResponse &response) {
    String id = request.getQueryParameter("id", "NULL");

    if (id.equals("NULL")) {
        response.sendString("");
        return;
    }

    int numId = id.toInt();

    MsgReciever *reciever = supplier.getRecieverById(numId);

    if (reciever == nullptr) {
        response.sendString("");
        return;
    }

    response.sendString(reciever->getName());
}

void onType(HttpRequest &request, HttpResponse &response) {
    String id = request.getQueryParameter("id", "NULL");

    if (id.equals("NULL")) {
        response.sendString("");
        return;
    }

    int numId = id.toInt();

    MsgReciever *reciever = supplier.getRecieverById(numId);

    if (reciever == nullptr) {
        response.sendString("");
        return;
    }

    switch (reciever->getType()) {
        case MsgType::GROUP:
            response.sendString("GROUP");
            return;
        case MsgType::USER:
            response.sendString("USER");
            return;
    }

    response.sendString("");
}

void onAllUser(HttpRequest &request, HttpResponse &response) {
    String ids = "";

    for (MsgReciever *reciever : *supplier.getRecievers()) {
        ids += reciever->getId();
        ids += " ";
    }

    response.sendString(ids);
}

void onUserSpecific(HttpRequest &request, HttpResponse &response) {
    String id = request.getQueryParameter("id", "NULL");

    if (id.equals("NULL")) {
        response.sendString("");
        return;
    }

    int numId = id.toInt();

    MsgReciever *reciever = supplier.getRecieverById(numId);

    if (reciever == nullptr) {
        response.sendString("");
        return;
    }

    String ids = "";

    for (MsgReciever *reciever : *supplier.getRecievers()) {
        if (reciever->getType() == MsgType::GROUP) {
            UserGroup* group = (UserGroup*) reciever;
            if (group->containsUser(numId)) {
                    ids += reciever->getId();
                    ids += " ";
            }
        } else {
            ids += reciever->getId();
            ids += " ";
        }
    }

    response.sendString(ids);
}

void onCreateGroup(HttpRequest &request, HttpResponse &response) {
    String id = request.getQueryParameter("id", "NULL");
    if (id.equals("NULL")) {
        response.sendString("");
        return;
    }
    int numId = id.toInt();

    String name = request.getQueryParameter("name", "NULL");

    if (name.equals("NULL")) {
        response.sendString("");
        return;
    }

    MsgReciever *reciever = supplier.getRecieverById(numId);

    if (reciever == nullptr || reciever->getType() != MsgType::USER) {
        response.sendString("");
        return;
    }

    //UserGroup* group = supplier.createGroup(name);

    //group->addUser((User*) reciever);

    //String s = "";
    //s+= group->getId();

    //response.sendString(s);
}


void onFile(HttpRequest &request, HttpResponse &response) {
    String file = request.uri.getRelativePath();

    if (file[0] == '.') {
        response.code = HTTP_STATUS_FORBIDDEN;
    } else {
        response.setCache(
            86400,
            true);  // It's important to use cache for better performance.
        response.sendFile(file);
    }
}

void wsConnected(WebsocketConnection &socket) {
    totalActiveSockets++;
    User *user = supplier.addReciever(&socket);
    socket.setUserData(user);
}

void wsUserEvent(User &user, const String event, const String args) {
    if (event.equalsIgnoreCase("REGISTER") && args.length() > 0) {
        user.setName(args);
        user.sendEvent(ServerEvent{server::Type::REGISTERED,
                                   new server::Registered{user.getId()}});
    } else if (event.equalsIgnoreCase("SEND") && args.length() > 3) {
        int id = -1;
        String message;
        for (int i = 0; i < args.length(); i++) {
            if (args[i] == ' ') {
                id = args.substring(0, i).toInt();
                message = args.substring(i + 1);
                break;
            }
        }

        if (id == -1) {
            user.sendError();
            return;
        }

        MsgReciever *reciever = supplier.getRecieverById(id);

        if (reciever == nullptr) {
            user.sendError();
            return;
        }

        reciever->recieveMessage(user.getId(), message);
    } else if (event.equalsIgnoreCase("KICK") && args.length() > 0) {
        int id = args.toInt();

        if (id == -1) {
            user.sendError();
            return;
        }

        MsgReciever *reciever = supplier.getRecieverById(id);

        if (reciever == nullptr) {
            user.sendError();
            return;
        }

        reciever->kick(user.getId());
    } else {
        user.sendError();
    }
}

void wsMessageReceived(WebsocketConnection &socket, const String &message) {
    User *user = reinterpret_cast<User *>(socket.getUserData());
    if (user != nullptr) {
        int id = -1;
        String event = "";
        String args = "";
        for (int i = 0; i < message.length(); i++) {
            if (message[i] == ' ') {
                event = message.substring(0, i);
                args = message.substring(i + 1);
                break;
            }
        }

        if (event.equals("")) {
            event = message;
        }

        wsUserEvent(*user, event, args);
    }

    Serial.println(_F("WebSocket message received:"));
    Serial.println(message);
}

void wsBinaryReceived(WebsocketConnection &socket, uint8_t *data, size_t size) {
    Serial << _F("Websocket binary data received, size: ") << size << endl;
}

void wsDisconnected(WebsocketConnection &socket) {
    totalActiveSockets--;

    User *user = reinterpret_cast<User *>(socket.getUserData());
    if (user != nullptr) {
        Serial.println(_F("Removed user:"));
        Serial.println(user->getName());
        supplier.removeReciever((MsgReciever *)user);
    }
}

void startWebServer() {
    httpServer.listen(80);
    httpServer.paths.set("/", onIndex);
    httpServer.paths.set("/user", onUser);
    httpServer.paths.set("/allusers", onAllUser);
    httpServer.paths.set("/allusersfor", onUserSpecific);
    httpServer.paths.set("/type", onType);
    httpServer.paths.set("/creategroup", onCreateGroup);
    httpServer.paths.setDefault(onFile);

    // Web Sockets configuration
    auto wsResource = new WebsocketResource();
    wsResource->setConnectionHandler(wsConnected);
    wsResource->setMessageHandler(wsMessageReceived);

    wsResource->setBinaryHandler(wsBinaryReceived);
    wsResource->setDisconnectionHandler(wsDisconnected);

    httpServer.paths.set("/ws", wsResource);

    Serial.println(
        _F("\r\n"
           "=== WEB SERVER STARTED ==="));
    Serial.println(WifiStation.getIP());
    Serial.println(_F("==========================\r\n"));
}

// Will be called when WiFi station becomes fully operational
void gotIP(IpAddress ip, IpAddress netmask, IpAddress gateway) {
    startWebServer();
}

void init() {
    spiffs_mount();  // Mount file system, in order to work with files

    Serial.begin(SERIAL_BAUD_RATE);  // 115200 by default
    Serial.systemDebugOutput(true);  // Enable debug output to serial

    WifiStation.enable(true);
    WifiStation.config(WIFI_SSID, WIFI_PWD);
    WifiAccessPoint.enable(false);

    // Run our method when station was connected to AP
    WifiEvents.onStationGotIP(gotIP);
}
