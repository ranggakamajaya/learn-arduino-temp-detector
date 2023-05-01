//import library
#include <LiquidCrystal.h>
#include <IRremote.h>

//define pin
#define RS 7
#define E  6
#define D4 5
#define D5 4
#define D6 3
#define D7 2
#define IR 9
#define sensorTMP36 A0
#define blueLED 10
#define greenLED 11
#define yellowLED 12
#define redLED 13

LiquidCrystal lcd(RS, E, D4, D5, D6, D7); //define LCD pin

byte degree[] = { 7, 5, 7, 0, 0, 0, 0, 0 }; //for showing degree symbol
  
int tempUnit = 0; // default Celsius
// 0 Celsius
// 1 Fahrenheit

IRrecv irrecv(IR); // define IR receiver pin
decode_results results; // store IR signals

bool systemPower;

void setup() {
  Serial.begin(9600); //initialize serial monitor
  systemPower = true;
  irrecv.enableIRIn(); // enable IR receiver
  irrecv.blink13(true); //blink when detect IR
  
  lcd.begin(16, 2);
  lcd.createChar(0, degree);
  lcd.clear();
  lcd.print("Temperature");

  delay(2000);
  delline(1);
}


//create a function to show temperature level
String msg;
void Temperature ( int v ) {
  msg = "";
  if (v >= 56 && v <= 125) {

    msg =  " HOT    ";
  }
  else if (v >= 40  && v <= 55) {

    msg =  " WARM   ";
  }
  else if (v >= 25  && v <= 39) {

    msg = " NORMAL ";
  }
  else if (v >= 10  && v <= 24) {

    msg = " COOL   ";
  }
  else if (v >= 0   && v <= 9)  {

    msg = " COLD   ";
  }
  else if (v >= -40 && v <= -1) {

    msg =  " FREEZE ";
  }
}


//create a function to show LED indicator based on temperature
void ShowLED ( int v ) {
  if (v >= 40 && v <= 125){
    digitalWrite(yellowLED, HIGH);
    digitalWrite(greenLED,LOW);
    digitalWrite(blueLED,LOW);
  }
  else if (v >= 15  && v <40) {
	digitalWrite(yellowLED,LOW);
    digitalWrite(greenLED,HIGH);
    digitalWrite(blueLED,LOW);
  }
  else if (v >= -40 && v < 15) {
	digitalWrite(yellowLED,LOW);
    digitalWrite(greenLED,LOW);
    digitalWrite(blueLED,HIGH);
  }
}


float AnalogToTemp(int x) {
  float outputVolt = x / 1023.0 * 5015;
  // *5015 to achive 750 mV which is the standard for TMP36 in room temperatur(25C)
  float tempC = (outputVolt-500)/10; //basic convertion for TMP36
  return tempC;
}

//create a function to clear line
void delline(int line) {
  lcd.setCursor(0, line);
  lcd.print("                     ");
  lcd.setCursor(0, line);
}

// LOOP
void loop() {  
  if (systemPower == true){
    if (irrecv.decode(&results)) {  
    Serial.print(results.value, HEX);
    switch(results.value) {   
      case 0xFFA25D:  // Button 1 on the IR remote
        tempUnit = 0; // turn on LED strip
      	Serial.print('test');
      	break;
      case 0xFF629D:  // Button 2 on the IR remote
        tempUnit = 1; // turn off LED strip
      	break;
      case 0xFD00FF:
      	delline(0);
      	delline(1);
      	digitalWrite(redLED,LOW);
    	digitalWrite(greenLED,LOW);
    	digitalWrite(blueLED,LOW);
      	digitalWrite(yellowLED, LOW);
      	systemPower = false;
      	break;
      	
    }
    irrecv.resume(); // receive the next value
    
  }
    
  float C = AnalogToTemp(analogRead(sensorTMP36));
  
  Temperature(C);
  
  ShowLED(C);
  
  float Celsius    = C;
  
  // Temperature conversions from Celsius         Temperature conversions to Celsius
  float Fahrenheit = C * 9 / 5 + 32;                 // C = (F - 32) * 5 / 9
  
  switch (tempUnit) {
    case 0:
      lcd.setCursor(4, 1);  lcd.print("Celsius");
      delay(1000); delline(1);
      lcd.print(Celsius);
      lcd.write((byte)0); lcd.print("C  ");
      break;
    case 1:
      lcd.setCursor(3, 1); lcd.print("Fahrenheit");
      delay(1000); delline(1);
      lcd.print(Fahrenheit);
      lcd.write((byte)0); lcd.print("F  ");
      break;
    
  }
  lcd.setCursor(9, 1); 
  lcd.print(msg);
  delay(3000); 
  delline(1);
  }
  else {
    if (irrecv.decode(&results)) {    
    switch(results.value) {   
      case 0xFD00FF:
      	systemPower = true;
      	break;
    }
    irrecv.resume(); // receive the next value
  }
  } 
}
