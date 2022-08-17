#include <WTV020SD16P.h>

/*
 Example: Control a WTV020-SD-16P module to play voices from an Arduino board.
 Created by Diego J. Arevalo, August 6th, 2012.
 Released into the public domain.
 */

int resetPin = A0;  // The pin number of the reset pin.
int clockPin = A1;  // The pin number of the clock pin.
int dataPin = A2;  // The pin number of the data pin.
int busyPin = A3;  // The pin number of the busy pin.

/*
Create an instance of the Wtv020sd16p class.
 1st parameter: Reset pin number.
 2nd parameter: Clock pin number.
 3rd parameter: Data pin number.
 4th parameter: Busy pin number.
 */
WTV020SD16P wtv020sd16p(resetPin,clockPin,dataPin,busyPin);

void setup() {
  //Initializes the module.
  wtv020sd16p.reset();
  Serial.begin(9600);
}

void loop() {
     Serial.println("ZERO");
     wtv020sd16p.playVoice(0);
     delay(2000);
     Serial.println("ONE");
     wtv020sd16p.playVoice(1);
     delay(2000);
     Serial.println("TWO");
     wtv020sd16p.playVoice(2);
     delay(2000);
     Serial.println("THREE");
     wtv020sd16p.playVoice(3);
     delay(2000);
     Serial.println("FOUR");
     wtv020sd16p.playVoice(4);
     delay(2000);
}
