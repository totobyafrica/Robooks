#include <SFE_ISL29125.h>
#include <Wire.h>
#include <SparkFun_TB6612.h>
  

unsigned long insertion; //time variable


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



//LED dec
const int ledP = 13;
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

void runCommand(int CommandNum) // different to getCommand, used to run motor movements
{
        switch (CommandNum)
        {
        case 1: // move forward
              langToPlay = (1+(11*langCase));
              Serial.println("Forward - RED");
              insertion = millis();
              SpeedControl(900,insertion,true);
              brake(motor1, motor2);
            break;
        case 2: // turn left
              langToPlay = (2+(11*langCase));
              Serial.println("Left - GREEN");
              forward(motor1, motor2, 100);
              delay(1250);
              brake(motor1, motor2);
              delay(100);
              left(motor1, motor2, 150);
              delay(780);
              brake(motor1, motor2);
            break;
        case 3: // move right
              langToPlay = (3+(11*langCase));
              Serial.println("Right - BLUE");
              forward(motor1, motor2, 100);
              delay(1250);
              brake(motor1, motor2);
              delay(100);
              right(motor1, motor2, 150);
              delay(780);
              brake(motor1, motor2);
            break;
        case 4: // move back
              langToPlay = (0+(11*langCase));
              Serial.println("Back - YELLOW");
              insertion = millis();
              SpeedControl(900,insertion,false);
              brake(motor1, motor2);
            break;
        default: //neutral - Color undefined
            break;
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
    else if ((scan_color.R >= 170) && (scan_color.G >= 170) && (scan_color.B <= 140))
  {
    return yellow;
  }
  else if ((scan_color.R >= 100) && (scan_color.G <= 80) && (scan_color.B <= 80))
  {
    return red;
  }
    else if ((scan_color.R <= 80) && (scan_color.G <= 80) && (scan_color.B >= 80))
  {
    return blue;
  }
    else if ((scan_color.R <= 50) && (scan_color.G >= 30) && (scan_color.B >= 31))
  {
    return green;
  }
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
/// InsertionTime is the time at which the function is called - call millis beforehand and start
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
}

void loop() {
  int pin_read = analogRead(buttonP);
  if (pin_read >= 845 && pin_read <= 870 && commandSeq[0] == 0 && commandSeq[1] == 0)
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
              break;
            case 2:
              langToPlay = (6+(11*langCase));
              break;
            case 3:
              langToPlay = (7+(11*langCase));
              break;
            case 4:
              langToPlay = (8+(11*langCase));
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
  delay(670);
  }
digitalWrite(ledP, LOW);
}
