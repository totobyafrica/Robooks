
#include <WTV020SD16P.h>
#include <SFE_ISL29125.h>
#include <Wire.h>
#include <SparkFun_TB6612.h>
  
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

void SpeedControl(int duration, unsigned long InsertionTime, bool forwardMove )
{
  int CurrSpeed = 70;
  int adder = 1;
  int slower = -1;
  int upperCeil = 170;
  int lowerCeil = 70;
  if (!forwardMove)
  {
    CurrSpeed = -70;
    adder = -1;
    slower = 1;
    lowerCeil = -70;
    upperCeil = -170;
  }
  unsigned long CatchTime = millis();
  while ((CatchTime - InsertionTime) <= duration/2)
    {
      CurrSpeed = CurrSpeed + 1;
      CurrSpeed = constrain(CurrSpeed, lowerCeil, upperCeil);
      CatchTime = millis();
      forward(motor1, motor2,CurrSpeed);
    }
  while ((CatchTime - InsertionTime) <= duration)
  {
      CurrSpeed = CurrSpeed - 1;
      CurrSpeed = constrain(CurrSpeed, lowerCeil, upperCeil);
      CatchTime = millis();
      forward(motor1, motor2,CurrSpeed);
  }
  brake(motor1, motor2);
}

int fwRun = 980; //1250 - original 1.25 block move
void runCommand(int CommandNum) // different to getCommand, used to run motor movements
{
        switch (CommandNum)
        {
        case 1: // move forward
              langToPlay = (1+(11*langCase));
              wtv020sd16p.playVoice(langToPlay);
              Serial.println("Forward - RED");
              forward(motor1, motor2, 100);
              delay(fwRun);
              brake(motor1, motor2);
            break;
        case 2: // turn left
              langToPlay = (2+(11*langCase));
              wtv020sd16p.playVoice(langToPlay);
              Serial.println("Left - GREEN");
              forward(motor1, motor2, 100);
              delay(fwRun);
              brake(motor1, motor2);
              delay(100);
              left(motor1, motor2, 150);
              delay(690);
              brake(motor1, motor2);
            break;
        case 3: // turn right
              langToPlay = (3+(11*langCase));
              wtv020sd16p.playVoice(langToPlay);
              Serial.println("Right - BLUE");
              forward(motor1, motor2, 100);
              delay(fwRun);
              brake(motor1, motor2);
              delay(100);
              right(motor1, motor2, 150);
              delay(690); //og 780 - 15 to 25% slippage
              brake(motor1, motor2);
            break;
        case 4: // move back
              langToPlay = (0+(11*langCase));
              wtv020sd16p.playVoice(langToPlay);
              Serial.println("Back - YELLOW");
              back(motor1, motor2, 100);
              delay(fwRun);
              brake(motor1, motor2);
            break;
        default: //neutral - Color undefined
            break;
        }
}

sColor spot_color(RGB scan_color)
 {
  if (scan_color.R >= 189 && scan_color.G >= 189 && scan_color.B >= 189) {
   return white;
  }
  else if (scan_color.R <= 3 && scan_color.G <= 3 && scan_color.B <= 3) {
   return black;
  }
    else if ((scan_color.R >= 185) && (scan_color.G >= 79) && (scan_color.B <= 58))
  {
    //wtv020sd16p.playVoice(XXXX);
    return yellow;
  }
  else if ((scan_color.R >= 78) && (scan_color.G <= 18) && (scan_color.B <= 18))
  {
    //wtv020sd16p.playVoice(XXXX);
    return red;
  }
    else if ((scan_color.R <= 6) && (scan_color.G <= 20) && (scan_color.B >= 27))
  {
   // wtv020sd16p.playVoice(XXXX);
    return blue;
  }
    else if ((scan_color.R <= 6) && (scan_color.G >= 17) && (scan_color.B >= 15))
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

void blinkLED(int LedPin, int NumBlinks)
{
  int Blinks = 0;
  while (Blinks < NumBlinks)
  {
    digitalWrite(LedPin, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(250);                       // wait for a second
    digitalWrite(LedPin, LOW);    // turn the LED off by making the voltage LOW
    delay(250); 
    Blinks++;
  }
  digitalWrite(LedPin, HIGH); 

}

bool checkArr(const int array[], int n) // CHECK IF ALL ELEMENTS IN ARRAY ARE EQUAL - USED IN LOGIC CHECK FOR COMMAND TO MEMORY PIPELINE
{   
    for (int i = 0; i < n - 1; i++)      
    {         
        if (array[i] != array[i + 1])
            return true;
    }
    return false;
}

void printArray(int a[],int n)
{
  for(int i=0; i<n ;i++){
      Serial.println("ELEMENT: ");
      Serial.println(i);
      Serial.println("E. VALUE: ");
      Serial.println(a[i]);
  }
} 
unsigned int langHit = 0;
unsigned int colorHit = 0;
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
  // put your main code here, to run repeatedly:
    int pin_read = analogRead(buttonP);

    if (pin_read >= 851 && pin_read <= 856)
    {
    Serial.print("power in val: ");
    Serial.print(pin_read);
    Serial.println("");
      langHit++;
    Serial.println("Val hits: ");
    Serial.println("Lang hits: ");
    Serial.println(langHit);
    Serial.println("Col hits: ");
    Serial.println(colorHit);
    }
    if (pin_read >= 350)
    {
    Serial.print("power in val: ");
    Serial.print(pin_read);
    Serial.println("");
      colorHit++;
    Serial.print("Val hits: ");
    Serial.print("Lang hits: ");
    Serial.println(langHit);
    Serial.print("Col hits: ");
    Serial.println(colorHit);
    }
    delay(150);
    pin_read = 0;

}
