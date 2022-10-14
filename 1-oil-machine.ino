#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

const int Button250ml = 9; // button for 250ml
const int Button500ml = 4; // button for 500ml
const int Button1000ml = 6; // button for 1000ml
const int StartButton = 7; // button for start


const int MotorRelay = 11; // relay for motor on off
const int Valve = 10; // For solenoid Valve
const int Buzzer = 12; //for sound alert
boolean Start_pressed = false;
boolean b1pressed = false;
boolean b2pressed = false;
boolean b3pressed = false;
boolean b4pressed = false;
boolean WP_ON = false;

byte sensorInterrupt = 0;  // 0 = digital pin 2 for water flow sensor
byte sensorPin       = 2;

float calibrationFactor = 300.0; //Calibration factor

volatile byte pulseCount;

float flowRate;
unsigned int flowMilliLitres = 0;
unsigned long totalMilliLitres = 0;
unsigned long TotalPrint = 0;
unsigned long Threshold = 295;

unsigned long oldTime;

void setup()
{ lcd.init();
  lcd.backlight();
  
    lcd.setCursor(0, 0);
    lcd.print(" Automatic Oil ");
    lcd.setCursor(0, 1);
    lcd.print("Filling Machine ");
    delay(3000);
    lcd.clear();
  
    lcd.setCursor(2, 0);
    lcd.print("Designed by");
    lcd.setCursor(1, 1);
    lcd.print("City Hardware");
    delay(2000);
    lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Set Qty: ");
  lcd.print("250");
  lcd.print(" ML");
  pinMode(Button250ml, INPUT_PULLUP);
  pinMode(Button500ml, INPUT_PULLUP);
  pinMode(Button1000ml, INPUT_PULLUP);

  pinMode(StartButton, INPUT_PULLUP);


  pinMode(MotorRelay, OUTPUT);
  pinMode(Valve, OUTPUT);
  digitalWrite(MotorRelay, HIGH);
  digitalWrite(Valve, HIGH);
  pinMode(Buzzer, OUTPUT);

  // Serial.begin(9600);

  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}

void loop()
{ if (digitalRead(Button250ml) == LOW && b1pressed == false && WP_ON == false)
  { b1pressed = true;
    lcd.clear();
    Threshold = 50; //295 to 50
    lcd.setCursor(0, 0);
    lcd.print("Set Qty: ");
    lcd.print("250");
    lcd.print(" ML");
    digitalWrite(Buzzer, HIGH);
    delay(80);
    digitalWrite(Buzzer, LOW);
    totalMilliLitres = 0;

  }
  if (digitalRead(Button250ml) == HIGH)
  {
    b1pressed = false;
  }
  if (digitalRead(Button500ml) == LOW && b2pressed == false && WP_ON == false)
  { b2pressed = true;
    lcd.clear();
    //Threshold = 585;
    Threshold = 460;
    lcd.setCursor(0, 0);
    lcd.print("Set Qty: ");
    lcd.print("500");
    lcd.print(" ML");
    digitalWrite(Buzzer, HIGH);
    delay(80);
    digitalWrite(Buzzer, LOW);
    totalMilliLitres = 0;
  }
  if (digitalRead(Button500ml) ==+ HIGH)
  {
    b2pressed = false;
  }
  if (digitalRead(Button1000ml) == LOW && b3pressed == false && WP_ON == false)
  { b3pressed = true;
    lcd.clear();
    //Threshold = 1195;
    Threshold = 820;
    lcd.setCursor(0, 0);
    lcd.print("Set Qty: ");
    lcd.print("1000");
    lcd.print(" ML");
    digitalWrite(Buzzer, HIGH);
    delay(80);
    digitalWrite(Buzzer, LOW);
    totalMilliLitres = 0;
  }
  if (digitalRead(Button1000ml) == HIGH)
  {
    b3pressed = false;
  }


  detachInterrupt(sensorInterrupt);


  flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;


  oldTime = millis();

  flowMilliLitres = (flowRate / 60) * 1000;

  totalMilliLitres += flowMilliLitres;

  unsigned int frac;

  TotalPrint = totalMilliLitres;
  if(Threshold == 295)
  {
    TotalPrint = map(TotalPrint, 0, 50, 0, 250);//295 to 50
    TotalPrint = constrain(TotalPrint, 0, TotalPrint);
    TotalPrint = constrain(TotalPrint, 0, 250);
  }
  if(Threshold == 460)
  {
    TotalPrint = map(TotalPrint, 0, 460, 0, 500);
    TotalPrint = constrain(TotalPrint, 0, TotalPrint);
    TotalPrint = constrain(TotalPrint, 0, 500);
  }
  if(Threshold == 820)
  {
    TotalPrint = map(TotalPrint, 0, 820, 0, 1000);
    TotalPrint = constrain(TotalPrint, 0, TotalPrint);
    TotalPrint = constrain(TotalPrint, 0, 1000);
  }
  
  lcd.setCursor(0, 1);
  lcd.print("Out Qty: ");
  lcd.print(TotalPrint);
  lcd.print(" ML");


  if (digitalRead(StartButton) == LOW && Start_pressed == false && WP_ON == false)
  { lcd.clear();
    Start_pressed = true;
    digitalWrite(MotorRelay, LOW);
    digitalWrite(Valve, LOW);
    WP_ON = true;
    lcd.setCursor(0, 0);
    lcd.print("Set Qty: ");
    if(Threshold==50)//295 to 50
    {lcd.print("250");}
    if(Threshold==460)
    {lcd.print("500");}
    if(Threshold==820)
    {lcd.print("1000");}
    
    lcd.print(" ML");
    totalMilliLitres = 0;
    TotalPrint = 0;
  }
  if (digitalRead(StartButton) == HIGH )
  {
    Start_pressed = false;
  }

  if (totalMilliLitres > Threshold)
  {
    digitalWrite(MotorRelay, HIGH);
    digitalWrite(Valve, HIGH);
    WP_ON = false;

    digitalWrite(Buzzer, HIGH);
    delay(600);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Filling");
    lcd.setCursor(5, 1);
    lcd.print("Complete.... ");
    digitalWrite(Buzzer, LOW);
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set Qty: ");
    if(Threshold==50)//295 to 50
    {lcd.print("250");}
    if(Threshold==460)
    {lcd.print("500");}
    if(Threshold==820)
    {lcd.print("1000");}
    lcd.print(" ML");
    totalMilliLitres = 0;
  }


  pulseCount = 0;

  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}


void pulseCounter()
{
  pulseCount++;
}
