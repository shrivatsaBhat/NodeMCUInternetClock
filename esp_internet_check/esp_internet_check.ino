/*
 *  The ESP8266 Internet Connection Status
 *  based off the /ESP8266WiFi/WiFiClient example
 * 
 */
 
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

// Your WiFi network credentials
const char* ssid = "ssid";
const char* password = "password";

// The URL we will use to test our connection
const char* host = "www.google.co.in";
const byte port = 80;

ESP8266WiFiMulti wifiMulti;

// Function declarations
bool ConnectWiFi(int);
bool ConnectInternet(void);
bool ConnectNetwork(void);

void setup(){
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    ConnectWiFi(1000);
    Serial.println("SetUp Done!");

}

void loop() {
    bool status = ConnectNetwork();
    Serial.print("Is Netowrk Connected - ");
    Serial.println(status);
    delay(3000);
}

// @param int retryDelay: Time in milliseconds to wait between status checking (default 500ms)
bool ConnectWiFi(int retryDelay = 500) {
    // Add all available accesspoint names
    wifiMulti.addAP(ssid,password);

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

    while(!isWiFi) {
        isWiFi = ConnectWiFi(1000);
    }
    if(isWiFi){
        hasInternet = ConnectInternet();
        while(!hasInternet) {
            hasInternet = ConnectInternet();
        }
        return hasInternet;
    }
    return hasInternet;
}