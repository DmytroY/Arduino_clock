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
int x, y, hour, minute, minuteBefore, sec, secBefore, alarm_hour, alarm_minute;
int centerX = 120;		// center of round clock
int centerY = 120;		// center of round clock
int r = 100;			// radius of round clock
float a, tempr;
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

void drawClockface(){
	for(a = 0; a < 3.14*2; a += 3.1416/6){
		x = cos(a)*r;
		y = sin(a)*r;
    myGLCD.setColor(125, 125, 125);
		myGLCD.fillCircle(centerX + x, centerY + y, 5);
	}
}
	

void drawHands() {
  int hourHandL = r*6/10;
  int hourHandW = 3;
  int minuteHandL = r*8/10;
  int minuteHandW = 1;

  myGLCD.setColor(0, 0, 0);
  myGLCD.fillCircle(centerX, centerY, r*9/10);
  
  // hour hend
  a = -(3.1416/6)*(hour + (minute*0.0167) - 3); //angle in radians
  x = cos(a)*hourHandL;
  y = sin(a)*hourHandL;
  myGLCD.setColor(125, 255, 125);
  for (int dx = -hourHandW; dx <= hourHandW; dx++) {
    for (int dy = -hourHandW; dy <= hourHandW; dy++) {
      myGLCD.drawLine(centerX+dx, centerY+dy, centerX+x+dx, centerY-y+dy);
    }
  }

  // minute hend
  a = -(3.1416/30)*(minute - 15); //angle in radians
  x = cos(a)*minuteHandL;
  y = sin(a)*minuteHandL;
  myGLCD.setColor(125, 255, 125);
  for (int dx = -minuteHandW; dx <= minuteHandW; dx++) {
    for (int dy = -minuteHandW; dy <= minuteHandW; dy++) {
      myGLCD.drawLine(centerX+dx, centerY+dy, centerX+x+dx, centerY-y+dy);
    }
  }
  
}

void printInfo() {
  // Print time
  x = 190;
  y = 5;
  myGLCD.setFont(BigFont);
  myGLCD.setColor(125, 255, 125);
  myGLCD.printNumI(hour, x, y, 2, '0');
  myGLCD.print(":", x+16*2, y);
  myGLCD.printNumI(minute, x+16*3, y, 2, '0');
  myGLCD.print(":", x+16*5, y);
  myGLCD.printNumI(sec, x+16*6, y, 2, '0');

  // Print temp
  tempr = rtc.getTemp();
  Serial.print("-- Temperature: ");
  Serial.println(tempr);
  x = 250;
  y = 40;
  myGLCD.printNumI(tempr, x, y, 2);
  myGLCD.print("*C", x+16*2, y);
  

}

// ------------------------------------
/* int readSerial2(String request) {
   Serial.println(request);
   String inputText = "";
   while (inputText == ""){
    if (Serial.available()) {
      inputText = Serial.readString();
    }
  }
  inputText.trim();            
  return inputText.toInt();
} */

//--------------------------------------------
void setup() {
  rtc.begin();							// RTC clock initialisation
  
  Serial.begin(9600);					// serial interface initialisation
  Serial.println("Serial.begin(9600)");
  
  myGLCD.InitLCD(LANDSCAPE);			// LCD initialisation
  myGLCD.clrScr();
  myGLCD.setBackColor(0, 0, 0);  
  
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
  
    rtc.setTime(hour, minute, sec);
  }
  drawClockface();
  Serial.println("setup routine done");
  //tone(A0, 440, 100);
}
// ---------------------------------------
void loop() {
  t = rtc.getTime();
  hour = t.hour;
  minute = t.min;
  sec = t.sec;

  if (minute != minuteBefore) {
	  drawHands();
	  minuteBefore = minute;
  }

  if (sec != secBefore) {
    printInfo();
    secBefore = sec;
  }
}
