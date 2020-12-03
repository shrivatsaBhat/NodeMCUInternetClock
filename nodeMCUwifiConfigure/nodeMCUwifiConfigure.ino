#include <Wire.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h> // Include the WiFi library
#include <ESP8266WiFiMulti.h> // Include the WiFi-Multi library
#include <ESP8266WebServer.h>

// System Variables
int statusCode;
const char* ssid = "text";
const char* passphrase = "text";
String st;
String content;


// System Function Decalration
void setupAP(void);
bool testWifi(void);
void launchWeb(void);
void autoConnectWifi(void);
void createWebServer(void);

//WiFiClient  client;
ESP8266WebServer server(80); //Establishing Local server at port 80 whenever required

void setup() {
  Serial.begin(115200);  // Initialize serial communication & Initialising if(DEBUG)Serial Monitor
  Wire.begin();

  Serial.println();
  Serial.println("Waiting.");
  
  while ((WiFi.status() != WL_CONNECTED)) {
    Serial.print(".");
    delay(100);
    server.handleClient();
  }
}

void autoConnectWifi() {
  Serial.println();
  Serial.println("Disconnecting previously connected WiFi");
  WiFi.disconnect();
  EEPROM.begin(512); //Initialasing EEPROM
  delay(10);
  Serial.println();
  Serial.println("Startup");
  //  Read eeprom for ssid and pass
  Serial.println("Reading EEPROM ssid");
  String esid;
  for (int i = 0; i < 32; ++i) {
    esid += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");
  String epass = "";
  for (int i = 32; i < 96; ++i) {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(epass);
  WiFi.begin(esid.c_str(), epass.c_str());

  if (testWifi()) {
    Serial.println("Succesfully Connected!!!");
  } else {
    Serial.println("Turning the HotSpot On");
    launchWeb();
    setupAP();// Setup HotSpot
  }
}

void ConnectToWiFi(void){
  if ((WiFi.status() != WL_CONNECTED))
  {
    setupAP();
    launchWeb();
  }
}


// Fuctions used for WiFi credentials saving and connecting to it which you do not need to change 
bool testWifi(void) {
  int checkCode = 0;
  Serial.println("Waiting for Wifi to connect");
  while ( checkCode < 20 ) {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(500);
    Serial.print("*");
    checkCode++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
}

void launchWeb() {
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");
  }
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  // Start the server
  server.begin();
  Serial.println("Server started");
}

void setupAP(void) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("--no networks found--");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  st = "<ol>";
  for (int i = 0; i < n; ++i) {
    // Print SSID and RSSI for each network found
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);
    st += ")";
    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
    st += "</li>";
  }
  st += "</ol>";
  delay(100);
  
  WiFi.softAP("ESP8266_HOTSPOT", "");
  Serial.println("Software Application");
  launchWeb();
  Serial.println("Finish");
}

void createWebServer() {
    server.on("/", []() {

      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE HTML>\r\n<html>";
      content += "<head><style>body{display: inline-grid;background:linear-gradient(to right,#fff,#eee);color:#111;font-size:20px;}input{font-size:30px;font-weight: bold;color:#000;padding:10px;border-radius:5px;outline:none;}input[type=submit]{color:#fff;background:#4285F4;display:inline-block;border:none;}input[type=submit]:hover{background:#0060FF;letter-spacing:1px;cursor:pointer;}</style></head>";
      content += "<body><h1>Admin page of ESP8266 at ";
      content += ipStr;
      content += "<br><br><form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"SCAN\"></form>";
      content += "<p>List of Available WiFi";
      content += st;
      content += "</p><form method='get' action='setting'>SSID: <input name='ssid' length=32><br><br>PASS: <input name='pass' length=64 type=\"password\"><br><br><input type='submit' value=\"SUBMIT\"></form>";
      content += "</h1></body></html>";
      server.send(200, "text/html", content);
    });
    server.on("/scan", []() {
      //setupAP();
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);

      content = "<!DOCTYPE HTML>\r\n<html style=\"font-size: 100px;\">Go Back";
      server.send(200, "text/html", content);
    });

    server.on("/setting", []() {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      if (qsid.length() > 0 && qpass.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 96; ++i) {
          EEPROM.write(i, 0);
        }
        Serial.println(qsid);
        Serial.println("");
        Serial.println(qpass);
        Serial.println("");

        Serial.println("Writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
          Serial.print("Wrote: ");
          Serial.println(qsid[i]);
        }
        Serial.println("Writing eeprom pass:");
        for (int i = 0; i < qpass.length(); ++i)
        {
          EEPROM.write(32 + i, qpass[i]);
          Serial.print("Wrote: ");
          Serial.println(qpass[i]);
        }
        EEPROM.commit();

        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
        ESP.reset();
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", content);

    });
  } 


void loop() {
  delay(1000);
}
