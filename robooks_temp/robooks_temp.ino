#include <WTV020SD16P.h>
#include <SFE_ISL29125.h>
#include <Wire.h>
#include <SparkFun_TB6612.h>
#include "color_recognition.h"


// LANG VARIABLE HERE
const int langSet = 11;
int langCase = 0;
const int langCeil = 3;
unsigned int langToPlay = 0;
// This value (langCase) is capped at 3, due to there
// being 4 languages (that is: Hungarian, Romanian, Slovakian, English)
// ordered in that order 0-HU 1-RO 2-SK 3-EN



//button INPUT declaration
const int buttonP = A7;
const int buttonArray[] = {1,2,3};
const int lowerRange[] = {};
const int upperRange[] = {};
int currentState;
int lastState = LOW;

// MOTOR UNIT
const int EncoderUnit = 58; // 58 E.U. ->> 1 cm

#define AIN1 9
#define BIN1 8
#define AIN2 10
#define BIN2 7
#define PWMA 11
#define PWMB 6
#define STBY 12

// these constants are used to allow you to make your motor configuration 
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = -1;
const int offsetB = 1;

// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.
Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY);

//Speaker / Sound Unit
int resetPin = A0;  // The pin number of the reset pin.
int clockPin = A1;  // The pin number of the clock pin.
int dataPin = A2;  // The pin number of the data pin.
int busyPin = A3;  // The pin number of the busy pin.
WTV020SD16P wtv020sd16p(resetPin,clockPin,dataPin,busyPin);



//LED dec
const int ledP = 13;
// Declare sensor object
SFE_ISL29125 RGB_sensor;
unsigned int redlow = 52;
unsigned int redhigh = 3200;
unsigned int greenlow = 81;
unsigned int greenhigh = 4716;
unsigned int bluelow = 66;
unsigned int bluehigh = 3511;
// Declare RGB Values
int redVal = 0;
int greenVal = 0;
int blueVal = 0;

//enum for color objects
enum sColor { white,black,red, yellow, blue, green, undefined };


// Rotary Encoder Inputs
// Define pins 2, 3 if using two motors, configured for testing motor A
 
 #define inputCLK_B 2 
 #define inputDT_B 3
 
 #define inputCLK_A 4 
 #define inputDT_A 5 


sColor stateToConsider;
/*
 * INT LOGIC
 * 0 - NEUTRAL (UNDEFINED COLORS)
 * 1 - FORWARD (RED)
 * 2 - RIGHT (BLUE)
 * 3 - LEFT (GREEN)
 * 4 - BACK (YELLOW)
 * 5 - RESET MEMORY (BLACK)
 * 6 - WRITE TO MEMORY (WHITE)
 * >= 8 -> return to NEUTRAL
 */
bool commandAdded;
int commandSeq[20];
int const cSsize = sizeof(commandSeq)/sizeof(int);
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
    wtv020sd16p.reset();
}

void loop() {
  int pin_read = analogRead(buttonP);
  if (pin_read >= 851 && pin_read <= 853 && commandSeq[0] == 0 && commandSeq[1] == 0)
   {
   // Alternative modulo
   // langCase = (langCase + 1) % 4;
   langCase++;
   if (langCase > langCeil){
    langCase = 0;
   }
   Serial.print("Val: ");
   Serial.println(pin_read);
   Serial.print("Lang Change to: ");
   Serial.println(langCase);
   langToPlay = (4+(11*langCase));
   wtv020sd16p.playVoice(langToPlay);
   Serial.println(langCase);
   blinkLED(ledP,2);
   delay(1000);
  }
 else if (pin_read >= 350 ) // BUTTON VOLTAGE --> FOR REAL TEST USE 350
  //if (red > 20 || green > 20 || blue > 20)
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
  if ( (redVal >= 0) && (greenVal >= 0) && (blueVal >= 0))
  {
    RGB ScanCol = {redVal, greenVal, blueVal };
    stateToConsider = spot_color(ScanCol);
    int commandToConsider = getCommand(stateToConsider);
        Serial.println(commandToConsider);
    if ((stateToConsider != undefined && commandToConsider > 0 && x_pntr < tAsize) || (testArray[0] != 0 && x_pntr < tAsize))
    {
      if(testArray[0] != commandToConsider)
      {
        resetArray(testArray, tAsize);
        x_pntr = 0;
      }
        testArray[x_pntr] = commandToConsider;
        x_pntr += 1;
        Serial.println("x_pntr:");
        Serial.print(x_pntr);
        Serial.println(testArray[x_pntr]);
    }
    else if (x_pntr >= tAsize)
    {
      printArray(testArray, tAsize);
        if(checkArr(testArray,tAsize) != true)
        {
          if(testArray[1] == 6) //scanned color WHITE & passed test array
          {
          blinkLED(ledP,5);  
          langToPlay = (9+(11*langCase));
          wtv020sd16p.playVoice(langToPlay);
          delay(1000);
          Serial.print("STARTING MEMORY");
          for(int i=0; i<cSsize;i++)
          {
            if(commandSeq[i] != 0)
            {
            delay(250);
            Serial.println("currently running seq ");
            Serial.print(commandSeq[i]);
            runCommand(commandSeq[i]);
            delay(250);
            }

          }
          }
          else if(testArray[1] == 5) //scanned color BLACK & passed test array
          {
          blinkLED(ledP,4);
          langToPlay = (10+(11*langCase));
          wtv020sd16p.playVoice(langToPlay);
          Serial.print("RESETTING MEMORY");
          resetArray(commandSeq, cSsize);
          memoryPointer = 0;
          }
          else  //scanned color OTHER THAN UNDEFINED & passed test array
          {
            blinkLED(ledP,2);
            switch (testArray[1])
            {
            case 1:
              langToPlay = (5+(11*langCase));
              wtv020sd16p.playVoice(langToPlay);
              break;
            case 2:
              langToPlay = (6+(11*langCase));
              wtv020sd16p.playVoice(langToPlay);
              break;
            case 3:
              langToPlay = (7+(11*langCase));
              wtv020sd16p.playVoice(langToPlay);
              break;
            case 4:
              langToPlay = (8+(11*langCase));
              wtv020sd16p.playVoice(langToPlay);
              break;
           default: //neutral - Color undefined
              break;
            }
            Serial.print(" - ADDED COMMAND NO:");
            Serial.print(testArray[1]); 
            commandSeq[memoryPointer] = testArray[1];
            memoryPointer += 1;
          }
        }
        resetArray(testArray, tAsize);
        x_pntr = 0;
    }
  }
  
  // Delay for sensor to stabilize
  delay(380); //base is 670 - set to 380 / 670 if 380 proves to be impractical | faulty 
  }
digitalWrite(ledP, LOW);
}
