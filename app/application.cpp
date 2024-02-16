/*
    This file is part of ESPSmartHome.

    ESPSmartHome is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

    ESPSmartHome is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>. 

    Author Dominik Ruszczyk
*/

#include "Digital.h"
#include "Network/Http/Websocket/WebsocketConnection.h"
#include "Platform/Station.h"
#include "WConstants.h"
#include <Network/Http/Websocket/WebsocketResource.h>
#include <SmingCore.h>


// #define MDNS

#ifdef MDNS
#include <Network/Mdns/Service.h>
#include <Network/Mdns/Responder.h>
#include <Network/Mdns/debug.h>
#endif

HttpServer httpServer;

// If you want, you can define WiFi settings globally in Eclipse Environment
#ifndef WIFI_SSID
#define WIFI_SSID "Masterdev" // Put your SSID and password here
#define WIFI_PWD "cn2+cx@79"
#endif

#define PIN_1 5
#define PIN_2 14
#define PIN_3 12
#define PIN_4 13

// nie dziala: 15 16

#define INPUT_1 0 // dziala
#define INPUT_2 4 // dziala
#define INPUT_3 3 // dziala
#define INPUT_4 2 // dziala

#define PIN_AMOUNT 4

#define DEFAULT_STATE false

const uint16_t PINS[PIN_AMOUNT] = {PIN_1, PIN_2, PIN_3, PIN_4};
const uint16_t LOCK_PINS[PIN_AMOUNT] = {INPUT_1, INPUT_2, INPUT_3, INPUT_4};

struct PinStatus {
  bool state;
  bool locked;

  String getState() {
    String s = state ? "1" : "0";
    s+= locked ? "1" : "0";
    return s;
  }
};


PinStatus PINS_STATUS[PIN_AMOUNT] = {{DEFAULT_STATE, false}, {DEFAULT_STATE, false}, {DEFAULT_STATE, false}, {DEFAULT_STATE, false}};

void onIndex(HttpRequest &request, HttpResponse &response) {
  auto tmpl = new TemplateFileStream(F("index.html"));
  // auto& vars = tmpl->variables();
  // vars["counter"] = String(counter);
  response.sendNamedStream(tmpl);
}


String pinToString(int pin) {
  return PINS_STATUS[pin].getState();
}



bool isLocked(int pin) {
  return PINS_STATUS[pin].locked;
}


String setPin(size_t pin, bool state) {
  if (pin < 0 || pin >= PIN_AMOUNT) {
    return "-1";
  }

  if (PINS_STATUS[pin].state == state) return pinToString(pin);
  if (isLocked(pin)) return pinToString(pin);

  digitalWrite(PINS[pin], !state);
  PINS_STATUS[pin].state = state;

  if (state) {
    WebsocketConnection::broadcast("ON " + String(pin));
  } else {
    WebsocketConnection::broadcast("OFF " + String(pin));
  } 


  return pinToString(pin);
}

String getPin(size_t pin) {
  if (pin < 0 || pin >= PIN_AMOUNT) {
    return "-1";
  }

  return pinToString(pin);
}


String togglePin(size_t pin) {
  return setPin(pin, !PINS_STATUS[pin].state);
}

void onTurnOn(HttpRequest &request, HttpResponse &response) {
  String pin = request.getQueryParameter("pin", "NONE");
  if (pin == "NONE") {
    response.sendString("-1");
    return;
  }

  response.sendString(setPin(pin.toInt(), true));
}

void onState(HttpRequest &request, HttpResponse &response) {
  String pin = request.getQueryParameter("pin", "NONE");
  if (pin == "NONE") {
    response.sendString("-1");
    return;
  }

  response.sendString(getPin(pin.toInt()));
}


void onAmount(HttpRequest &request, HttpResponse &response) {
  String s = "";
  s += PIN_AMOUNT;
  response.sendString(s);
}

void onTurnOff(HttpRequest &request, HttpResponse &response) {
  String pin = request.getQueryParameter("pin", "NONE");
  if (pin == "NONE") {
    response.sendString("-1");
    return;
  }

  response.sendString(setPin(pin.toInt(), false));
}

void onToggle(HttpRequest &request, HttpResponse &response) {
  String pin = request.getQueryParameter("pin", "NONE");
  if (pin == "NONE") {
    response.sendString("-1");
    return;
  }

  response.sendString(togglePin(pin.toInt()));
}

void onFile(HttpRequest &request, HttpResponse &response) {
  String file = request.uri.getRelativePath();

  if (file[0] == '.') {
    response.code = HTTP_STATUS_FORBIDDEN;
  } else {
    response.setCache(86400, true);
    response.sendFile(file);
  }
}

void wsConnected(WebsocketConnection &socket) {
  // socket.setUserData(user);
}

void wsUserEvent(const String event, const String args) {}

void wsMessageReceived(WebsocketConnection &socket, const String &message) {
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

  wsUserEvent(event, args);
}

void wsBinaryReceived(WebsocketConnection &socket, uint8_t *data, size_t size) {
}

void wsDisconnected(WebsocketConnection &socket) {}


#ifdef MDNS


DEFINE_FSTR_LOCAL(hostName, "smarthome");

HttpServer server;

class SmarthomeService : public mDNS::Service {
public:
  String getInstance() override { return F("UDP Server"); }

  void addText(mDNS::Resource::TXT &txt) override {
    txt.add(F("md=SmartHome"));
    txt.add(F("fn=SmartHome on ESP8266"));
  }
};

static mDNS::Responder responder;
static SmarthomeService smarthomeService;

void startmDNS() {
  responder.begin(hostName);
  responder.addService(smarthomeService);
}

#endif


void startWebServer() {
  httpServer.listen(80);
  httpServer.paths.set("/", onIndex);
  httpServer.paths.set("/state", onState);
  httpServer.paths.set("/amount", onAmount);
  httpServer.paths.set("/on", onTurnOn);
  httpServer.paths.set("/off", onTurnOff);
  httpServer.paths.set("/toggle", onToggle);
  httpServer.paths.setDefault(onFile);

  // Web Sockets configuration
  auto wsResource = new WebsocketResource();
  wsResource->setConnectionHandler(wsConnected);
  wsResource->setMessageHandler(wsMessageReceived);

  wsResource->setBinaryHandler(wsBinaryReceived);
  wsResource->setDisconnectionHandler(wsDisconnected);

  httpServer.paths.set("/ws", wsResource);

  //Serial.println(WifiStation.getIP());
}

// Will be called when WiFi station becomes fully operational
void gotIP(IpAddress ip, IpAddress netmask, IpAddress gateway) {
  startWebServer();


  #ifdef MDNS
  startmDNS();
  #endif
}

void initPins() {
  for (size_t i = 0; i < PIN_AMOUNT; i++) {
    pinMode(PINS[i], OUTPUT_OPEN_DRAIN);
    digitalWrite(PINS[i], !DEFAULT_STATE);

    pinMode(LOCK_PINS[i], INPUT_PULLUP);
  }
}

void lockButton(int pin) {
  if (isLocked(pin)) return;

  setPin(pin, true);
  WebsocketConnection::broadcast("LOCK " + String(pin));
  PINS_STATUS[pin].locked = true;
}


void unLockButton(int pin) {
    if (!isLocked(pin)) return;

    PINS_STATUS[pin].locked = false;
    WebsocketConnection::broadcast("UNLOCK " + String(pin));
    setPin(pin, false);
}

void checkLocks() {
  for (size_t i = 0; i < PIN_AMOUNT; i++) {
    uint8_t status = digitalRead(LOCK_PINS[i]);
    if (status == 1) {
      unLockButton(i);
    } else {
      lockButton(i);
    }
  }
}

Timer procTimer;

void startSerial() {
  Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
  Serial.systemDebugOutput(true); // Enable debug output to serial
}

void init() {
  spiffs_mount(); // Mount file system, in order to work with files

  //startSerial();

  WifiStation.setHostname("smarthome");
  WifiStation.enable(true);
  WifiStation.setHostname("smarthome");
  WifiStation.config(WIFI_SSID, WIFI_PWD);
  WifiStation.setHostname("smarthome");
  WifiAccessPoint.enable(false);
  WifiStation.setHostname("smarthome");

  // Run our method when station was connected to AP
  WifiEvents.onStationGotIP(gotIP);

  initPins();


  procTimer.initializeMs(200, checkLocks).start();
}
