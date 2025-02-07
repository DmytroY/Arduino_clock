#include <Arduino.h>
#include <UTFT.h>                         	//Libriary for TFT
#include <DS3231.h>                       	//Libriary for Real Time Clock

// instances 
UTFT    myGLCD(CTE32_R2, 38, 39, 40, 41);	// myGLCD object of class UTFT with paramenter for specific TFT screed model
DS3231  rtc(SDA, SCL);                  	// rtc object of class DS3231 - Real-time-clock HW modul connected to pins SDA and SCL

//variables
extern uint8_t BigFont[];          // Declare which fonts we will be using for TFT
extern uint8_t SmallFont[];        
extern uint8_t SevenSegNumFont[];  
Time t;
int x, y, hour, minute, sec, alarm_hour, alarm_minute;
String message = "";

// routines
int readSerial(String request) {
// the function for reading serial.
// wiil be used for initial setup hours, minutes and secundes
// In case of hours do not entered during 5 sec returns -1
  Serial.println(request);
  String inputText = "";
  unsigned long startTime = millis();

  while (inputText == ""){
    if (Serial.available()) {
      inputText = Serial.readString();
    }
    if ((request == "Enter hours") && (millis() - startTime > 5000)) return -1;
  }
  inputText.trim();                        // remove any \r \n whitespace at the end of the String
  return inputText.toInt();
}

// ------------------------------------
void setup() {
  rtc.begin();							// RTC clock initialisation
  
  Serial.begin(9600);					// serial interface initialisation
  Serial.println("Serial.begin(9600)");
  
  myGLCD.InitLCD(LANDSCAPE);			// LCD initialisation
  myGLCD.clrScr();
  myGLCD.setBackColor(0, 0, 0);  
  myGLCD.setColor(125, 255, 125);
  myGLCD.setFont(BigFont);
  
  Serial.println("-- Time Initial Setup --");	// set parameters via serial interface
  hour = readSerial("Enter hours");				// if hours do not entered during 5 secund readSerial returns "-1",  skip time setup and use time saved in RTC
    if (hour >= 0) {
    minute = readSerial("Enter minutes");
    sec = readSerial("Enter secundes");
//    alarm_hour = readSerial("Enter alarm_hour");
//    alarm_minute = readSerial("Enter alarm_minute");

	String message = "";
	message = message + "-- setting time " + hour + ":" + minute + ":" + sec;
	Serial.println(message);

/*     Serial.print("-- setting time ");
    Serial.print(hour);  
    Serial.print(":");  
    Serial.print(minute);  
    Serial.print(":");  
    Serial.println(sec); */
  
    rtc.setTime(hour, minute, sec);
  }
  Serial.println("setup routine done");
}
// ---------------------------------------
void loop() {
  t = rtc.getTime();
  hour = t.hour;
  minute = t.min;
  sec = t.sec;
  
  Serial.print(hour);  
  Serial.print(":");  
  Serial.print(minute);  
  Serial.print(":");  
  Serial.println(sec);  
 
  //myGLCD.clrScr();
  x = 100;
  y = 100;
  myGLCD.printNumI(hour, x, y, 2, '0');
  myGLCD.print(":", x+16*2, y);
  myGLCD.printNumI(minute, x+16*3, y, 2, '0');
  myGLCD.print(":", x+16*5, y);
  myGLCD.printNumI(sec, x+16*6, y, 2, '0');
  delay(1000);
}
