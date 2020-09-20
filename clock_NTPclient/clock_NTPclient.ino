/***************************NodeMCU Internet Clock*********************************/

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

const char *ssid     = "ssid";
const char *password = "password";

const long utcOffsetInSeconds = 19800;     //UTC offset in seconds

char days[7][12]  = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
char month[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);            //create object of NTP client

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);            //create object of ssd1306 -1 no hardware rest pin

void setup()
{
  Serial.begin(115200);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);

  /*intialize OLED display*/
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // display.setCursor(10,28);
  display.setCursor(3,28);
  display.setTextSize(2);
  display.print("PARDAKANDA");
  display.display();
  delay(3500);
  display.clearDisplay();

  
  display.setCursor(3,28);
  display.setTextSize(2);
  display.setTextColor(WHITE);

  /*Connect to the WiFi*/
  WiFi.begin(ssid, password);

  display.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(800);
    //Serial.print (".");
    display.print(".");
    display.display();
  }
  display.setCursor(0,0);
  display.setTextSize(1);
  display.clearDisplay();
  timeClient.begin();
}

void loop() 
{
  timeClient.update();          //upadte time
  displayTimeDate();             //display time on OLED
  delay(100);
}

void displayTimeDate()
{
  String meridiem;
  int hour;
  
  /*Calculate time and date*/
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime);

  int date = ptm->tm_mday;
  int monthNum = ptm->tm_mon+1;
  int year = ptm->tm_year+1900;

  String day = days[timeClient.getDay()];
  int minute = timeClient.getMinutes();
  int second = timeClient.getSeconds();

  /*Convert 24hr format to 12hr format*/
  if(timeClient.getHours() >= 12)
  {
    meridiem = "PM"; 
    hour = timeClient.getHours() - 12;
  }
  else
  {
    meridiem = "AM";
    hour = timeClient.getHours();
  }

  if(hour == 0)
  {
    hour = 12;
  }
  
  /*Display time on OLED display*/
  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(5,10);
  display.print(day);
  
  display.setTextSize(3);
  display.setCursor(58,6);
  if(date < 10) display.print(0);
  display.print(date);

  display.setTextSize(1);
  display.setCursor(97,8);
  display.print(month[monthNum-1]);
  display.setCursor(97,20);
  display.print(year);
  
 
  display.setCursor(5,39);
  display.setTextSize(3);
  if(hour < 10) display.print(0);
  display.print(hour);
  display.print(":");
  if(minute < 10) display.print(0);
  display.print(minute);
  display.setTextSize(2);
  display.setCursor(100,37);
  if(second < 10) display.print(0);
  display.print(second);
  display.setTextSize(1);
  display.setCursor(100,55);
  display.print(meridiem);
  display.display();
}
