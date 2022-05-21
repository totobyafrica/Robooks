#include <WTV020SD16P.h>
#include <SFE_ISL29125.h>
#include <Wire.h>
#include <SparkFun_TB6612.h>

//button INPUT declaration
const int buttonP = A6;
const int buttonArray[] = {1,2,3};
const int lowerRange[] = {};
const int upperRange[] = {};
int currentState;
int lastState = LOW;

// MOTOR UNIT
const int EncoderUnit = 58; // 58 E.U. ->> 1 cm

// Motor A

int pwmA = 3;
int in1A = 5;
int in2A = 4;
int encAa = 6;
int encAb = 7;

// Motor B

int pwmB = 10;
int in1B = 11;
int in2B = 12;
int encBa = 8;
int encBb = 9;

int posA = 0;
int posB = 0;

//LED dec
const int ledP = A7;
// Declare sensor object
SFE_ISL29125 RGB_sensor;
unsigned int redlow = 590;
unsigned int redhigh = 3865;
unsigned int greenlow = 961;
unsigned int greenhigh = 5181;
unsigned int bluelow = 773;
unsigned int bluehigh = 4109;
// Declare RGB Values
int redVal = 0;
int greenVal = 0;
int blueVal = 0;

//enum for color objects
enum sColor { white,black,red, yellow, blue, green, undefined };
//AUDIO PLAYER
int resetPin = A0;  // The pin number of the reset pin.
int clockPin = A1;  // The pin number of the clock pin.
int dataPin = A2;  // The pin number of the data pin.
int busyPin = A3;  // The pin number of the busy pin.
//Wtv020sd16p wtv020sd16p(resetPin,clockPin,dataPin,busyPin); // ** FIX LIB ZIP FILE - inaccessible
sColor stateToConsider;
/*
 * INT LOGIC
 * 0 - NEUTRAL
 * 1 - FORWARD
 * 2 - RIGHT
 * 3 - LEFT
 * 4 - BACK
 * >= 5 -> return to NEUTRAL
 */
bool commandAdded;
int commandSeq[20];
int const cSsize = sizeof(commandSeq)/sizeof(commandSeq[0]);
int testArray[5];
int const tAsize = sizeof(testArray)/sizeof(testArray[0]);
//color hp.
struct RGB
{
  int R;
  int G;
  int B;
};

int getCommand(sColor inputColor)
{
  switch (inputColor)
  {
    case red: // move forward
        return 1;
    case green: // turn left
        return 2;
    case blue: // move right
        return 3;
    case yellow: // move back
        return 4;
    default: //neutral - Color undefined
        return 0;
  }
}

sColor spot_color(RGB scan_color)
 {
  if (scan_color.R >= 240 && scan_color.G >= 240 && scan_color.B >= 240) {
   return white;
  }
  else if (scan_color.R <= 25 && scan_color.G <= 25 && scan_color.B <= 25) {
   return black;
  }
  else if ((scan_color.R >= 170) && (scan_color.G <= 40) && (scan_color.B <= 40))
  {
    //wtv020sd16p.playVoice(XXXX);
    return red;
  }
  else if ((scan_color.R >= 200) && (scan_color.G >= 200) && (scan_color.B <= 100))
  {
    //wtv020sd16p.playVoice(XXXX);
    return yellow;
  }
    else if ((scan_color.R <= 60) && (scan_color.G <= 40) && (scan_color.B >= 220))
  {
   // wtv020sd16p.playVoice(XXXX);
    return blue;
  }
    else if ((scan_color.R <= 110) && (scan_color.G >= 170) && (scan_color.B <= 110))
  {
    return green;
  }
  //wtv020sd16p.playVoice(XXXX);
    else
  {
    return undefined;
  }
 }
void resetArray(int integerArray[] , int arraySize)
{
    for (int x = 0; x < arraySize; x++)
    {
        integerArray[x] = 0;
    }
}

 
void setup() {
  resetArray(testArray,tAsize);
  resetArray(commandSeq, cSsize);
  commandAdded = false;
  // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(ledP, OUTPUT);
    pinMode(buttonP, INPUT);
    // Initialize the ISL29125 with simple configuration so it starts sampling
    if (RGB_sensor.init())
    {
      Serial.println("Sensor Initialization Successful\n\r");
    }
    stateToConsider = undefined;
 // wtv020sd16p.reset();
}

void loop() {
  int pin_read = analogRead(buttonP);
  if (pin_read > 700)
  {
  Serial.print("Val: ");
  Serial.println(pin_read);


  digitalWrite(ledP, HIGH);
  // Read sensor values (16 bit integers)
  unsigned int red = RGB_sensor.readRed();
  unsigned int green = RGB_sensor.readGreen();
  unsigned int blue = RGB_sensor.readBlue();
  
  // Convert to RGB values
  int redV = map(red, redlow, redhigh, 0, 255);
  int greenV = map(green, greenlow, greenhigh, 0, 255);
  int blueV = map(blue, bluelow, bluehigh, 0, 255);
  
  // Constrain to values of 0-255
  redVal = constrain(redV, 0, 255);
  greenVal = constrain(greenV, 0, 255);
  blueVal = constrain(blueV, 0, 255);
     
  Serial.print("Red: "); 
  Serial.print(redVal);
  Serial.print(" - Green: ");
  Serial.print(greenVal);
  Serial.print(" - Blue: "); 
  Serial.println(blueVal);
  if ( (redVal > 0) && (greenVal > 0) && (blueVal > 0))
  {
    RGB ScanCol = {redVal, greenVal, blueVal };
    stateToConsider = spot_color(ScanCol);
    
    commandAdded = true;
  }
  
  // Delay for sensor to stabilize
  delay(2000);
  }
digitalWrite(ledP, LOW);

  


}
