# ESP SmartHome server

## SmartHome type web server designed for the ESP8266 processor using the [Sming Library](https://github.com/SmingHub/Sming)

Using a REST API you are able to toggle the state of the output pins on the board.
There is also a websocket server that dynamically sends status updates, which helps in developing a responsive front-end for multiple devices at once.

### Rest API

| Path    | Paramaters   | Output                                                                                                                  |
|---------|--------------|-------------------------------------------------------------------------------------------------------------------------|
| /amount | None         | Number of pins ("4" by default)                                                                                         |
| /state  | ?pin=[0 - N] | 2 numbers, first is pin state (0 or 1), second is locked state (0 or 1). Example: "00" (Pin not toggled and not locked) |
| /on     | ?pin=[0 - N] | Turns on the pin, and returns /state                                                                                    |
| /off    | ?pin=[0 - N] | Turns off the pin, and returns /state                                                                                   |
| /toggle | ?pin=[0 - N] | Toggles the pin, and returns /state                                                                                     |
| /ws     | NONE         | Websocket URL

### Websocket output

| Command | Paramaters |
|---------|------------|
| ON      | Pin ID     |
| OFF     | Pin ID     |
| LOCK    | Pin ID     |
| UNLOCK  | Pin ID     |


### Images

![Image 1](https://github.com/DomiRusz24/ESP-SmartHome/blob/main/img/1.png?raw=true "Image 1")
![Image 2](https://github.com/DomiRusz24/ESP-SmartHome/blob/main/img/2.png?raw=true "Image 2")
![Image 3](https://github.com/DomiRusz24/ESP-SmartHome/blob/main/img/3.png?raw=true "Image 3")


