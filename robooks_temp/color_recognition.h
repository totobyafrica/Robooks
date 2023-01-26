#include <WTV020SD16P.h>
#include <SFE_ISL29125.h>
#include <Wire.h>
#include <SparkFun_TB6612.h>
#include "robooks_temp.ino"

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
  else if ((scan_color.R >= scan_color.G + scan_color.B) || ((scan_color.R >= 78) && (scan_color.G <= 18) && (scan_color.B <= 18)))
  {
    //wtv020sd16p.playVoice(XXXX);
    return red;
  }
    else if ((scan_color.B >= scan_color.R + scan_color.G) || ((scan_color.R <= 6) && (scan_color.G <= 20) && (scan_color.B >= 27)))
  {
   // wtv020sd16p.playVoice(XXXX);
    return blue;
  }
    else if ((scan_color.G >= scan_color.R + scan_color.B) || ((scan_color.R <= 6) && (scan_color.G >= 17) && (scan_color.B >= 15)))
  {
    return green;
  }
  //wtv020sd16p.playVoice(XXXX);
    else
  {
    return undefined;
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