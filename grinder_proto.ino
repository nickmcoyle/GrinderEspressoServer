/////// SMART GRINDER 1.1 CODE Nick "The Eggplant" Coyle and Constantine "C-man/Connie" Ichim   May 2017 /////////


//amount of coffee we want to grind before shutting off
  float loadLimit = 18;
  float lagFactor = 0.40;
  float savedLimit;    //variable used to save the limit in eeprom
  int eeAddress = 0;   //Location we want the data to be put.
  //for eeprom
  #include <EEPROM.h>
  
//for relay
const int relayOutputPin = 2;
const int relayButtonPin = 3;
int relayButtonState;    // current state of the button
int lastRelayButtonState = HIGH;  // previous state of the button
boolean grinding = false;

//for timer and shut off
 boolean done = false;
 int interval = 2000;
 
//for load cell
#include "HX711.h"
#define DOUT  A1
#define CLK  A0
HX711 scale(DOUT, CLK);   
float calibration_factor = 1961; //1935 previously
float load; //the output from the cell
//end load cell

//for lcd display
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C  lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the I2C bus address for an unmodified module
static const unsigned long REFRESH_INTERVAL = 1000; // ms
static unsigned long lastRefreshTime = 0;
//end lcd display

//buttons and lights
const int tareButtonLED = 4;
const int plusButtonPin = 5;
const int minusButtonPin = 6;
const int tareButtonPin = 7;

//for BLE
//"RBL_nRF8001.h/spi.h/boards.h" library is needed in the project
#include <SPI.h>
#include <boards.h>
#include <RBL_nRF8001.h>
char bleValue;

void setup() {

//for grind limit saved in eeprom
EEPROM.get(eeAddress, savedLimit);
//if the limit is set to a real value, then save it
if(savedLimit > 0 ) {
    loadLimit = savedLimit;  //Variable to store in EEPROM.
}

//for ble

 // Default pins set to 9 and 8 for REQN and RDYN
  // Set your REQN and RDYN here before ble_begin() if you need
  //ble_set_pins(3, 2);
  
  // Set your BLE Shield name here, max. length 10
  //ble_set_name("My Name");
  
  // Init. and start BLE library.
  ble_begin();
  

  //for scale
  scale.set_scale();
  scale.tare();  //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading


pinMode(relayButtonPin, INPUT_PULLUP);
pinMode(tareButtonPin, INPUT_PULLUP);
pinMode(minusButtonPin, INPUT_PULLUP);
pinMode(plusButtonPin, INPUT_PULLUP);
pinMode(tareButtonLED, OUTPUT);
pinMode(relayOutputPin, OUTPUT);
//pinMode(LED_BUILTIN, OUTPUT);


  Serial.begin (9600); 
  
  //for lcd display
  lcd.setBacklightPin(3,POSITIVE);
    lcd.setBacklight(HIGH); // NOTE: You can turn the backlight off by setting it to LOW instead of HIGH
    lcd.begin(16, 2);
    lcd.clear();
 //end lcd display

PrintInitializing();

}

void PrintInitializing() {

lcd.print("Initializing");
delay(250);
int i = 0;
while(i < 5) {
  delay(250);
  lcd.print(".");
  i++;
  }
  delay(250);
  lcd.clear();
  lcd.print("Initializing");
  i = 0;
while(i < 5) {
  delay(250);
  lcd.print(".");
  i++;
  }
 delay(250);
  scale.tare();  //Reset the scale to 0 again

}




void loop() {

bluetoothConnect();
checkRelayButton();
checkTareButton();
checkLimitButtons();
checkScale();
refreshLCD();

//for tare button light
digitalWrite(tareButtonLED, HIGH);


//start grinding and stop when reached the limit
if( (grinding == true) && (done == false) ) { //only run this if relay is turned on
 if ( (load >= (loadLimit - lagFactor) )  ) { //it usually seems to go over by 0.3-.5g every time
        digitalWrite(relayOutputPin, LOW);
          //code to delay stopping the cycle in case the scale reads high or weird for a second
           delay(interval);   
   if( load >= (loadLimit-lagFactor) ) {
          //then we really must have reached the desired weight or close enough 
            done = true;
            grinding = false;
            digitalWrite(relayOutputPin, LOW);   
    }
      
     
  } else  {
     if(done == false) {
       digitalWrite(relayOutputPin, HIGH);
     }
  }
} else {
  digitalWrite(relayOutputPin, LOW); //make sure grinder is turned off when the button wasn't pressed
  }
  
}



void bluetoothConnect() { 

  if ( ble_connected() )
  {

    //send data from arduino to app
    char buffer[10];
dtostrf(load, 4, 2, buffer); // float, StringLength including decimal point, number of     digits after decimal, string array to store the results
ble_write_bytes( (unsigned char *)buffer, loadLimit);

    while ( ble_available() )
    {
      //Serial.write( ble_read() ); //for debugging in serial monitor
      bleValue = ble_read();
 
      switch(bleValue)
{
case 't':
if(grinding == false) {
  resetSystem();
}
break;
case 'g':
if(grinding == false && done == false) {
grinding = true; 
}
break; 
case '+':  
changeLoadLimit('+');
break;
case '-': 
changeLoadLimit('-');
break;
default:
if( lagFactor != bleValue ) {
//changeLagFactor(bleValue);
}
break;        
}


    //send data from arduino to app
//     double val1 = loadLimit;
//     double val2 = lagFactor; 
//  
//    ble_write(val1);
//    ble_write(val2);
    }
    
    }
  ble_do_events();
  
  }


void checkRelayButton() {
  
//for relay switch
int relayButtonState = digitalRead(relayButtonPin);
int relayState = digitalRead(relayOutputPin);
 
  if ( (relayButtonState != lastRelayButtonState) && (grinding != true) ) {

  if ( (relayButtonState == HIGH) && (done == false)  ) {
 grinding = true;   
  }
  
lastRelayButtonState = relayButtonState;
  }
  // set the relay:
if(grinding == true) { 
  digitalWrite(relayOutputPin, HIGH);
  done = false;
}
  
  }



void checkTareButton() {

//for tare switch
int tareButtonState = digitalRead(tareButtonPin); 
if( (tareButtonState == LOW) && (grinding == false) ){
  resetSystem();
 
}
  
}

void resetSystem() {
   scale.tare();  //Reset the scale to 0
  done = false; //reset the grind done state 
  lcd.clear(); 
}

void checkLimitButtons() {

//for plus and minus buttons
int plusButtonState = digitalRead(plusButtonPin);
if(plusButtonState == LOW) {
  changeLoadLimit('+');
  
}
int minusButtonState = digitalRead(minusButtonPin);
if(minusButtonState == LOW) {
  changeLoadLimit('-');
}
  
}
void changeLoadLimit(char c) {
  if (c == '+') { 
  loadLimit+= 1;
  }
  else if (c == '-') {
    loadLimit-= 1;
  } 
   savedLimit = loadLimit;  //Variable to store in EEPROM.  
  //One simple call, with the address first and the object second.
  EEPROM.put(eeAddress, savedLimit);  
   // eeAddress += sizeof(float); //Move address to the next byte after float 'f'.

}

void changeLagFactor(float lf) {
  lagFactor = lf;
}

void checkScale() {

// for load cell 
scale.set_scale(calibration_factor); //Adjust to this calibration factor
 load = scale.get_units();
if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a')
      calibration_factor += 10;
    else if(temp == '-' || temp == 'z')
      calibration_factor -= 10;
  }
  
}


void refreshLCD() {

//for lcd display 
//refresh every 1 second
  if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
  {
    lastRefreshTime += REFRESH_INTERVAL;
                 lcd.clear();    
  }

lcd.setCursor(0,0);
    
    if (  (done == false) && (grinding == false) ) {
      lcd.print("Ready");    
    } else if ( (grinding == true) && (done == false) ) {      
     lcd.print("...");
   } else if ( (done == true) && (relayButtonState == HIGH)) {
    //print RESET and blink the tare LED slowly
      lcd.print("RESET");      
      delay(100);
      digitalWrite(tareButtonLED, LOW);
   //lcd.print(relayButtonState);
   } else  {
     lcd.print("done");
   
   }
    lcd.print("  Stop: ");
    lcd.print(loadLimit,0);
    lcd.print("g");
    lcd.setCursor(0,1);
    lcd.print("     ");
   // lcd.print("lf: ");
   // lcd.print(lagFactor);  
   // lcd.print(" ");  
    if(load > 0) {
    lcd.print(" ");
    lcd.print(load);
    } else {
      lcd.print(load);
    }
    lcd.print("g");
   lcd.print("  ");   
    delay(250);
//end lcd display

}

