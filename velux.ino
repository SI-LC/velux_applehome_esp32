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
//    Uses HomeKit for the ESP32                          //
//    ------------------------------------------------    //
//                                                        //
//                                                        //
////////////////////////////////////////////////////////////

#define RELAY_PIN_1 21                            // pin number for opening relay window
#define RELAY_PIN_2 19                            // pin number for closing relay window
#define RELAY_PIN_3 18                            // pin number for opening relay window shade
#define RELAY_PIN_4 5                             // pin number for closing relay window shade
#define LED_PIN     25                            // pin number for blinking LEDs

#include "HomeSpan.h" 
#include "velux.h" 

void setup() {
  
  Serial.begin(115200);

  homeSpan.begin(Category::Bridges,"Window");     // Bridge name

  new SpanAccessory();  
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      
  new SpanAccessory();                                                          
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Window Shade");   
    new DEV_Window(21,19,1);                      // Window shade open relay pin, close relay pin, relay operation times                                         


  new SpanAccessory();                                                          
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
      new Characteristic::Name("Window");
    new DEV_WindowShade(18,5,1);                 // Window open relay pin, close relay pin, relay operation times

} // end of setup()

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();
  
} // end of loop()
