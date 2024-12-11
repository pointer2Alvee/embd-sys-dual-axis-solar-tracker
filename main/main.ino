//_____________LIBRARIES___________//
#include<dht.h>
#include <Servo.h>
#include<LiquidCrystal.h>

//_____________VARIABLES___________//



//ALARM VARIABLES
//LEDs
byte pin_RedLED = A5;
byte pin_GreenLED = 2;
//Buzzer
byte pin_Buzzer = 3;

//WATER SENSOR
byte pin_WaterSensor = 4;

//LDR-SENSOR VARIABLES
int ldrtopr = A0; //top right LDR A1 pin
int ldrtopl = A1; //top left LDR A2 pin

int ldrbotr = A2; // bottom right LDR A0 pin
int ldrbotl = A3; // bottom left LDR A3 pin



//DHT11 TEMP-HUMIDITY SENSOR VARIABLES
byte pin_DHT11Sensor = 13;

//SERVO PINS
int servo1Pin = 5; // bottom servo
int servo2Pin = 6; // top servo

int servo1Pos = 0;
int servo2Pos  = 0;
int pos = 0;

int servoHighestAngle = 150;
int servoLowestAngle = 20;
// LCD PINS
/* Connect the LCD to pins as shown */
int RS = 7 ;
int EN = 8;
int D4 = 9;
int D5 = 10;
int D6 = 11;
int D7 = 12;

//_____________OBJECTS___________//
dht objectDHT;
Servo thisServo1;
Servo thisServo2;

//Creating object type - LiquidCrystal
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

//_____________FUNCTIONS___________//

// Alarm System Function
void func_AlarmSystem(boolean status_ALARM) {
  if (status_ALARM) {
    digitalWrite(pin_RedLED, HIGH);
    digitalWrite(pin_Buzzer, HIGH);
    digitalWrite(pin_GreenLED, LOW);
    delay(200);
    digitalWrite(pin_RedLED, LOW);
    digitalWrite(pin_Buzzer, LOW);
    digitalWrite(pin_GreenLED, LOW);
    delay(200);
  } else if (!status_ALARM) {
    digitalWrite(pin_GreenLED, HIGH);
    digitalWrite(pin_RedLED, LOW);
    digitalWrite(pin_Buzzer, LOW);

  }

}

// Servo motor rotation
void func_Rotation(int servoID, int type) {
  if (servoID == 1 && type == 1) {
    thisServo1.write(servo1Pos - 1);
    func_WaterDetection();
    if (servo1Pos > servoHighestAngle)
    {
      servo1Pos = servoHighestAngle;
    }
  } else if (servoID == 1 && type == 2) {

    thisServo1.write(servo1Pos + 1);
    func_WaterDetection();
    if (servo1Pos < servoLowestAngle)
    {
      servo1Pos = servoLowestAngle;
    }
  } else if (servoID == 1 && type == 3) {
    thisServo1.write(servo1Pos); // write means run servo
    func_WaterDetection();
  }

  if (servoID == 2 && type == 1) {
    thisServo2.write(servo2Pos - 1);
    func_WaterDetection();
    if (servo2Pos > servoHighestAngle)
    {
      servo2Pos = servoHighestAngle;
    }
  } else if (servoID == 2 && type == 2) {
    thisServo2.write(servo2Pos + 1);
    func_WaterDetection();
    if (servo2Pos < servoLowestAngle)
    {
      servo2Pos = servoLowestAngle;
    }
  } else if (servoID == 2 && type == 3) {
    thisServo2.write(servo2Pos);
    func_WaterDetection();
  }

}
//LDR Detection Function
void func_LightIntensityDetection() {

  servo1Pos = thisServo1.read();
  servo2Pos = thisServo2.read();

  int topl = analogRead(ldrtopl); //read analog values from top left LDR
  int topr = analogRead(ldrtopr); //read analog values from top right LDR
  int botl = analogRead(ldrbotl); //read analog values from bottom left LDR
  int botr = analogRead(ldrbotr); //read analog values from bottom right LDR


  int avgtop = (topl + topr) / 2; //average of top LDRs
  int avgbot = (botl + botr) / 2; //average of bottom LDRs
  int avgleft = (topl + botl) / 2; //average of left LDRs
  int avgright = (topr + botr) / 2; //average of right LDRs


  if (avgtop < avgbot)
  {
    func_Rotation(2, 1);
    delay(8);
  }
  else if (avgbot < avgtop)
  {
    func_Rotation(2, 2);
    delay(8);
  }
  else
  {
    func_Rotation(2, 3);
  }

  if (avgleft < avgright)
  {
    func_Rotation(1, 1);
    delay(8);
  }
  else if (avgright < avgleft)
  {
    func_Rotation(1, 2);
    delay(8);
  }
  else
  {
    func_Rotation(1, 3);
  }
  delay(30);
}

//DHT11 Temp-Humidity Function
void func_TempHumidDetection() {


  objectDHT.read11(pin_DHT11Sensor);
  //prints results
  Serial.print("TEMPERATURE: ");
  Serial.print(objectDHT.temperature);
  Serial.print("C: ");
  Serial.print("     ");
  Serial.print("HUMIDITY: ");
  Serial.print(objectDHT.humidity);
  Serial.print("%");

  //Display at LCD
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("TEMP: ");
  lcd.print(objectDHT.temperature);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("HUMID: ");
  lcd.print(objectDHT.humidity);
  lcd.print(" %");
  // delay(500);
  // lcd.clear();

}

void func_WaterDetection() {

  /* The water sensor will switch LOW when water is detected.
    Get the Arduino to illuminate the LED and activate the buzzer
    when water is detected, and switch both off when no water is present */
  if ( digitalRead(pin_WaterSensor) == HIGH) {
    func_AlarmSystem(true);
  } else {
    func_AlarmSystem(false);
  }
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Leds Pin
  pinMode(pin_RedLED, OUTPUT);
  pinMode(pin_GreenLED, OUTPUT);
  pinMode(pin_Buzzer, OUTPUT);

  pinMode(ldrtopr, INPUT);
  pinMode(ldrtopl, INPUT);
  pinMode(ldrbotr, INPUT);
  pinMode(ldrbotl, INPUT);

  //WaterSensor
  pinMode(pin_WaterSensor, INPUT);

  //servo
  thisServo1.attach(servo1Pin); // Defines on which pin is the servo motor attached
  thisServo2.attach(servo2Pin); // Defines on which pin is the servo motor attached


  //LCD Display
  lcd.begin(16, 2);   // set-up LCD display
}


void rotation() {
  //for demo

  for (pos = 20; pos <= 150; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree

    thisServo1.write(pos);
    thisServo2.write(pos);  // tell servo to go to position in variable 'pos'
    func_WaterDetection();
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  for (pos = 150; pos >= 20; pos -= 1) { // goes from 180 degrees to 0 degrees

    thisServo1.write(pos);
    thisServo2.write(pos);              // tell servo to go to position in variable 'pos'
    func_WaterDetection();
    delay(15);                       // waits 15 ms for the servo to reach the position


  }


}
void loop() {
  // put your main code here, to run repeatedly:

  func_LightIntensityDetection();
  func_TempHumidDetection();
  //rotation(); //testing
  // func_AlarmSystem(true);//testing


}
