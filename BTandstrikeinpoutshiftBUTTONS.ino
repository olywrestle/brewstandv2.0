/* BRAD BREWSTAND TESTING CONFIG
Just testing github
5 button screen change
Dual ds18b20 temp different screens
Countdown timer

Working:
Bluetooth 
DS18B20
Readout to Serial Monitor (Bluetooth)
RTC prints Date and time to serial
LED turn on shift register
LCD with SR

**** DS18B20 Pinout (Left to Right, pins down, flat side toward you)
 - Left   = Ground
 - Center = Signal (with 3.3K to 4.7K resistor to +5 or 3.3 )
 - Right  = +5 or +3.3 V
PENDING CHANGE **** LCD Pinout
 Pin 1 to Arduino GND
 Pin 2 to Arduino 5V
 Pin 3 to wiper ( this is the middle pin of the 10k potentiometer )
 Pin 4 (RS) to Arduino pin 7
 Pin 5 (RW) to Arduino GND
 Pin 6  (EN) to Arduino pin 8
 Pin 11 (DB4) to Arduino pin 9
 Pin 12 (DB5)to Arduino  pin 10
 Pin 13 (DB6) to Arduino pin 11
 Pin 14 (DB7) to Arduino pin 12
 Pin 15 to 5V (with resistor?)
 Pin 16 to GND
**** Arduino NANO Pinout
 D0 to HC-05 TXD
 D1 to HC-05 RXD
 D2 to DS18B20 Signal
 D3 to _
 D4 to LED Shift Reg Data
 D5 to LED Shift Reg Latch
 D6 to LED Shift Reg Clock
 D7 to 
 D8 to 
 D9 to 
 D10 to LCD SR LATCH
 D11 to LCD SR DATA
 D12 to 
 D13 to LCD SR CLOCK
 A0 to HC-05 SIGNAL (BT connection indicator)
 A1 to 5 button board input
 A2 to 
 A3 to  
 A4 to RTC SDA
 A5 to RTC SCL 


*****************************
*****LED Shift Register******
*bit set                    *
*      6 |--|_|--| Vcc      *
*      5 |   7   | 7  OPEN  *
*      4 |   4   | DataPin  *
*      3 |   H   |          *
*      2 |   C   | LatchPin *
*      1 |   5   | ClockPin *
*      0 |   9   |          *
*    Gnd |___5___|          *
*****************************
*LEDS to Shift register
* Strike LED
* blueledstrike = bit 0
* greenledstrike = bit 1
* redled1strike = bit 2
*MASH
* blueledmash = bit 3
* greenledmash = bit 4
* redled1mash = bit 5
*BOIL
* redledboil = bit 6
*****************************
*****************************

*****************************
*****LCD Shift Register******
*                            *
* LCD RS |--|_|--| Vcc 5v+   *
*        |   7   |           *
* LCD En |   4   | Data D11  *
* LCD D4 |   H   | Gnd       *
* LCD D5 |   C   | Latch D10 *
* LCD D6 |   5   | Clock D13 *
* LCD D7 |   9   | 5V+       *
*    Gnd |___5___|           *
*****************************
*****************************
LCD PINS
Left to right screen facing you
1 - Ground
2 - 5V+
3 - POT Wiper
4 - SR Q1
5 - Ground
6 - SR Q3
7 -
8 -
9 -
10 -
11 - SR Q4
12 - SR Q5
13 - SR Q6
14 - SR Q7
15 - 5v+ with resistor
15 - Ground
*****************************
*****************************
**SR 74HC595 PINS************
*****LED Shift Register*************
*      Q1 |--|_|--| Vcc +          *
*      Q2 |   7   | Q0             *
*      Q3 |   4   | Ds DataPin     *
*      Q4 |   H   | OE             *
*      Q5 |   C   | ST/CP LatchPin *
*      Q6 |   5   | SH/CP ClockPin *
*      Q7 |   9   | MR             *
*    Gnd  |___5___| Q7             *
************************************
*/

#include <SoftwareSerial.h>//used for BLUETOOTH module
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <LiquidCrystal.h>//up[dated for SPI aka Shift Reg
#include <Wire.h> //RTC MODULE

#define ONE_WIRE_BUS 2 //DS18B20
#define DS3231_I2C_ADDRESS 0x68 //RTC MODULE

float TW;
float TD; //Dough In
float TG; // Grain temp
float M; //Water to grain ratio
float GW; //Grain Weight

float TGMAX = 90.0;
float TGMIN = 32.0;
float TDMIN = 140.0;
float TDMAX = 158.9;
float MMIN = 1.0;
float MMAX = 2.0;
float GWMIN = 1.0;
float GWMAX = 100.0;

//BLUETOOTH SETUP
SoftwareSerial BTinput(0, 1); // RX, TX
int BluetoothData; // the data given from Computer
//http://www.instructables.com/id/Arduino-AND-Bluetooth-HC-05-Connecting-easily/step3/Arduino-Code/
// BTconnected will = false when not connected and true when connected
boolean BTconnected = false;
// connect the STATE pin to Arduino pin A0
const byte BTpin = A0;
//http://www.martyncurrey.com/hc-05-fs-040-state-pin/
//end BT SETUP

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass address of our oneWire instance to Dallas Temperature.
DallasTemperature sensors(&oneWire);
/*-----( Declare Variables )-----*/
// Assign the addresses of your 1-Wire temp sensors.
// See the tutorial on how to obtain these addresses:
// http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html

DeviceAddress Probe01 = {
  0x28, 0xFF, 0x3C, 0x15, 0x68, 0x14, 0x04, 0x62
};
DeviceAddress Probe02 = {
  0x28, 0xFF, 0xBB, 0xB2, 0x52, 0x14, 0x00, 0x02
};
//END DALLAS

// RTC SETUP
// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}
//END RTC

//SHIFTREGISTER SETUP
int latchPin = 5;
int clockPin = 6;
int dataPin = 4;
byte leds = 0;
//LEDS to Shift register
//Strike LED
const int blueledstrike = 0;
const int greenledstrike = 1;
const int redledstrike = 2;
int myCounter1 = 0;
//MASH
const int blueledmash = 3;
const int greenledmash = 4;
const int redled1mash = 5;
int myCounter2 = 0;
//BOIL
const int redledboil = 6;
int myCounter3 = 0;
const int repeats = 10;
//END SHIFT
//SOUND aka TONE
const int buzzer1 = 9;
int buzzerCounter1 = 0;
int buzzerCounter2 = 0;

//LCD SHIFT REG
LiquidCrystal lcd(10);
byte plusChar[8] = {
  0b00100,
  0b01110,
  0b00100,
  0b00000,
  0b11111,
  0b00000,
  0b01110,
  0b00000
};

byte degChar[8] = {
  0b11100,
  0b10100,
  0b11100,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
//END LCD

//Analog 5 Button
int analogPin = 1;
int val = 0;

void setup() {

//clear shift register
  bitClear(leds, blueledstrike);
      bitClear(leds, greenledstrike);
      bitClear(leds, redledstrike);
      updateShiftRegister();
      //end clear shift
      pinMode(BTpin, INPUT);
      BTcontact;
  Wire.begin();
  BTinput.begin(9600);
  //SHIFT REGISTER
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  //END SHIFT 
    digitalWrite(buzzer1, LOW);
    
  establishContact(); //BLUETOOTH CONNECTION
delay(2000); //to allow serial read and write
 while (BTinput.available() == 0) {
  }
  BTinput.read();
  BTinput.println("Welcome to Brewstand_2.0");
  delay(1000);
  BTinput.println("Follow Prompts for Strike Calc");
  delay(250);
  BTinput.println("Enter Dough In Temp");
 while (BTinput.available() == 0) {
  }
  TD = BTinput.parseFloat();
  BTinput.print("You entered: ");
  BTinput.println(TD);
 while (TD < TDMIN || TD > TDMAX){ //test within parameters
   BTinput.println("Enter valid temp 140-158");
    while (BTinput.available() == 0) {
     }
      TD = BTinput.parseFloat();
      BTinput.print("You entered: ");
      BTinput.println(TD);
 }
  
delay(1000);
//GRAIN INPUTS and VALIDATION
 BTinput.println("Enter Grain Temp");
 while (BTinput.available() == 0) {
  }
  TG = BTinput.parseFloat();
  BTinput.print("You entered: ");
  BTinput.println(TG);
  while (TG < TGMIN || TG > TGMAX){ //test within parameters
   BTinput.println("Enter valid temp 32-90");
    while (BTinput.available() == 0) {
     }
      TG = BTinput.parseFloat();
      BTinput.print("You entered: ");
      BTinput.println(TG);
 } 
delay(1000);
//Water to Grain Ratio inputs and validations
  BTinput.println("Enter Water to Grain Ratio");
 while (BTinput.available() == 0) {
  }
  M = BTinput.parseFloat();
  BTinput.print("You entered: ");
  BTinput.println(M);
    while (M < MMIN || M > MMAX){ //test within parameters
   BTinput.println("Enter valid ratio 1.0 - 2.0");
    while (BTinput.available() == 0) {
     }
      M = BTinput.parseFloat();
      BTinput.print("You entered: ");
      BTinput.println(M);
 }
//Water to Grain Weight inputs and validations
  BTinput.println("Enter Grain Weight in lbs.");
 while (BTinput.available() == 0) {
  }
  GW = BTinput.parseFloat();
  BTinput.print("You entered: ");
  BTinput.println(GW);
    while (GW < GWMIN || GW > GWMAX){ //test within parameters
   BTinput.println("Enter valid weight 1.0 - 100.0");
    while (BTinput.available() == 0) {
     }
      M = BTinput.parseFloat();
      BTinput.print("You entered: ");
      BTinput.print(GW);
      BTinput.print("lbs");
 } 
  delay(1000);
  BTinput.println("Calculating Strike Volume and Temp");

// set the resolution to 10 bit (Can be 9 to 12 bits .. lower is faster)
sensors.begin();
  sensors.setResolution(Probe01, 10);
  sensors.setResolution(Probe02, 10);
  sensors.requestTemperatures();
  
  delay(750);
  StrikeCalc();
  delay(1500);
 // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
  delay(2000);
  lcd.clear();
}


void loop() {
  float HLTtemp = (sensors.getTempC(Probe01) * 1.8) + 32;
  float strikeTempmin = (TW - 1.00);
  float strikeTempmax = (TW + 1.00);
  float strikeTempunder = (TW - 6.00);
  // put your main code here, to run repeatedly:
//SHIFT
    leds = 0;

// Show Strike Page with Amt and temp on button press.

  val = analogRead(analogPin);
  if (val > 800){
  }
  else if (val > 0 && val < 75){
    StrikeCalc();
    lcd.setCursor(0, 0);
    displayTemperature(Probe01);
  }
// end reshow strike

/* Strike temp LED */
 
       if (HLTtemp == -196.60) // Measurement failed or no device found
  {
      bitClear(leds, blueledstrike);
      bitClear(leds, greenledstrike);
      bitClear(leds, redledstrike);
      updateShiftRegister();
      //lcd.setCursor(4, 0);
      BTinput.print("HLT: ");
      lcd.setCursor(0, 0);
      displayTemperature(Probe01);


      
  }
  else if (HLTtemp < strikeTempunder) {
      bitSet(leds, blueledstrike);
      bitClear(leds, greenledstrike);
      bitClear(leds, redledstrike);
      updateShiftRegister();
    //lcd.setCursor(4, 0);
      BTinput.print("HLT: ");
      lcd.setCursor(0, 0);
    displayTemperature(Probe01);


  }
  else if (HLTtemp >= strikeTempunder && HLTtemp < strikeTempmin) { //strikeTempunder is Strike - 6 degrees strikeTempmin is Strike -1 degree
    bitClear(leds, blueledstrike);
    //lcd.setCursor(4, 0);
      BTinput.print("HLT: ");
      lcd.setCursor(0, 0);
    displayTemperature(Probe01);

    if (myCounter1 < 3) {
        for (int i = 0; i < repeats; i++) {
          bitSet(leds, redledstrike);
          bitSet(leds, greenledstrike);
              updateShiftRegister();
          digitalWrite(buzzer1, HIGH);
          delay(300);
          digitalWrite(buzzer1, LOW);
      bitClear(leds, greenledstrike);
      bitClear(leds, redledstrike);
          updateShiftRegister();
          delay(300);
        }
        myCounter1 = myCounter1 + 1;
      }
          bitSet(leds, redledstrike);
          bitSet(leds, greenledstrike);
              updateShiftRegister();

  }
  else if (HLTtemp > strikeTempmin && HLTtemp < strikeTempmax) {
      bitClear(leds, blueledstrike);
      bitSet(leds, greenledstrike);
      bitClear(leds, redledstrike);
          updateShiftRegister();
      //lcd.setCursor(4, 0);
        BTinput.print("HLT: ");
        lcd.setCursor(0, 0);
      displayTemperature(Probe01);


    }
  else if (HLTtemp > strikeTempmax) {
     bitClear(leds, blueledstrike);
      bitClear(leds, greenledstrike);
      bitSet(leds, redledstrike);
          updateShiftRegister();
      //lcd.setCursor(4, 0);
        BTinput.print("HLT: ");
        lcd.setCursor(0, 0);
      displayTemperature(Probe01);

    }
  else {
    //lcd.setCursor(4, 0);
      BTinput.print("HLT: ");
      lcd.setCursor(0, 0);
    displayTemperature(Probe01);

    }
    
//STRIKE LED DONE  



  
  sensors.requestTemperatures();
  lcd.setCursor(8, 0);
  BTinput.print("MSH: ");
  displayTemperature(Probe02);
  BTinput.print(" at ");
  displayTime();
  BTinput.println(" ");
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis()/1000);
  delay(5000);
}


//***********************************************
/*-----( Declare User-written Functions )-----*/

//***Begin Strike Calculation
void StrikeCalc()
{
  // TW=[TD(2.08xM + 0.4) - 0.4xTG]/(2.08xM)
  float Step1 = (2.08 * M) + 0.4;
  //BTinput.println(Step1);
  float Step2 = Step1 * TD;
  //BTinput.println(Step2);
  float Step3 = 0.4 * TG;
  //BTinput.println(Step3);
  float Step4 = Step2 - Step3;
  //BTinput.println(Step4);
  float Step5 = 2.08 * M;
  //BTinput.println(Step5);
  float StrikeTemp = Step4 / Step5;
  float StrikeVolumne = GW * M;
  float StrikeVolGal = StrikeVolumne * .25;
  BTinput.println("************************");
  BTinput.print("Strike Temp: ");
  BTinput.print(StrikeTemp);
  BTinput.println("*F");
  BTinput.print("Strike Amt: ");
  BTinput.print(StrikeVolGal);
  BTinput.println(" gal");
  BTinput.println("************************");
  delay(100);
}
//End Strike Calculation***

//***Begin Temp reading and Dsiplay
void displayTemperature(DeviceAddress deviceAddress) {

  float tempC = sensors.getTempC(deviceAddress);

  if (tempC == -127.00) // Measurement failed or no device found
  {
    BTinput.println("Sensor Error");
    lcd.print("Err");
  }
  else
  {
    BTinput.print(DallasTemperature::toFahrenheit(tempC));
    BTinput.println("* ");
    lcd.print(DallasTemperature::toFahrenheit(tempC));
        //lcd.write((uint8_t)2);
    }
}
// End Temp Reading and Display***

//***BEGIN CONTACT for BT
void BTcontact(){
      while (!BTconnected)
    {
      if ( digitalRead(BTpin)==HIGH)  { BTconnected = true;};
    }
          BTinput.println("Bluetooth Connected");
          
}
void establishContact() {
  while (!BTinput.available()){     //Do Absolutely Nothing until something is received over the serial port
    
    BTinput.println("Ready?");
    delay(750);
    BTinput.println("Type only Y when you are.....");
    delay(3000);
  }
  }
//END CONTACT for BT***

//***RTC TIME READ AND PRINT FUNCTIONS
void readDS3231time(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}
void displayTime()
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
  // send it to the serial monitor
  BTinput.print(hour, DEC);
  // convert the byte variable to a decimal number when displayed
  BTinput.print(":");
  if (minute<10)
  {
    BTinput.print("0");
  }
  BTinput.print(minute, DEC);
  BTinput.print(":");
  if (second<10)
  {
    BTinput.print("0");
  }
  BTinput.print(second, DEC);
  BTinput.print(" ");
  BTinput.print(month, DEC);
  BTinput.print("/");
    BTinput.print(dayOfMonth, DEC);
  BTinput.print("/");
  BTinput.println(year, DEC);
/*  BTinput.print(" Day of week: ");
  switch(dayOfWeek){
  case 1:
    BTinput.println("Sunday");
    break;
  case 2:
    BTinput.println("Monday");
    break;
  case 3:
    BTinput.println("Tuesday");
    break;
  case 4:
    BTinput.println("Wednesday");
    break;
  case 5:
    BTinput.println("Thursday");
    break;
  case 6:
    BTinput.println("Friday");
    break;
  case 7:
    BTinput.println("Saturday");
    break;
  }
*/
}
//END TIME ***

//SHIFT REGISTER START LED
void updateShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, leds);
   digitalWrite(latchPin, HIGH);
}
//END SHIFT

