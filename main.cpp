#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>
#include <WiFi.h>
#include "time.h"
const char* ssid     = "BELL358";
const char* password = "71A36D5C24AD";
hw_timer_t * timer = NULL;
struct tm timeinfo;
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define FONT_SIZE 2
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
volatile int year = 0;
volatile int month = 0;
volatile int day = 0;

volatile int second = 0;
volatile int minute = 0;
volatile int hour = 0;
void IRAM_ATTR onTimer() {
  if(second == 59){
    second = 0;
    if(minute == 59){
      minute = 0;
      if(hour==23){
        hour = 0;
        timeinfo.tm_mday++;
        mktime(&timeinfo);
        year = timeinfo.tm_year + 1900;
        month = timeinfo.tm_mon + 1;
        day = timeinfo.tm_mday;
      }else{
        hour++;
      }
    }else{
      minute++;
    }

  }else{
    second++;
  }
  
  // Toggle an LED or perform a task
}
void time_setup(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  // Initialize a NTP Client
  configTime(-5*3600, 3600, "pool.ntp.org","time.nist.gov","time.windows.com"); // GMT offset, daylight offset, NTP server address
  Serial.println("Connected to WiFi");

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  hour = timeinfo.tm_hour;
  minute = timeinfo.tm_min;
  second = timeinfo.tm_sec + 1;
  year = timeinfo.tm_year + 1900;
  month = timeinfo.tm_mon + 1;
  day = timeinfo.tm_mday;
}
void setup() {
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(FONT_SIZE);
  display.setTextColor(WHITE);
  // Display static text
  
  timer = timerBegin(0, 80, true); // Timer 0, prescaler is 80, count up

  // Attach the ISR to the timer
  timerAttachInterrupt(timer, &onTimer, true); // Attach the interrupt service routine

  // Set the timer to trigger the interrupt every 1 second (1e6 microseconds)
  timerAlarmWrite(timer, 1000000, true); // Set the timer for 1 second

  // Enable the timer
  timerAlarmEnable(timer); 
  time_setup();

}
void display_date(int x, int y){
  display.setCursor(0+x,0+y);
  display.print(String(year));
  display.setCursor(22*FONT_SIZE+x,0+y);
  display.print(".");
  display.setCursor(27*FONT_SIZE+x,0+y);
  if(month < 10){
    display.print("0");
  }
  display.print((String)month);
  display.setCursor(37*FONT_SIZE+x,0+y);
  display.print(".");
  display.setCursor(42*FONT_SIZE+x,0+y);
  if(day < 10){
    display.print("0");
  }
  display.print((String)day);
}
void display_time(int x,int y){
  display.setCursor(0+x,0+y);
  if(hour<10){
    display.print("0");
  }
  display.print((String)hour);
  display.setCursor(13*FONT_SIZE+x,0+y);
  display.print(":");
  display.setCursor(20*FONT_SIZE+x,0+y);
  if(minute<10){
    display.print("0");
  }
  display.print((String)minute);
  display.setCursor(33*FONT_SIZE+x,0+y);
  display.print(":");
  display.setCursor(40*FONT_SIZE+x,0+y);
  if(second<10){
    display.print("0");
  }
  display.print((String)second);
}
void loop(){
  display.clearDisplay();
  display_date(9,20);
  display_time(11,40);
  display.display();
}