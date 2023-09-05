#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"
#include <Arduino.h>

//#include <SPI.h>
//#include <WiFi.h>

// To Do

 // bugs
/*

 */
 

// Site specfic variables

//const char* ssid = "A_Virtual_Information";
//const char* password = "BananaRock";

const char* ssid = "SmartStuff";
const char* password = "Password123456";
//IPAddress ip(192, 168, 1, 15);  
#define DATA_IPADDR IPAddress(192,168,1,15) 
#define DATA_IPMASK IPAddress(255,255,255,0)
#define DATA_GATEWY IPAddress(192,168,1,1)
#define DATA_DNS1   IPAddress(192,168,1,1)

boolean connectWifi();

//on/off callbacks
bool SpeakerOnRequest();
bool SpeakerOffRequest();

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;

Switch *SpeakerControl = NULL;

bool isSpeakerOn = false;
bool isSpeakerOff = false;
 
const int SpeakerControlPin = 0;  
//const int PushButtonPin = 3; // 3 is the RX pin

void setup()
{

  Serial.begin(115200);

  Serial.println("Booting SpeakerControl...");
/*
  delay(2000);
  Serial.println("flashing LED on GPIO2...");
  //flash fast a few times to indicate CPU is booting
  digitalWrite(GPIO2, LOW);
  delay(100);
  digitalWrite(GPIO2, HIGH);
  delay(100);
  digitalWrite(GPIO2, LOW);
  delay(100);
  digitalWrite(GPIO2, HIGH);
  delay(100);
  digitalWrite(GPIO2, LOW);
  delay(100);
  digitalWrite(GPIO2, HIGH);

  Serial.println("Delaying a bit...");
  delay(2000);
*/
  // Initialise wifi connection
  wifiConnected = connectWifi();

  if (wifiConnected) {
    /*
    Serial.println("flashing slow to indicate wifi connected...");
    //flash slow a few times to indicate wifi connected OK
    digitalWrite(GPIO2, LOW);
    delay(1000);
    digitalWrite(GPIO2, HIGH);
    delay(1000);
    digitalWrite(GPIO2, LOW);
    delay(1000);
    digitalWrite(GPIO2, HIGH);
    delay(1000);
    digitalWrite(GPIO2, LOW);
    delay(1000);
    digitalWrite(GPIO2, HIGH);
    */
    
    Serial.println("starting upnp responder");
    upnpBroadcastResponder.beginUdpMulticast();

    // Define your switches here. Max 10
    // Format: Alexa invocation name, local port no, on callback, off callback
    SpeakerControl = new Switch("Tony's Speaker", 70, SpeakerOnRequest, SpeakerOffRequest);
   
    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*SpeakerControl);

  }

  Serial.println("Default with speaker on at startup  - Relay on...");
  pinMode(SpeakerControlPin, FUNCTION_3);
  pinMode(SpeakerControlPin, OUTPUT);
  digitalWrite(SpeakerControlPin, LOW); // turn on relay

}

void loop()
{

  delay(1000); // Main loop timer -

  if (wifiConnected) {

    upnpBroadcastResponder.serverLoop();
    
    SpeakerControl->serverLoop();
    
  }

 
}

bool SpeakerOnRequest() {
  Serial.println("Request to turn speaker on  - Relay  on...");

      digitalWrite(SpeakerControlPin, LOW); // turn on relay

  isSpeakerOn = true;
  return isSpeakerOn;
}

bool SpeakerOffRequest() {
  Serial.println("Request to turn speaker off received - Relay off...");

    digitalWrite(SpeakerControlPin, HIGH); // turn off relay

  isSpeakerOff = false;
  return isSpeakerOff;
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi() {
  boolean state = true;
  int i = 0;
  
  //WiFi.config(ip);
  WiFi.persistent(true);
  WiFi.mode(WIFI_STA);
  WiFi.config(DATA_IPADDR, DATA_GATEWY, DATA_IPMASK);
  //WiFi.config(IPAddress(ip));
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi Network");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.print(".");
    if (i > 10) {
      state = false;
      break;
    }
    i++;
  }

  if (state) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed. Bugger");
  }

  return state;
}


/*
 * 
 * Ideas to speed up connections - partially implemented above
 * 
 * 
#ifndef SECRETS_H
#define SECRETS_H

#define WIFI_SSID "YourWifiName"
#define WIFI_AUTH "YourWifiPassword"

// data from DHCP: IP address, network mask, Gateway, DNS
#define DATA_IPADDR IPAddress(192,168,178,111) 
#define DATA_IPMASK IPAddress(255,255,255,0)
#define DATA_GATEWY IPAddress(192,168,178,1)
#define DATA_DNS1   IPAddress(192,168,178,1)

// data from your Wifi connect: Channel, BSSID
#define DATA_WIFI_CH 6
#define DATA_WIFI_BSSID {0x12, 0x23, 0x34, 0x45, 0x56, 0x67}

#endif



 Uses hardcoded wifi data to connect, as a proof-of-concept.
 *  
 * DO NOT USE THIS CODE - if you ever change your wifi access point, or if it
 * changes channels (which they tend to do), your code will fail. It will not
 * try to connect to your access point in a normal way.
 * 
 * Use the other example if you want to copy & paste code.
 

#include "ESP8266WiFi.h"
#include "secrets.h"

/* Only do a fast connection with a given SSID, password, BSSID, channel.
 * This will fail when your access point does its normal work.
 
int wifi_fast() {
  WiFi.persistent(true); 
  WiFi.mode(WIFI_STA);
  WiFi.config(DATA_IPADDR, DATA_GATEWY, DATA_IPMASK);
  uint8_t my_bssid[] = DATA_WIFI_BSSID;
  char my_ssid[] = WIFI_SSID;
  char my_auth[] = WIFI_AUTH;
  WiFi.begin(&my_ssid[0], &my_auth[0], DATA_WIFI_CH, &my_bssid[0], true);
  uint32_t timeout = millis() + 5000;
  while ((WiFi.status() != WL_CONNECTED) && (millis()<timeout)) { delay(5); }
  return (WiFi.status() == WL_CONNECTED);  
}

/* One-time setup function. Does wifi connection and shows timing.
 
void setup() {
  WiFi.setAutoConnect(false); // prevent early autoconnect
  Serial.begin(115200);
  delay(1500); // just for debugging
  Serial.println("\nStarting.");
  uint32_t ts_start = millis();
  int wifi_fast_ok = wifi_fast();
  Serial.print("Timing wifi_fast(): "); Serial.print(millis()-ts_start); Serial.println("ms");
  Serial.print("Result="); Serial.println(wifi_fast_ok);
}

/* Main loop: waits 1 second, then reboots - so that it tries to connect multiple times.
 
void loop() {
  delay(1000); // wait, reboot, try again. 
  Serial.println("Rebooting.");
  ESP.restart();
}
 */





