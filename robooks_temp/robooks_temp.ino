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
unsigned int redlow = 675;
unsigned int redhigh = 4230;
unsigned int greenlow = 863;
unsigned int greenhigh = 5123;
unsigned int bluelow = 628;
unsigned int bluehigh = 3389;
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
 * 0 - NEUTRAL (UNDEFINED COLORS)
 * 1 - FORWARD (RED)
 * 2 - RIGHT (GREEN)
 * 3 - LEFT (BLUE)
 * 4 - BACK (YELLOW)
 * 5 - RESET MEMORY (BLACK)
 * 6 - WRITE TO MEMORY (WHITE)
 * >= 8 -> return to NEUTRAL
 */
bool commandAdded;
int commandSeq[20];
int const cSsize = sizeof(commandSeq)/sizeof(commandSeq[0]);
int testArray[3];
int const tAsize = sizeof(testArray)/sizeof(testArray[0]);
int x_pntr; // test array pos pointer
int memoryPointer; // command seq pointer
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
    case black: // reset memory
        return 5;
    case white: // write to memory
        return 6;
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
bool check(const int array[], int n) // CHECK IF ALL ELEMENTS IN ARRAY ARE EQUAL - USED IN LOGIC CHECK FOR COMMAND TO MEMORY PIPELINE
{   
    for (int i = 0; i < n - 1; i++)      
    {         
        if (array[i] != array[i + 1])
            return true;
    }
    return false;
}
 
void setup() {
  x_pntr = 0;
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
  if (pin_read > 0)
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
    int commandToConsider = getCommand(stateToConsider);
        Serial.println(commandToConsider);
    if (stateToConsider != undefined && commandToConsider > 6 && x_pntr < tAsize)
    {
        testArray[x_pntr] = commandToConsider;
        x_pntr += 1;
        Serial.println(testArray[x_pntr]);
    }
    else if (x_pntr >= tAsize)
    {
        if(check(testArray,tAsize) == true)
        {
            Serial.print(" - ADDED COMMAND NO:");
            Serial.print(testArray[1]); 
            commandSeq[memoryPointer] = testArray[1];
            memoryPointer += 1;
        }
        x_pntr = 0;
    }
  }
  
  // Delay for sensor to stabilize
  delay(2000);
  }
digitalWrite(ledP, LOW);


}
