#include "NTPTime.h"
#include "Display.h"
#include "OTAUpdate.h"
#include "TemperatureSensing.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

//Global constants
const char* MY_WIFI_SSID = "your ssid";
const char* MY_WIFI_PASSWORD = "your password";
const IPAddress MY_IP_ADDRESS(192,168,1,201); 
const IPAddress MY_GATEWAY(192,168,1,1);
const IPAddress MY_SUBNET(255,255,255,0);
const int LED_PIN = D5;
const int RELAY_PIN = D7;
const unsigned long REFRESH_INTERVAL = 2000; // ms
const uint32_t RUNTIME_LIMIT = 3600; //seconds in 1 hour

//Global variables
NTPTime myTime("81.94.123.17", -8, true);
float Temperature = 0;
unsigned long PowerOnTime = 0;
static unsigned long LastRefreshDisplay = 0;
static unsigned long LastRefreshTime = 0;
String WebSite, Javascript, XML;
const char* WWW_USERNAME = "coffee"; //for url auth
const char* WWW_PASSWORD = "1234"; //for url auth
const uint16_t WWW_PORT = 9675;
ESP8266WebServer server(WWW_PORT);

void setup() {
  initSecureVariables(); //found in SecureVariables.ino redefines MY_WIFI_SSID and MY_WIFI_PASSWORD
  initPins();
  initDisplay();
  initWifi();  
  initWebServer();  
  initOTA();   
  initTemperatureSensor();
  myTime.updateTime();
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();  
  checkSafety();
  
  //refresh display and get temperature reading every 2 seconds
  if(millis() - LastRefreshDisplay >= REFRESH_INTERVAL)
  {
    LastRefreshDisplay += REFRESH_INTERVAL;
    refreshDisplay(); 
    updateTemperature();  
  }  
  
  //refresh the time every 20 seconds
  if(millis() - LastRefreshTime >= 20000)
  {
    LastRefreshTime += REFRESH_INTERVAL;
    myTime.updateTime();
  }
}

/**
 * A method to start the wifi connection to our access point
 */
void initWifi() {
  //As a solution I put
  //in the beginning of setup() and lo-behold!! Crashes no more! 
  WiFi.persistent(false);
  
  // Connect to WiFi network
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Connecting to:");
  display.println(MY_WIFI_SSID);
 
  WiFi.config(MY_IP_ADDRESS, MY_GATEWAY, MY_SUBNET);
  WiFi.begin(MY_WIFI_SSID, MY_WIFI_PASSWORD);
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("WiFi connected"); 
  display.display();
}

void initPins() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  
}

/**
* A method to calculate the amount of time the relay has been on
**/
unsigned long getRuntime() {
  unsigned long runTime = 0;
  if (getRelayState() == 1) {
    runTime = millis() - PowerOnTime;
    runTime /= 1000;  
  }  
     
  return runTime;
}

/**
 * A method to length of time the relay has been on and format it for display
 */
String formatRuntime(unsigned long runTime) {
  String timeStr = "--";
  byte hh, mm, ss;
  if(runTime > 0) {
    hh = runTime / 3600;
    mm = (runTime - (hh*3600)) / 60;
    ss = (runTime - (hh * 3600) - (mm * 60));
    if (hh < 10)timeStr += "0";
    timeStr += (String)hh + ":";
    if (mm < 10)timeStr += "0";
    timeStr += (String)mm + ":";
    if (ss < 10)timeStr += "0";
    timeStr += (String)ss;
  }

  return timeStr;
}

/**
 * A method to turn off the relay if it has been on for 1 hour just for safety
 */
void checkSafety() {
  if (getRuntime() >= RUNTIME_LIMIT) {
    togglePower();
  }
}

/**
 * A method to get the temperature reading from the sensor
 */
long getTemperature() {
  return Temperature;
}

/**
 * A method to refresh the temperature reading from the sensor
 */
void updateTemperature() {
  sensors.requestTemperatures(); // Send the command to get temperatures
  Temperature = sensors.getTempFByIndex(0);
}

/**
 * A method to update what is shown on the display screen
 */
void refreshDisplay() {  
  display.setTextSize(2);
  display.setCursor(0,0);
  display.printf("%.1f",Temperature);
  display.setTextSize(1);
  display.print((char)247);
  display.print("F"); 
  display.setTextSize(2); 
  display.print('\n');    
  display.print(myTime.getTimeFormatted());
  display.setTextSize(1);
  display.print(myTime.getAmPm());
  display.display();
  display.clearDisplay();
}

/**
 * A method to get the state of the relay, if it is on or not
 */
int getRelayState() {
   return digitalRead(RELAY_PIN);
}

/**
 * A method to get the state of the LED, if it is on or not
 */
int getLEDState() {
   return digitalRead(LED_PIN);
}

/**
 * A method to get the state of the relay, if it is on or not and return a String: on or off
 */
String getRelayStatus() {
  String relayStatus;
  
  if (getRelayState() == 0) {
    relayStatus = "off";
  } else {
    relayStatus = "on";
  }
  return relayStatus;
}

/**
 * A method to turn the relay and LED off/on
 */
void togglePower() {
  PowerOnTime = millis();  
  digitalWrite(RELAY_PIN, !getRelayState());
  digitalWrite(LED_PIN, !getLEDState());
  server.send(200, "text/plain", "Okay");
}
