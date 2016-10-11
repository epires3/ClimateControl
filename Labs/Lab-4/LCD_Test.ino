// include the library code:
#include <LiquidCrystal.h>
#include <DHT.h>
#define DHTTYPE DHT22
#define DHTPIN 7

DHT dht(DHTPIN, DHTTYPE);

/*
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 8
 * LCD D7 pin to digital pin 6
 * LCD R/W pin to GND
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
*/

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 8, 6);


const int led = 13;
const int pir = 10;
const int upButton = 3;
const int downButton = 2; 
const int cool_fan = A1;
const int heat_fan = A0;

int current_temp, set_temp;
int lastUpState = HIGH;
int lastDownState = HIGH;
int lastPirState = LOW; 
int roomCnt = 0;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2); 
  lcd.print("TEMP  SET  HUM%");
  Serial.begin(9600);
  pinMode(pir,INPUT);
  pinMode(upButton,INPUT_PULLUP);
  pinMode(downButton,INPUT_PULLUP);
  pinMode(cool_fan,OUTPUT);
  pinMode(heat_fan,OUTPUT);
  pinMode(led,OUTPUT);
}

void loop() {
 current_temp = dht.readTemperature();
 temp_RW();
 fanControl();
 lightControl();
}

void temp_RW(){
  //delay(2000);
  upPress();
  downPress();
  lcd.setCursor(0,1);
  lcd.print(current_temp);
  lcd.print("C");
  lcd.print("   ");
  if(set_temp == 0)
    set_temp = current_temp; 
  lcd.print(set_temp);
  lcd.print("C");
  lcd.print("   ");
  lcd.print(dht.readHumidity());
  //delay(2000);
}

void upPress(){
   // read the pushbutton input pin:
  int buttonState = digitalRead(upButton);
  // compare the buttonState to its previous state
  if (buttonState != lastUpState) {
    // if the state has changed, increment the counter
    if (buttonState == LOW) 
      set_temp++;
    // Delay a little bit to avoid bouncing
    delay(50);
  }
  // save the current state as the last state,
  lastUpState = buttonState;
}

void downPress(){
   // read the pushbutton input pin:
  int buttonState = digitalRead(downButton);
  // compare the buttonState to its previous state
  if (buttonState != lastDownState) {
    // if the state has changed, increment the counter
    if (buttonState == LOW) 
      set_temp--;
    // Delay a little bit to avoid bouncing
    delay(50);
  }
  // save the current state as the last state,
  lastDownState = buttonState;
}

void fanControl()
{
  // Push fan is activated, Pull is deactivated (Cools room)
  if(current_temp > set_temp){
    digitalWrite(heat_fan,LOW);
    digitalWrite(cool_fan,HIGH);
  }
  // Pull fan is activated, Push is deactivated (Heats room)
  else if(current_temp < set_temp){
    digitalWrite(cool_fan,LOW);
    digitalWrite(heat_fan,HIGH);
  }
  // Push and Pull fans deactivated (Temperature matches user input)
  else{
    digitalWrite(cool_fan,LOW);
    digitalWrite(heat_fan,LOW);
  }
}

void lightControl(){
  pirCount();
  if(digitalRead(pir) == HIGH)
    digitalWrite(led,HIGH);
  else
    digitalWrite(led,LOW);
}

void pirCount(){
   // read the out pin:
  int currentState = digitalRead(pir);
  // compare the currentState to its previous state
  if (currentState != lastPirState) {
    // if the state has changed, increment the counter
    if (currentState == LOW) 
      Serial.println(++roomCnt);
  }
  // save the current state as the last state,
  lastPirState = currentState;
}



