
const int buttonP = A7;
const int buttonArray[] = {1,2,3};
const int lowerRange[] = {};
const int upperRange[] = {};
int currentState;
int lastState = LOW;

void setup() {
  // put your setup code here, to run once:
  pinMode(buttonP, INPUT);
  Serial.begin(9600);
}


void loop() {
  // put your main code here, to run repeatedly:
  int pin_read = analogRead(buttonP);
  if (pin_read > 0){
    Serial.print("Val: ");
    Serial.println(pin_read);
  }
}


void checkButton(){
  int reading = analogRead(buttonP);
  int tmpState = LOW;
  int buttonAmnt = 2;
  if(reading > 15){
    for(int i = buttonAmnt;i--;i>=0)
    {
     if(reading >= lowerRange[i] && reading <= upperRange[i])
     {
      currentState = lastState;
     }
    }
  }
}