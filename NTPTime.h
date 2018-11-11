#ifndef NTPTime_h
#define NTPTime_h

#include <TimeLib.h>
#include <NtpClientLib.h>

class NTPTime 
{
private: 
  String url;
  int offset;
  bool isDaylightSavings;
  String timeRaw = "";
  String AmPm = "";
  String errorMsg;
  
public:
  NTPTime(String url, int offset, bool isDS) :
  url(url),
  offset(offset),
  isDaylightSavings(isDS) 
  {
    getConnection();
  }

  void updateTime() {
    timeRaw = NTP.getTimeStr();
  }
  
  String getTimeFormatted() {  
    return formatTime(timeRaw);
  }

  String getAmPm() {
    return AmPm;
  }

  String getTimeRaw() { 
    return timeRaw;
  }
  
  String formatTime(String timeStr) {  
    //remove the seconds
    timeStr.remove(5); 
  
    //extract the hour as int
    int hh = (timeStr.substring(0,2)).toInt();  
    
    if(hh < 12) {
      AmPm = "AM";   
    } else {
      AmPm = "PM";
    }  
    
    if(hh == 0) { 
      hh = 12;   
    } 
    if (hh > 12) { 
      hh -= 12;  
    }
    
    String hourStr = String(hh);  
    timeStr.replace( timeStr.substring(0,2), hourStr);
      
    return timeStr;
  }

  /**
 * Before you can retrieve the time on the Internet, you must already start the NTP object. 
 * It is started with the begin() method which takes 3 parameters in the setup():
 * 1. String: the time server URL. By default, you can query pool.ntp.org
 * 2. int: local time offset from UTC
 * 3. boolean: true if this timezone has daylight savings mode
 * 
  */
  void getConnection() {
  NTP.begin(url, offset, isDaylightSavings);
  NTP.setInterval(60000);

  NTP.onNTPSyncEvent([this](NTPSyncEvent_t error) {
    if (error) {          
      this->errorMsg = "Time Sync error: \n";
      if (error == noResponse) {
        this->errorMsg += "NTP server not reachable\n";
      } else if (error == invalidAddress) {
        this->errorMsg += "Invalid NTP server address\n";
      } else {        
        NTP.getTimeDateString(NTP.getLastNTPSync());
      }      
    }
  });
  
}
};


#endif
