#include "TimeUtils.h"
#include <Arduino.h>
#include <time.h>


const char* mainNTPServer         = "pool.ntp.org";
const char* secondaryNTPServerA   = "time.google.com";
const char* secondaryNTPServerB   = "time.windows.com";

TimeUtils::TimeUtils(){}

void TimeUtils::setClock(String timezone) {
  configTime(0, 0, mainNTPServer, secondaryNTPServerA, secondaryNTPServerB);
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    yield();
    delay(500);
    Serial.print(F("."));
    now = time(nullptr);
  }
  
  setenv("TZ",timezone.c_str(), 0);
  tzset();
}

String TimeUtils::getTime(){
  time_t now = time(nullptr);
  char buf[sizeof "2011-10-08T07:07:09"];
  strftime(buf, sizeof buf, "%FT%T", gmtime(&now));
  String result = buf;
  result.substring(2, result.length()-2); //Trim away CTRL-CHAR
  return result.c_str();
}
