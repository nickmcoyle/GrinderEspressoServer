#include "Display.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "NTPTime.h"
#include "OTAUpdate.h"
#include "TemperatureSensing.h"

//Global constants
const char* MY_WIFI_SSID = "";
const char* MY_WIFI_PASSWORD = "";
const IPAddress MY_IP_ADDRESS(192,168,1,249); 
const IPAddress MY_GATEWAY(192,168,1,1);
const IPAddress MY_SUBNET(255,255,255,0);
const IPAddress MY_DNS(192,168,1,1);
const int LED_PIN = D5;
const int RELAY_PIN = D7;
const unsigned long RUNTIME_LIMIT = 60*60*1000;
const uint16_t WWW_PORT = 9675;

//Global variables
NTPTime myTime("pool.ntp.org", -8*3600, true);
float Temperature = 0;
unsigned long PowerOnTime = 0;
static unsigned long LastRefreshDisplay = 0;
static unsigned long LastRefreshTime = 0;
unsigned long DST_CHECK_INTERVAL = 10*1000;
unsigned long currentMillis = 0;
String WebSite, Javascript, XML;

ESP8266WebServer server(WWW_PORT);

void setup() {
  Serial.begin(115200);
  Serial.println("Serial begin");
  initSecureVariables(); //found in SecureVariables.ino redefines MY_WIFI_SSID and MY_WIFI_PASSWORD
  initPins();
  initDisplay();
  initWifi();  
  initWebServer();  
  initOTA();   
  initTemperatureSensor();  
  currentMillis = millis();
  LastRefreshDisplay = millis();
  LastRefreshTime = millis();
  myTime.begin();    
}

void loop() {
  server.handleClient();
  //ArduinoOTA.handle();  
  checkSafety();  
  
  //refresh display and get temperature reading from sensor every 2 seconds
  if(millis() - LastRefreshDisplay >= 2*1000)
  {
    LastRefreshDisplay = millis();
    refreshDisplay(); 
    updateTemperature();     
  }  
  
  //refresh the time
  if(millis() - LastRefreshTime >= 10*1000)
  {    
    LastRefreshTime = millis();    
    Serial.print("Raw time is: ");
    Serial.println(myTime.getRawTime());
    Serial.print("Formatted time is: ");
    Serial.println(myTime.getTimeFormatted() + myTime.getAmPm());
   
    Serial.println("");
  }

/*
  //wait 30 seconds to check for DST
  if(millis() - currentMillis >= DST_CHECK_INTERVAL) {
    currentMillis = millis();    
    DST_CHECK_INTERVAL += 60*1000;
    myTime.checkDST();
  }
  */
}

/**
 * A method to start the wifi connection to our access point
 */
void initWifi() {
  
  // Connect to WiFi network
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Connecting to:");
  display.println(MY_WIFI_SSID);
  display.display();
  
  WiFi.config(MY_IP_ADDRESS, MY_GATEWAY, MY_SUBNET, MY_DNS);  
  WiFi.mode(WIFI_STA);
  WiFi.begin(MY_WIFI_SSID, MY_WIFI_PASSWORD);
  
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 100 );
    Serial.print ( "." );
    display.print(".");
    display.display();
  }
  Serial.println("");
  Serial.println("Wifi connected");
  Serial.println("");
  
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
* A method to calculate the amount of time the relay has been on. Returns milliseconds.
**/
unsigned long getRuntime() {
  unsigned long runTime = 0;
  if (getRelayState() == 1) {
    runTime = millis() - PowerOnTime;
  }
  return runTime;
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
  digitalWrite(RELAY_PIN, !getRelayState());
  digitalWrite(LED_PIN, !getLEDState());
  server.send(200, "text/plain", "Okay");
  //if the coffee machine is on, then reset the run timer, else it should be 0
  if(getRelayState() == 1) {
    PowerOnTime = millis();
  } else {
    PowerOnTime = 0;
  }
}
