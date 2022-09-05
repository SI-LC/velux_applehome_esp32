
////////////////////////////////////
//   DEVICE-SPECIFIC SERVICES     //
////////////////////////////////////

struct DEV_Window : Service::Window {                           // Window Door Opener

  int relayPin1;                                                // relay pin for opening
  int relayPin2;                                                // relay pin for closing
  int nClicks;                                                  // number of times to operate the relay
  SpanCharacteristic *current;                                  // reference to the Current State Characteristic
  SpanCharacteristic *target;                                   // reference to the Target State Characteristic
  SpanCharacteristic *position;                                 // reference to the Actual State Characteristic

  SpanCharacteristic *power;

  DEV_Window(int relayPin1, int relayPin2, int nClicks) : Service::Window(){       
        
    current=new Characteristic::CurrentPosition(0);              // initial value of 0 means closed
    target=new Characteristic::TargetPosition(0);                // initial value of 0 means closed
    target->setRange(0,100,100);                                 // set the allowable target-position range to 0-100 IN STEPS of 100
    position=new Characteristic::PositionState(0);               // initial value of 0 means closed

    this->relayPin1=relayPin1;
    this->relayPin2=relayPin2;

    this->nClicks=nClicks;
    pinMode(relayPin1,OUTPUT);                                   // set the mode for this pin to be an OUTPUT (standard Arduino function)
    pinMode(relayPin2,OUTPUT);                                   // set the mode for this pin to be an OUTPUT (standard Arduino function)
    
    Serial.print("Configuring Velux Window Opener: Pin=");       // initialization message
    Serial.print(relayPin1,relayPin2);
    Serial.print("  Clicks=");                                   // add output message for number of relay operations
    Serial.print(nClicks);
    Serial.print("\n");

  } // end constructor

  boolean update(){                                              // update() method

    // see HAP Documentation for details on what each value represents

    if(target->getNewVal()==0){                                  // if the target-state value is set to 0, requesting the window to be in open position
      LOG1("Opening Velux Window\n");
      LOG1(relayPin1);
      
      digitalWrite(relayPin1,HIGH);                             // turn relay1 on
      delay(500);                                               // wait 500 ms
      digitalWrite(relayPin1,LOW);                              // turn relay1 off
      delay(250);
      current->setVal(2);                                       // set the current-state value to 2, which means "opening"
      position->setVal(2);                                      

      for(int i=0;i<nClicks;i++){                               // loop over number of clicks

      }
      
    } else {
      LOG1("Closing Velux Window\n");                           // else the target-state value is set to 1, and HomeKit is requesting the door to be in the closed position
      LOG1(relayPin2);
      digitalWrite(relayPin2,HIGH);                             // turn relay2 on
      delay(500);                                               // wait 500 ms
      digitalWrite(relayPin2,LOW);                              // turn relay2 off
      delay(250);                                               // wait 250 ms
      current->setVal(3);                                       // set the current-state value to 3, which means "closing"         
      position->setVal(2);                                      // clear any prior obstruction detection
    }
    
    return(true);                                               // return true
  
  } // update

  void loop(){                                                  // loop() method

    if(current->getVal()==target->getVal())                     // if current-state matches target-state there is nothing do -- exit loop()
      return;

    if(current->getVal()==4)                                    // if the current-state is stopped, there is nothing more to do - exit loop()     
      return;

    // This last bit of code only gets called if the window is in a state that represents actively opening or actively closing.
    // If there is an obstruction, the door is "stopped" and won't start again until the Controller requests a new open or close action

    if(target->timeVal()>5000)                     // simulate a window that takes 5 seconds to operate by monitoring time since target-state was last modified
      current->setVal(target->getVal());           // set the current-state to the target-state
       
  } // loop
  
};

////////////////////////////////////

struct DEV_WindowShade : Service::WindowCovering {     // A motorized Window Shade with Hold Feature

  
  int relayPin3;                                   // pin number defined for this LED
  int relayPin4;                                   // pin number defined for this LED
  int nClicks;                                      // NEW! number of times to blink

  SpanCharacteristic *current;                     // reference to a "generic" Current Position Characteristic (used by a variety of different Service)
  SpanCharacteristic *target;                      // reference to a "generic" Target Position Characteristic (used by a variety of different Service)

  DEV_WindowShade(int relayPin3, int relayPin4, int nClicks) : Service::WindowCovering(){       // constructor() method
        
    current=new Characteristic::CurrentPosition(0);     // Window Shades have positions that range from 0 (fully lowered) to 100 (fully raised)
    
    target=new Characteristic::TargetPosition(0);       // Window Shades have positions that range from 0 (fully lowered) to 100 (fully raised)
    target->setRange(0,100,100);                         // set the allowable target-position range to 0-100 IN STEPS of 100

    this->relayPin3=relayPin3;                            // don't forget to store relayPin...
    this->relayPin4=relayPin4;                            // don't forget to store relayPin...

    this->nClicks=nClicks;
    pinMode(relayPin3,OUTPUT);                         // ...and set the mode for ledPin to be an OUTPUT (standard Arduino function)
    pinMode(relayPin4,OUTPUT);                         // ...and set the mode for ledPin to be an OUTPUT (standard Arduino function)
    
    Serial.print("Configuring Motorized Window Shade");   // initialization message
    Serial.print(relayPin3,relayPin4);
    Serial.print("  Blinks=");                       // NEW! add output message for number of blinks
    Serial.print(nClicks);
    Serial.print("\n");

  } // end constructor

  boolean update(){                              // update() method

    // The logic below is based on how HomeKit appears to operate in practice, which is NOT consistent with
    // HAP documentation.  In that document HomeKit seems to support fully opening or fully closing a window shade, with
    // an optional control to HOLD the window shade at a given in-between position while it is moving.

    // In practice, HomeKit does not appear to implement any form of a HOLD control button, even if you instantiate that
    // Characteristic.  Instead, HomeKit provides a full slider control, similar to the brightness control for a lightbulb,
    // that allows you to set the exact position of the window shade from 0-100%.  This obviates the need to any sort of HOLD button.
    // The resulting logic is also very simple:

    if(target->getNewVal()>current->getVal()){      // if the target-position requested is greater than the current-position, simply log a "raise" message  
      LOG1("Raising Shade\n");                      // ** there is nothing more to do - HomeKit keeps track of the current-position so knows raising is required
      
      LOG1(relayPin3);
      digitalWrite(relayPin3,HIGH);                    // turn pin on
      delay(500);                                   // wait 100 ms
      digitalWrite(relayPin3,LOW);                     // turn pin off
      delay(250);                                   // wait 250 ms
      
    } else 
    if(target->getNewVal()<current->getVal()){      // if the target-position requested is less than the current-position, simply log a "raise" message  
      LOG1("Lowering Shade\n");                     // ** there is nothing more to do - HomeKit keeps track of the current-position so knows lowering is required

      LOG1(relayPin4);
      digitalWrite(relayPin4,HIGH);                    // turn pin on
      delay(500);                                   // wait 100 ms
      digitalWrite(relayPin4,LOW);                     // turn pin off
      delay(250);                                   // wait 250 ms
    }
        
    return(true);                               // return true
  
  } // update

  void loop(){                                     // loop() method

    // Here we simulate a window shade that takes 5 seconds to move to its new target posiiton
    
    if(current->getVal()!=target->getVal() && target->timeVal()>5000){          // if 5 seconds have elapsed since the target-position was last modified...
      current->setVal(target->getVal());                                        // ...set the current position to equal the target position
    }

    // Note there is no reason to send continuous updates of the current position to the HomeKit.  HomeKit does NOT display the
    // current position.  Rather, it simply compares the value of the current position to the value of target positon as set by the
    // the user in the Home App.  If it finds current and target positions are the same, it knows the shade is stopped.  Otherwise
    // it will report the shade is raising or lowering depending on whether the specified target state is greater or less than
    // the current state.

    // According to HAP, the Characteristic Position State is also required.  However, this seems duplicative and is NOT needed
    // at all given the way HomeKit uses current position.  HomeSpan will warn you if Position State is not defined (since it 
    // is technically required) but this works fine without it.
    
  } // loop
  
};
