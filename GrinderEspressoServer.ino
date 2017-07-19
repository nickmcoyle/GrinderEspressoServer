//This example will set up a static IP - in this case 192.168.1.201


///////////for 0.96" OLED display ssd1306 clone from ebay
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);
 
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
 
 
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

static const unsigned long REFRESH_INTERVAL = 2000; // ms
static unsigned long lastRefreshTime = 0;

///////////end OLED display

/////for time
#include <TimeLib.h>
#include <NtpClientLib.h>
///end time

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//for OTA upload
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// Data wire is plugged into pin 2 on the Arduino
//#define ONE_WIRE_BUS 2
int ONE_WIRE_BUS = D6;

// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
float temp;

const char* ssid = "iprotectmykids";
const char* password = "4905ndugu";

int ledPin = D5;
int relayPin = D7;

unsigned long previousMillis = 0;
unsigned long ss = 0;

ESP8266WebServer server(9675);
IPAddress ip(192, 168, 1, 201); // where xx is the desired IP Address
IPAddress gateway(192, 168, 1, 1); // set gateway to match your network
//for auth
const char* www_username="coffee";
const char* www_password = "1234";

String webSite, javaScript, XML;

void buildWebsite() {
  buildJavascript();
  webSite = "<!DOCTYPE HTML>\n";
  webSite += "<head>\n";
  webSite += "<meta charset='utf-8'>\n";
  webSite += "<meta http-equiv='X-UA-Compatible' content='IE=edge'>\n";
  webSite += "<meta name='viewport' content='width=device-width, initial-scale=1'>\n";
  //css
  webSite += "<style>\n";
  webSite += "body {font-size:3.5em;}\n";
  webSite += "h1 {font-size:2em;text-align:center}\n";
  webSite += ".toggle.ios, .toggle-on.ios, .toggle-off.ios { border-radius: 20px;}\n";
  webSite += ".toggle.ios .toggle-handle { border-radius: 20px; }\n";
  webSite += ".slow .toggle-group { transition: left 0.7s; -webkit-transition: left 0.7s; }\n";
  webSite += "</style>\n";
  //bootstrap css
  webSite += "<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css' integrity='sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u' crossorigin='anonymous'>\n";
  //bootstrap theme
  webSite += "<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css' integrity='sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp' crossorigin='anonymous'>\n";
  //bootstrap toggle switch
  webSite += "<link href='https://gitcdn.github.io/bootstrap-toggle/2.2.2/css/bootstrap-toggle.min.css' rel='stylesheet'>\n";
  
  webSite += "</head>\n";
  webSite += "<BODY onload='process()'>\n";

  //load scripts
  //load jQuery
  webSite += "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>\n";
  //bootstrap js
  webSite += "<script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js' integrity='sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa' crossorigin='anonymous'></script>\n";
  //bootstrap toggle
  webSite += "<script src='https://gitcdn.github.io/bootstrap-toggle/2.2.2/js/bootstrap-toggle.min.js'></script>\n";
  //font awesome icons
  webSite += "<script src='https://use.fontawesome.com/5ff54f6697.js'></script>\n";
  
  webSite += "<div class='container-fluid'>\n";
  
  webSite += "<div class='row'>\n";
  webSite += "<div class='col-md-4 col-sm-1 col-xs-1'></div>\n"; //edge margin
  webSite += "<div class='col-md-4 col-sm-10 col-xs-10' id='appContainer'>\n"; //begin app container
  webSite += "<BR><h1>GRINDER</h1><BR>\n";
  
  webSite += "<div class='row'>\n";
  webSite += "<h3>COFFEE MACHINE</h3>\n";
  webSite += "<div class='col-md-9 col-sm-9 col-xs-9' id='rt'>\n";
  webSite += "Runtime <A id='runtime'></A>\n";
  webSite += "</div>\n"; //end col rt
  webSite += "<div class='col-md-3 col-sm-3 col-xs-3'></div>\n";
  webSite += "</div>\n"; //end row
  
  webSite += "<div class='row'>\n";
  webSite += "<div class='col-md-9 col-sm-9 col-xs-9' id='temperature'>\n";
  webSite += "Temperature <A id='temp'></A>&#176;F\n";
  webSite += "</div><BR>\n"; //end col temperature
  webSite += "<div class='col-md-3 col-sm-3 col-xs-3'></div>\n";
  webSite += "</div>\n"; //end row

  webSite += "<BR><BR><div class='row'>\n";
  webSite += "<div class='col-md-6 col-sm-8 col-xs-8' id='coffeeMachinePowerStatus'>\n";
  webSite += ("Coffee machine <A id='pinStatus'></A>\n");
  webSite += "</div>\n"; //end coffeeMachinePowerStatus col
  webSite += "<div class='col-md-6 col-sm-3 col-xs-3' id='coffeeMachinePowerBtn'>\n";
  webSite += "<input id='powerBtn1' disabled type='checkbox' data-onstyle='danger' data-toggle='toggle' data-style='ios slow' data-on='<i class=\"fa fa-power-off\"></i> On&nbsp;' data-off='<i class=\"fa fa-power-off\"></i> Off&nbsp;' ></input>\n";
  webSite += "</div>\n"; //end coffeeMachinePowerBtn col
  webSite += "</div>\n"; //end row
  
  webSite += "<BR><BR><div class='row'>\n";
  webSite += "<div class='col-md-12 col-sm-12 col-xs-12 text-center'>\n";
  webSite += "<A id='loading'></A>\n";
  webSite += "</div>\n"; //end col
  webSite += "</div>\n"; //end row
  webSite += "</div>\n"; //end app container
  webSite += "<div class='col-md-4 col-sm-1 col-xs-1'></div>\n"; //edge margin
  webSite += "</div>\n"; //end row
  webSite += "</div>\n"; //end container fluid
  
  webSite += javaScript;
  webSite += "</BODY>\n";
  webSite += "</HTML>\n";
}

void buildJavascript() {

  javaScript = "<SCRIPT>\n";
  javaScript += "var grinder = { on:false, powerBtn1On:null }; \n";
  javaScript += "var xmlHttp=createXmlHttpObject();\n";
  javaScript += "function createXmlHttpObject(){\n";
  javaScript += " if(window.XMLHttpRequest){\n";
  javaScript += "    xmlHttp=new XMLHttpRequest();\n";
  javaScript += " }else{\n";
  javaScript += "    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n";
  javaScript += " }\n";
  javaScript += " return xmlHttp;\n";
  javaScript += "}\n";

  javaScript += "function process(){\n";
  javaScript += " if(xmlHttp.readyState==0 || xmlHttp.readyState==4) {\n";
  javaScript += "   xmlHttp.open('PUT','xml',true);\n";
  javaScript += "   xmlHttp.onreadystatechange=handleServerResponse;\n";
  javaScript += "   xmlHttp.send(null);\n";
  javaScript += " }\n";
  javaScript += " setTimeout('process()',1000);\n";
  javaScript += " }\n";

  javaScript += "function handleServerResponse(){\n";
  javaScript += " if(xmlHttp.readyState<4){\n";
  //javaScript+="   document.getElementById('loading').innerHTML = 'Loading...';\n";
  javaScript += " }\n";
  javaScript += " else if(xmlHttp.readyState==4 && xmlHttp.status==200){\n";
  javaScript += "   document.getElementById('loading').innerHTML = '';\n";
  javaScript += "   xmlResponse=xmlHttp.responseXML;\n";
  javaScript += "   xmldoc = xmlResponse.getElementsByTagName('temp');\n";
  javaScript += "   temp = xmlHttp.responseXML.getElementsByTagName('temp')[0].firstChild.nodeValue;\n";
  javaScript += "   runtime = xmlHttp.responseXML.getElementsByTagName('runtime')[0].firstChild.nodeValue;\n";
  javaScript += "   pinStatus = xmlHttp.responseXML.getElementsByTagName('pinStatus')[0].firstChild.nodeValue;\n";
  javaScript += "   document.getElementById('runtime').innerHTML=runtime;\n";
  javaScript += "   document.getElementById('temp').innerHTML=temp;\n";
  javaScript += "   document.getElementById('pinStatus').innerHTML=pinStatus;\n";
  javaScript += " if(pinStatus == 'on') { grinder.on = true; } \n";
  javaScript += " else if (pinStatus == 'off') { grinder.on = false; } ;\n";
  javaScript += " }\n";
  javaScript += " }\n";


  //used for post
  javaScript += "   function togglePower1(hideLoading){\n";  
  javaScript += "  if(  ( (grinder.on == true) && (grinder.powerBtn1On == false) ) ||  ( (grinder.on == false) && (grinder.powerBtn1On == true) )  ) { \n";
  javaScript += "   document.getElementById('loading').innerHTML = 'Loading...';\n";
  javaScript += "   var http = new XMLHttpRequest();\n";
  javaScript += "   var url = 'togglePower1';\n";
  javaScript += "   var params = 'togglePower=togglePower1';\n";
  javaScript += "   http.open('POST','togglePower1',true);\n";
  javaScript += "   http.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');\n";
  javaScript += "   http.onreadystatechange= function togglePower(){\n";
  javaScript += "}\n";
  javaScript += "   http.send(params);\n";
  javaScript += " }\n";  
  javaScript += " $('#powerBtn1').bootstrapToggle('enable'); \n";
  javaScript += " }\n";
  
  javaScript += "  $(document).ready(function() { \n";
    javaScript += "  window.setTimeout(fixPowerBtns,3000); \n";
  javaScript += "  $('#coffeeMachinePowerBtn').on('click', function() { if(grinder.powerBtn1On === true){grinder.powerBtn1On = false} else (grinder.powerBtn1On = true); }); \n";
  javaScript += "  $('#powerBtn1').change(function() { \n";
  javaScript += "  $('#powerBtn1').bootstrapToggle('disable') \n"; 
  javaScript += "  togglePower1(); \n";
  javaScript += " }); \n";
  javaScript += " }); \n";

  javaScript += "   function fixPowerBtns() {\n";
  javaScript += "  $('#powerBtn1').bootstrapToggle('enable'); \n";
  javaScript += " if(grinder.on === true ) { grinder.powerBtn1On = true; $('#powerBtn1').bootstrapToggle('on'); } \n";
  javaScript += " else { grinder.powerBtn1On = false; $('#powerBtn1').bootstrapToggle('off'); } \n";
  javaScript += " }\n";

  javaScript += "   function hideLoading() {\n";
  javaScript += " document.getElementById('loading').innerHTML = '';\n";
  javaScript += " }\n";

  javaScript += "</SCRIPT>\n";

}

void buildXML() {
  XML = "<?xml version='1.0'?>";

  XML += "<response>";

  XML += "<runtime>";
  XML += millis2time();
  XML += "</runtime>";

  XML += "<temp>";
  XML += getTemp();
  XML += "</temp>";

  XML += "<pinStatus>";
  XML += getPinStatus();
  XML += "</pinStatus>";

  XML += "</response>";
}

String millis2time() {
  String Time = "";
  byte mm, hh;

  int pinstatus = digitalRead(relayPin);
  if (pinstatus == 1) { //if relay is on
    ss = millis() - previousMillis;
    ss = ss / 1000;
    //if relay has been on for 1/2 hour, then please turn it off
    if ( ss >= 1800  ) {
      digitalWrite(relayPin, LOW);
      digitalWrite(ledPin, LOW);
      return Time = "--";
    }
    hh = ss / 3600;
    mm = (ss - hh * 3600) / 60;
    ss = (ss - hh * 3600) - mm * 60;
    if (hh < 10)Time += "0";
    Time += (String)hh + ":";
    if (mm < 10)Time += "0";
    Time += (String)mm + ":";
    if (ss < 10)Time += "0";
    Time += (String)ss;
  } else {
    Time = "--";
  }
  return Time;
}

float getTemp() {
  sensors.requestTemperatures(); // Send the command to get temperatures
  float temp = sensors.getTempFByIndex(0);
   return temp;
}

void refreshDisplay() {
 float temp = getTemp(); 
 display.setTextSize(2);
 display.setCursor(0,0);
 display.print(temp);
 display.print((char)247);
 display.print("F"); 
  //next line
  display.print('\n');
  //get the time str
  String timeStr = NTP.getTimeStr();
  //for it nicer
  timeStr.remove(5); //removes the seconds
  int hh = (timeStr.substring(0,2)).toInt();  
  if(hh == 0) { hh = 1; 
  //timeStr += "AM"; 
  } //1am
  else if (hh > 12 && hh < 24) { hh -= 12; 
  //timeStr += "PM";
  }
  else if (hh == 24) { hh -= 12;
  //timeStr += "AM"; 
  }
  //else if (hh < 12) { timeStr += "AM"; }
  //else { timeStr += "PM"; }
  String niceHour = String(hh);
  timeStr.replace( timeStr.substring(0,2), niceHour);
  //display it on the OLED screen
  display.print(timeStr);
  display.display();
  display.clearDisplay();
}

String getPinStatus() {
  String pinStatus;
  int pinstatus = digitalRead(relayPin);
  if (pinstatus == 0) {
    pinStatus = "off";
  } else if (pinstatus == 1) {
    pinStatus = "on";
  }
  return pinStatus;
}

void handleWebsite() {
  buildWebsite();
  server.send(200, "text/html", webSite);
}

void handleXML() {
  buildXML();
  server.send(200, "text/xml", XML);
}

void togglePower1() {
  previousMillis = millis();
  int relayState = digitalRead(relayPin);
  int ledState = digitalRead(ledPin);
  //toggle them
  digitalWrite(relayPin, !relayState);
  digitalWrite(ledPin, !ledState);
  server.send(200, "text/plain", "Okay");
}


void setup() {


 //As a solution I put:
  WiFi.persistent(false);
  //in the beginning of setup() and lo-behold!! Crashes no more!

  Serial.begin(115200);
  delay(10);

  pinMode(relayPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(relayPin, LOW);

  // Connect to WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  //server.on("/", handleWebsite);
  server.on("/xml", handleXML);
  server.on("/togglePower1", togglePower1);

//auth
server.on("/", [](){
  if(!server.authenticate(www_username, www_password))
        return server.requestAuthentication();
      handleWebsite();
    server.send(200, "text/plain", "Login OK");
  });
  
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  // Start up the library
  sensors.begin();

  //OTA code
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);
  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");
  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  //end OTA code


///////////for OLED display
// by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C 
  // init done 
  display.display();
   // Clear the buffer.
  display.clearDisplay(); 
  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Hello, Nick. It's nice to see you.");
  display.display();  
  display.clearDisplay();

/////////////////end OLED display

//for time
//Before you can retrieve the time on the Internet, you must already start the NTP object. 
//It is started with the begin() method which takes 3 parameters in the setup():
//The time server URL. By default, you can query pool.ntp.org
//Time difference that matches your time zone
//Summer time. Set to True to manage daylight saving mode, winters
NTP.begin("pool.ntp.org", -8, true);
NTP.setInterval(60000);

NTP.onNTPSyncEvent([](NTPSyncEvent_t error) {
  if (error) {
    Serial.print("Time Sync error: ");
    if (error == noResponse)
      Serial.println("NTP server not reachable");
    else if (error == invalidAddress)
      Serial.println("Invalid NTP server address");
    }
  else {
    Serial.print("Got NTP time: ");
    Serial.println(NTP.getTimeDateString(NTP.getLastNTPSync()));
  }
});

//end time

}

void loop() {

  server.handleClient();

  ArduinoOTA.handle();


 //for OLED display 
//refresh every 1 second
  if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
  {
    lastRefreshTime += REFRESH_INTERVAL;
                 refreshDisplay(); 
  }
  
}

