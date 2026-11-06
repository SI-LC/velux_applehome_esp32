////////////////////////////////////
//   DEVICE-SPECIFIC SERVICES     //
////////////////////////////////////

// DEV_Window exposes a HomeKit "Window" service and drives two relays.
//
// HomeKit sends TargetPosition as a percentage: 0 = closed, 100 = open.
// The target range is limited to steps of 100, so the Home app only ever
// sends 0 or 100 - i.e. fully close or fully open.
//
// Each command pulses one relay for 500 ms (a "button press" on the Velux
// wall switch) and then waits 250 ms so two presses can't overlap. The
// delay() calls block HomeSpan briefly, which is intentional and harmless.

struct DEV_Window : Service::Window {

  int relayPin1;                                                // relay pulsed when HomeKit asks for target = 0 (close)
  int relayPin2;                                                // relay pulsed when HomeKit asks for target = 100 (open)
  int nClicks;                                                  // unused - reserved for repeating the pulse
  SpanCharacteristic *current;                                  // CurrentPosition (0-100 %)
  SpanCharacteristic *target;                                   // TargetPosition  (0-100 %)
  SpanCharacteristic *position;                                 // PositionState   (0 = closing, 1 = opening, 2 = stopped)

  SpanCharacteristic *power;                                    // unused

  DEV_Window(int relayPin1, int relayPin2, int nClicks) : Service::Window(){

    current=new Characteristic::CurrentPosition(0);              // start as closed (0 %)
    target=new Characteristic::TargetPosition(0);                // start as closed (0 %)
    target->setRange(0,100,100);                                 // only allow 0 or 100 - no partial positions
    position=new Characteristic::PositionState(0);               // initial state

    this->relayPin1=relayPin1;
    this->relayPin2=relayPin2;

    this->nClicks=nClicks;
    pinMode(relayPin1,OUTPUT);
    pinMode(relayPin2,OUTPUT);

    Serial.print("Configuring Velux Window: close pin=");       // initialization message
    Serial.print(relayPin1);
    Serial.print("  open pin=");
    Serial.print(relayPin2);
    Serial.print("  Clicks=");
    Serial.print(nClicks);
    Serial.print("\n");

  } // end constructor

  boolean update(){                                              // called by HomeSpan when the Home app changes TargetPosition

    if(target->getNewVal()==0){                                  // target = 0 -> HomeKit wants the window CLOSED
      LOG1("Window target 0 (close) - pulsing relay on pin ");
      LOG1(relayPin1);
      LOG1("\n");

      digitalWrite(relayPin1,HIGH);                             // relay on  (press the wall-switch button)
      delay(500);                                               // hold for 500 ms
      digitalWrite(relayPin1,LOW);                              // relay off (release the button)
      delay(250);                                               // settle before anything else
      current->setVal(2);                                       // nudge current away from target so Home shows "moving" until loop() settles it
      position->setVal(2);                                      // PositionState 2 = stopped (relay pulse is over)

      for(int i=0;i<nClicks;i++){                               // placeholder - nClicks is not used yet

      }

    } else {                                                    // target = 100 -> HomeKit wants the window OPEN
      LOG1("Window target 100 (open) - pulsing relay on pin ");
      LOG1(relayPin2);
      LOG1("\n");
      digitalWrite(relayPin2,HIGH);                             // relay on  (press the wall-switch button)
      delay(500);                                               // hold for 500 ms
      digitalWrite(relayPin2,LOW);                              // relay off (release the button)
      delay(250);                                               // settle before anything else
      current->setVal(3);                                       // nudge current away from target so Home shows "moving" until loop() settles it
      position->setVal(2);                                      // PositionState 2 = stopped (relay pulse is over)
    }

    return(true);                                               // tell HomeSpan the update was accepted

  } // update

  void loop(){                                                  // called by HomeSpan continuously

    if(current->getVal()==target->getVal())                     // nothing in progress - exit
      return;

    if(current->getVal()==4)                                    // never true with the values used above; kept as-is
      return;

    // We have no feedback from the Velux motor, so simulate a window that takes
    // 5 seconds to move: once 5 s have passed since the target changed, report
    // that the current position now equals the target.

    if(target->timeVal()>5000)
      current->setVal(target->getVal());

  } // loop

};

////////////////////////////////////

// DEV_WindowShade exposes a HomeKit "Window Covering" service and drives two relays.
//
// Same idea as DEV_Window, but the direction is decided by comparing the new
// target to the current position rather than checking for 0:
//   target > current  -> pulse relayPin3 (raise / open)
//   target < current  -> pulse relayPin4 (lower / close)
// Because the target range is in steps of 100, the Home app only sends 0 or 100.

struct DEV_WindowShade : Service::WindowCovering {

  int relayPin3;                                   // relay pulsed to raise / open
  int relayPin4;                                   // relay pulsed to lower / close
  int nClicks;                                     // unused - reserved for repeating the pulse

  SpanCharacteristic *current;                     // CurrentPosition (0 = fully lowered, 100 = fully raised)
  SpanCharacteristic *target;                      // TargetPosition  (0 = fully lowered, 100 = fully raised)

  DEV_WindowShade(int relayPin3, int relayPin4, int nClicks) : Service::WindowCovering(){

    current=new Characteristic::CurrentPosition(0);     // start as fully lowered (0 %)

    target=new Characteristic::TargetPosition(0);       // start as fully lowered (0 %)
    target->setRange(0,100,100);                        // only allow 0 or 100 - no partial positions

    this->relayPin3=relayPin3;
    this->relayPin4=relayPin4;

    this->nClicks=nClicks;
    pinMode(relayPin3,OUTPUT);
    pinMode(relayPin4,OUTPUT);

    Serial.print("Configuring Window Covering: raise pin=");   // initialization message
    Serial.print(relayPin3);
    Serial.print("  lower pin=");
    Serial.print(relayPin4);
    Serial.print("  Clicks=");
    Serial.print(nClicks);
    Serial.print("\n");

  } // end constructor

  boolean update(){                              // called by HomeSpan when the Home app changes TargetPosition

    // HomeKit shows a 0-100 slider for window coverings (no separate HOLD button),
    // and it tracks the current position itself. So all we need to do is work out
    // which direction was requested and press the matching button.

    if(target->getNewVal()>current->getVal()){      // target above current -> raise / open
      LOG1("Covering raising - pulsing relay on pin ");
      LOG1(relayPin3);
      LOG1("\n");
      digitalWrite(relayPin3,HIGH);                 // relay on  (press the wall-switch button)
      delay(500);                                   // hold for 500 ms
      digitalWrite(relayPin3,LOW);                  // relay off (release the button)
      delay(250);                                   // settle before anything else

    } else
    if(target->getNewVal()<current->getVal()){      // target below current -> lower / close
      LOG1("Covering lowering - pulsing relay on pin ");
      LOG1(relayPin4);
      LOG1("\n");
      digitalWrite(relayPin4,HIGH);                 // relay on  (press the wall-switch button)
      delay(500);                                   // hold for 500 ms
      digitalWrite(relayPin4,LOW);                  // relay off (release the button)
      delay(250);                                   // settle before anything else
    }

    return(true);                                   // tell HomeSpan the update was accepted

  } // update

  void loop(){                                     // called by HomeSpan continuously

    // No feedback from the Velux motor, so simulate a covering that takes
    // 5 seconds to move: once 5 s have passed since the target changed, report
    // that the current position now equals the target.

    if(current->getVal()!=target->getVal() && target->timeVal()>5000){
      current->setVal(target->getVal());
    }

    // There's no need to stream intermediate positions to HomeKit. The Home app
    // simply compares current to target: equal means stopped, otherwise it shows
    // "raising" or "lowering" depending on which way the target lies.

    // HAP lists PositionState as required for Window Covering, but HomeKit works
    // fine without it here (HomeSpan prints a warning at startup, which is safe to ignore).

  } // loop

};
