/*
    This file is part of ESPSmartHome.

    ESPSmartHome is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

    ESPSmartHome is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with Foobar. If not, see <https://www.gnu.org/licenses/>. 
*/


let socket;


let state = [false, false, false, false];
let locked = [false, false, false, false];

function updateButton(id) {
    if (state[id]) {
        document.getElementById(`button${id}`).classList.add(`on`);
    } else {
        document.getElementById(`button${id}`).classList.remove(`on`);
    }

    if (locked[id]) {
        document.getElementById(`button${id}`).children[0].classList.add(`locked`);
    } else {
        document.getElementById(`button${id}`).children[0].classList.remove(`locked`);
    }
}

class Socket {

    constructor() {
        this.websocket = new WebSocket(`ws://${window.location.hostname}/ws`);

        var self = this;

        this.websocket.onopen = (evt) => self.onOpen(evt);
        this.websocket.onclose = (evt) => self.onClose(evt);
        this.websocket.onmessage = (evt) => self.onMessage(evt);
        this.websocket.onerror = (evt) => self.onError(evt);
    }

    onOpen(evt) {}

    onClose(evt) {}


    onEvent(name, args) {
        switch (name) {
            case "ON":
                state[parseInt(args)] = true;
                updateButton(parseInt(args));
                break;
            case "OFF":
                state[parseInt(args)] = false;
                updateButton(parseInt(args));
                break;
            case "LOCK":
                locked[parseInt(args)] = true;
                updateButton(parseInt(args));
                break;
            case "UNLOCK":
                locked[parseInt(args)] = false;
                updateButton(parseInt(args));
                break;
        }
    }

    onMessage(evt) {
        console.log(evt.data);
        let split = evt.data.split(" ", 2);
        this.onEvent(split[0].toUpperCase(), split[1]);
    }

    onError(evt) {}

    onOk() {

    }

    send(message) {
        this.websocket.send(message);
    }

    disconnect() {
        websocket.close();
    }
}

function init() {
    socket = new Socket();

    for (let i = 0; i < 4; i++) {
        updateState(i);
    }
}



function updateState(id) {
    fetch(`http://${window.location.hostname}/state?pin=${id}`).then((s) => {
        return s.text()
    })
    .then((s) => {
        if (s[0] == "1") {
            state[id] = true;
        } else {
            state[id] = false;
        }

        if (s[1] == "1") {
            locked[id] = true;
        } else {
            locked[id] = false;
        }

        updateButton(id);

    });
}

function pinClick(id) {
    fetch(`http://${window.location.hostname}/toggle?pin=${id}`).then((s) => {
        return s.text()
    })
    .then((s) => {
        if (s[0] == "1") {
            state[id] = true;
        } else {
            state[id] = false;
        }

        if (s[1] == "1") {
            locked[id] = true;
        } else {
            locked[id] = false;
        }


        updateButton(id);
    });
}

window.addEventListener("load", init, false);