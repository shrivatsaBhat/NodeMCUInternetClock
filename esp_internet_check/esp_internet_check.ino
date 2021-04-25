#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

const char* ssid = "PARDAKANDA";
const char* password = "shri@9803";

const char* host = "www.google.co.in";
const byte port = 80;

ESP8266WiFiMulti wifiMulti;

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


bool ConnectWiFi(int retryDelay = 1000) {
    wifiMulti.addAP(ssid,password);

    if(WiFi.status() != WL_CONNECTED){
        Serial.print("\nAttempting to connect to WiFi\n");
        Serial.print("Connecting");
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

        return true;
    }

    return false;
}