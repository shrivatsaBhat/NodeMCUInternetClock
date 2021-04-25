/***************************NodeMCU Internet Clock*********************************/

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFiMulti.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Your WiFi network credentials
const char *ssid1     = "ssid1";
const char *password1 = "password1";
const char *ssid2     = "ssid2";
const char *password2 = "password2";

// The URL we will use to test our connection
const char* host = "www.google.co.in";
const byte port = 80;

const long utcOffsetInSeconds = 19800;     //UTC offset in seconds

char days[7][12]  = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
char month[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds); //create object of NTP client

ESP8266WiFiMulti wifiMulti; // Create an instance of the ESP8266WiFiMulti class

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); //create object of ssd1306 -1 no hardware rest pin

// Function declarations
bool ConnectWiFi(int);
bool ConnectInternet(void);
bool ConnectNetwork(void);
void displayTimeDate(void);

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

  display.setCursor(3,28);
  display.setTextSize(2);
  display.print("PARDAKANDA");
  display.display();
  delay(3500);
  display.clearDisplay();

  /*Connect to the WiFi*/
  WiFi.mode(WIFI_STA); // set-up a WPA2-PSK network
}

bool interGlobalState = false;

void loop() 
{
  bool status = ConnectNetwork();

  if(status){
    timeClient.update();          //upadte time
    displayTimeDate();             //display time on OLED
  }
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

// @param int retryDelay: Time in milliseconds to wait between status checking (default 500ms)
bool ConnectWiFi(int retryDelay = 500) {
    // Add all available accesspoint names, ans passwords
    wifiMulti.addAP(ssid1,password1);
    wifiMulti.addAP(ssid2,password2);

    if(WiFi.status() != WL_CONNECTED){
        Serial.print("\nAttempting to connect to WiFi\n");
        Serial.print("Connecting");
        // Connect the strongest network
        while(wifiMulti.run() != WL_CONNECTED){
            Serial.print(".");
            delay(retryDelay);
        }
        Serial.print("\nConnected to " + String(WiFi.SSID()) + "\n");
        Serial.print("IP address:\t");
        Serial.println(WiFi.localIP());
        return true;
    }
    if(WiFi.status() == WL_CONNECTED) {
        return true;
    }
    return false;
}


bool ConnectInternet() {
    WiFiClient client;
    // Connect to the host and port 
    int isConnected = client.connect(host, port);

    if (!isConnected){
        Serial.println("Connection failed!!!");
        client.stop();
        delay(1000);
        return false;
    }
    if(isConnected) {
        Serial.println("Connection Built");
        client.stop();
        return true;
    }
    return false;
}


bool ConnectNetwork() {
    // Holder to store Bolean status
    bool hasInternet = false;
    bool isWiFi = ConnectWiFi(800);
    
    if(!interGlobalState){
      display.setCursor(3,28);
      // display.setTextSize(2);
      display.setTextColor(WHITE);

      display.print("Connecting");
      display.print("....");
      display.display(); 
    }

    while(!isWiFi) {
        isWiFi = ConnectWiFi(1000);
    }
    if(isWiFi){
        hasInternet = ConnectInternet();
        interGlobalState = hasInternet;

        while(!hasInternet) {
            hasInternet = ConnectInternet();
        }

        display.setCursor(0,0);
        display.setTextSize(1);
        display.clearDisplay();
        timeClient.begin();
        interGlobalState = hasInternet;
        return hasInternet;
    }
    interGlobalState = hasInternet;
    return hasInternet;
}