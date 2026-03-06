#include <Arduino.h>
#include <UTFT.h>                         	//Libriary for TFT
#include <DS3231.h>                       	//Libriary for Real Time Clock
#include <EEPROM.h>

//----------- instances-----------------
UTFT    myGLCD(CTE32_R2, 38, 39, 40, 41);	// myGLCD object of class UTFT with paramenter for specific TFT screed model
DS3231  rtc(SDA, SCL);                  	// rtc object of class DS3231 - Real-time-clock HW modul connected to pins SDA and SCL

//------------ variables ----------------
extern uint8_t BigFont[];          // Declare which fonts we will be using for TFT
extern uint8_t SmallFont[];        
extern uint8_t SevenSegNumFont[];  
Time t;
int x, y, hour, minute, minuteBefore, sec, secBefore, date, month, year, month_last = -1; 
int centerX = 110;		// center of round clock
int centerY = 120;		// center of round clock
int r = 100;			// radius of round clock
char serial_buf[50]; // buffer for messsages to serial interface
float a, tempr;
const char* months[] = {
  "",
  "January",
  "February",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December"
};
char month_cstr[12]; // c-style string for name of month

// routines
//----------------------------------------------------------------------
int readSerial(String request) {
// the function for reading serial.
// wiil be used for initial setup time and date
// In case of hours do not entered during 5 sec returns -1
  Serial.println(request);
  String inputText = "";
  unsigned long startTime = millis();

  while (inputText == ""){
    if (Serial.available()) {
      inputText = Serial.readString();
    }
    if ((request == "Enter year") && (millis() - startTime > 5000)) return -1;
  }
  inputText.trim();                        // remove any \r \n whitespace at the end of the String
  Serial.println(inputText);
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
	
//--------------------------------------------------------------------
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
//------------------------------------------------------------------
void printInfo() {
  // Print time
  tempr = rtc.getTemp();
  x = 190;
  y = 5;
  myGLCD.setFont(BigFont);
  myGLCD.setColor(125, 255, 125);
  myGLCD.printNumI(hour, x, y, 2, '0');
  myGLCD.print(":", x+16*2, y);
  myGLCD.printNumI(minute, x+16*3, y, 2, '0');
  myGLCD.print(":", x+16*5, y);
  myGLCD.printNumI(sec, x+16*6, y, 2, '0');
  
  x = 250;
  y = 40;
  myGLCD.printNumI(tempr, x, y, 2);
  myGLCD.print("C", x+16*3-4, y);
  myGLCD.drawCircle(x+16*2+8, y+4, 3);
  myGLCD.drawCircle(x+16*2+8, y+4, 2);
  
  y = 168;
  myGLCD.printNumI(date, x + 16*2 , y, 2);
  
  if(month != month_last) {
	  myGLCD.print("         ", x + 16*4 - 16 * 9, y + 8*3);
	  month_last = month;
  }
  myGLCD.print(month_cstr, x + 16*4 - 16 * strlen(month_cstr), y + 8*3);
  myGLCD.printNumI(year, x , y + 8*6);

  // Print data to serial.
  // Because float in printf is disabled in Arduino AVR libc I print it separately
  sprintf(serial_buf, "%02d:%02d:%02d   ", hour, minute, sec);
  Serial.print(serial_buf);
  Serial.print(tempr, 2);
  Serial.println("°C");
}

//--------------------------------------------
void setup() {
	rtc.begin();							// RTC clock initialisation

	Serial.begin(9600);					// serial interface initialisation
	Serial.println("Serial.begin(9600)");

	myGLCD.InitLCD(LANDSCAPE);			// LCD initialisation
	myGLCD.clrScr();
	myGLCD.setBackColor(0, 0, 0);  

	Serial.println("-- Date and Time Initial Setup --");	// set parameters via serial interface
	year =readSerial("Enter year"); 	// if year did not entered during 5 secund readSerial returns "-1",  skip time setup and use time saved in RTC
    if (year >= 0) {
		month = readSerial("Enter month");
		date = readSerial("Enter day of month");
		hour = readSerial("Enter hour");
		minute = readSerial("Enter minutes");
		sec = readSerial("Enter secundes");
		
    sprintf(serial_buf, "! date set to: %02d-%02d-%d", date, month, year);
    Serial.println(serial_buf);
    rtc.setDate(date, month, year);	
		
    sprintf(serial_buf, "! time set to: %02d:%02d:%02d", hour, minute, sec);
    Serial.println(serial_buf);
		rtc.setTime(hour, minute, sec);
	}
	
	drawClockface();
	Serial.println("setup routine done");
	//tone(A0, 440, 100);
}
// ---------------------------------------
void loop() {
  t = rtc.getTime();
  sec = t.sec;
  minute = t.min;
  
  strncpy(month_cstr, months[month], sizeof(month_cstr)); // get month name based on month number
  month_cstr[sizeof(month_cstr)-1] = '\0'; // ensure null-termination

  if (minute != minuteBefore) {
    hour = t.hour;
    date = t.date;
    month = t.mon;
    year = t.year;
  
    strncpy(month_cstr, months[month], sizeof(month_cstr)); // get month name based on month number
    month_cstr[sizeof(month_cstr)-1] = '\0'; // ensure null-termination
    
	  drawHands();
	  minuteBefore = minute;
  }

  if (sec != secBefore) {  
    printInfo();
    secBefore = sec;
  }
}
