#ifndef NTPTime_h
#define NTPTime_h

#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
  
class NTPTime 
{
private: 
  String url;
  int offset;
  bool isDayLightSavings; //flag is daylight savings in effect for the current datetime
  bool enableDayLightSavings; //flag for if the client cares about checking DST changes or not
  String AmPm = "";
  String errorMsg;
  time_t rawTime;
  
public:
  NTPTime(String myUrl, int myOffset, bool enableDST) :  
  url(myUrl),
  offset(myOffset),
  enableDayLightSavings(enableDST),
  isDayLightSavings(false)   
  {
    rawTime = timeClient.getEpochTime() - offset; 
  }

  //checks current date and time if DST is in effect and sets a flag to true if it and false if it isn't  
  void checkDST() {  
    Serial.println("checking DST");
    if(isDayLightSavingsTime()) {        
      isDayLightSavings = true;
      Serial.println("DST is in effect");
    } else {
      isDayLightSavings = false;
      Serial.println("Not DST");
    }
  }
  
  void begin() {    
    timeClient.begin();  
    timeClient.setTimeOffset(offset);    
  }
    
  void updateTime() {    
    timeClient.update(); 
    rawTime = timeClient.getEpochTime() - offset;
    //if raw time is less than may 13 2018 then we got bogus data from NTP and need to try again
    if(rawTime < 1526200196) {
      Serial.println("updating time");       
      begin();  
      updateTime();
    }    
  }

  

  String getTime24Hour() {
    updateTime();
    return timeClient.getFormattedTime();
  }

  time_t getRawTime() {
    return rawTime;
  }
  
  String getTimeFormatted() { 
    updateTime();       
    
    int hours = timeClient.getHours();
    int minutes = timeClient.getMinutes();

    //if daylight savings checking is enabled and it is currently daylight savings time,
    //just add 1 hour, otherwise the standard offset is already correct
    if(enableDayLightSavings) {
      if(isDayLightSavingsTime()) hours += 1;
    }

    //this adds a leading 0 to any minutes that are between 0 and 9 so they look good
    String minutesString = "";
    if(minutes < 10) { 
      minutesString += "0" + (String)minutes; 
    } else {
      minutesString = String(minutes);
    }
    
    //set the AmPm value for the current time
    if(hours < 12) {
      AmPm = "AM";   
    } else {
      AmPm = "PM";
    }  

    //change to 12 hour format
    if(hours == 0) { 
      hours = 12;   
    } 
    if (hours > 12) { 
      hours -= 12;  
    }
 
    return (String)hours + ":" + minutesString;
  }

  String getAmPm() {
    return AmPm;
  }

  boolean isDayLightSavingsTime()
  {
    //Receives unix epoch time and returns seconds of offset for local DST
    //Valid thru 2099 for US only, Calculations from "http://www.webexhibits.org/daylightsaving/i.html"
    //Code idea from jm_wsb @ "http://forum.arduino.cc/index.php/topic,40286.0.html"
    //Get epoch times @ "http://www.epochconverter.com/" for testing
    //DST update wont be reflected until the next time sync
    bool result = false;    
    int theYear = year(rawTime);
    int theMonth = month(rawTime);
    int theDay = day(rawTime);
    int theHour = hour(rawTime);    
    int beginDSTDay = (14 - (1 + theYear * 5 / 4) % 7);  
    int beginDSTMonth=3;
    int endDSTDay = (7 - (1 + theYear * 5 / 4) % 7);
    int endDSTMonth=11;
    if (((theMonth > beginDSTMonth) && (theMonth < endDSTMonth)) ||
        ((theMonth == beginDSTMonth) && (theDay > beginDSTDay)) ||
        ((theMonth == beginDSTMonth) && (theDay == beginDSTDay) && (theHour >= 2)) ||
        ((theMonth == endDSTMonth) && (theDay < endDSTDay)) ||
        ((theMonth == endDSTMonth) && (theDay == endDSTDay) && (theHour < 1))) {
        result = true;  //Add back in one hours worth of seconds - DST in effect
      }
    isDayLightSavings = result;
    return result;
}
};


#endif
