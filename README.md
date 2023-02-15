# velux_esp32
Apple Homekit integration (voice activation) for Velux windows using Lilygo T-Relay ESP32 and velux standard remotes

## What is this repo or project?

Code and instructions to automate opening and closing of Velux window and blind using apple home

## What do you need?

Lilygo TTGO T-Relay ESP32 4 relay - to drive both powered Velix window and blinds
Lilygo USB to serial chip T-U2T - to communicate with Lilygo board
Velux KLI 311 - Window wall switch
Velux KLI 312 - Blind wall swtich
21v power supply

Soldering kit
Wiring etc

## How does it work?

1. Connect to ardunio ide using esp32 dev board and serial chip
2. Upload code
3. Configure wifi via serial monitor
4. Connect to apple home using homespan paring setup code - https://github.com/HomeSpan/HomeSpan/blob/master/docs/UserGuide.md
5. Disconnect and reconnect to your permanent 24 dc power supply

## Wiring Diagram?

![Untitled](https://user-images.githubusercontent.com/27014359/219018425-351ac0a4-297d-4795-9bf8-c9a9cbdb733a.png)

<img width="241" alt="Screenshot 2023-02-15 at 11 35 37" src="https://user-images.githubusercontent.com/27014359/219016849-457cc7b6-b6e8-4220-a6d3-c92fa4e376ec.png">
