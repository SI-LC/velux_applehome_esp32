/*********************************************************************************
 *  MIT License
 *
 *  Copyright (c) 2022 Simon Cole
 *
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 ********************************************************************************/

////////////////////////////////////////////////////////////
//                                                        //
//    Velux window + blind for Apple HomeKit (HomeSpan)   //
//    ------------------------------------------------    //
//                                                        //
//    Runs on a LilyGo T-Relay ESP32 (4 relays).          //
//    Each relay is wired across one button on a Velux    //
//    wall switch (KLI 311 for the window, KLI 312 for    //
//    the blind). A HomeKit command pulses the relevant   //
//    relay for 500 ms, which the switch sees as a press. //
//                                                        //
//    Two accessories are exposed to the Home app:        //
//      "Window Shade"  -> relays on GPIO 21 / 19         //
//      "Window"        -> relays on GPIO 18 / 5          //
//                                                        //
////////////////////////////////////////////////////////////

// LilyGo T-Relay GPIO numbers. Kept here for reference; the
// values below in setup() are the ones actually used.
#define RELAY_PIN_1 21                            // relay 1 - "Window Shade" accessory, pulsed when target = 0 (close)
#define RELAY_PIN_2 19                            // relay 2 - "Window Shade" accessory, pulsed when target = 100 (open)
#define RELAY_PIN_3 18                            // relay 3 - "Window" accessory, pulsed when target > current (raise/open)
#define RELAY_PIN_4 5                             // relay 4 - "Window" accessory, pulsed when target < current (lower/close)
#define LED_PIN     25                            // on-board LED (currently unused)

#include "HomeSpan.h"
#include "velux.h"

void setup() {

  Serial.begin(115200);

  homeSpan.begin(Category::Bridges,"Window");     // Bridge name as it appears during pairing

  new SpanAccessory();                            // Accessory 1 is the bridge itself
    new Service::AccessoryInformation();
      new Characteristic::Identify();

  new SpanAccessory();                            // Accessory 2 - shown in the Home app as "Window Shade"
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name("Window Shade");
    new DEV_Window(21,19,1);                      // close relay pin, open relay pin, (unused) click count

  new SpanAccessory();                            // Accessory 3 - shown in the Home app as "Window"
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name("Window");
    new DEV_WindowShade(18,5,1);                  // raise relay pin, lower relay pin, (unused) click count

} // end of setup()

//////////////////////////////////////

void loop(){

  homeSpan.poll();

} // end of loop()
