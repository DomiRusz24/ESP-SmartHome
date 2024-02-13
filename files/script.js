/*
    httpServer.paths.set("/user", onUser);
    httpServer.paths.set("/allusers", onAllUser);
    httpServer.paths.set("/allusersfor", onUserSpecific);
    httpServer.paths.set("/type", onType);
    httpServer.paths.set("/creategroup", onCreateGroup);
*/

class User {
    constructor(id) {
        this.id = id;
        this.name = "UNKNOWN";
    }


    updateUsername() {
        var self = this;
        fetch(`http://${window.location.hostname}/user?id=` + this.id).then(function(response) {
            return response.text();
        }).then(function(data) {
            self.name = name;
        });
    }
}

let socket;


class Socket {

    constructor() {
        this.id = -1;
        this.name = "UNKNOWN";
        this.online = [];

        this.websocket = new WebSocket(`ws://${window.location.hostname}/ws`);

        var self = this;

        this.websocket.onopen = () => self.onOpen;
        this.websocket.onclose = () => self.onClose;
        this.websocket.onmessage = () => self.onMessage;
        this.websocket.onerror = () => self.onError;
    }

    onOpen(evt) {}

    onClose(evt) {}

    onMessage(evt) {
        let split = evt.data.split(" ", 2);
        if (split.size() != 2) return;
        onEvent(split[0].toUpperCase(), split[1]);
    }

    onError(evt) {}


    onEvent(name, args) {
        switch (name) {
            case "OK":
                this.onOk();
                break;
            case "ERROR":
                this.onError();
                break;
            case "REGISTER":
                this.onRegister(toNumber(args));
                break;
            case "MESSAGE":
                let split = args.split(" ", 2);
                if (split.size() != 2) return;
                this.onMessage(toNumber(split[0]), split[1]);
                break;
            case "KICK":
                this.onKick(toNumber(args));
                break;
            case "JOIN":
                this.onJoin(toNumber(args));
                break;
            case "LEAVE":
                this.onLeave(toNumber(args));
                break;
        }
    }

    onOk() {

    }

    onError() {

    }

    onRegister(id) {
        this.id = id;
    }


    getUserName(id) {
        for (let user of this.online) {
            if (user.id == id) return user.name;
        }

        return "UNKNOWN";
    }

    onMessage(from, message) {
        let name = this.getUserName(from);
        console.log(name + ": " + message);
    }

    onKick(who) {

    }

    onJoin(who) {
        getOnlineUsers();
    }

    onLeave(who) {
        getOnlineUsers();
    }


    sendMessage(to, message) {
        this.send("SEND " + to + " " + message);
    }

    kick(who) {
        this.send("KICK " + who);
    }

    register(name) {
        this.send("REGISTER " + name);
        this.name = name;
    }

    send(message) {
        this.websocket.send(message);
    }

    getOnlineUsers() {
        var self = this;
        fetch(`http://${window.location.hostname}/allusersfor?id=` + this.id).then(function(response) {
            return response.text();
        }).then(function(data) {
            self.online = [];
            for (let userId of data.split(" ")) {
                let user = new User(Number(userId));
                user.updateUsername();
                self.online.push(user);
            }
        });
    }

    disconnect() {
        websocket.close();
    }
}

function init() {
    console.log("sdfsdf");
    socket = new Socket();
}

window.addEventListener("load", init, false);