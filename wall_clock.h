#ifndef WALL_CLOCK
#define WALL_CLOCK

#include "utils.h"
#include "ChronoTwinedModule.h"
#include <Wire.h>
#include <RTClib.h>
#include <RTC_DS1307.h> // Same interface as DS3231


class WallClock : public ChronoTwinedFsmModule {
   RTC_DS1307 rtc;
   // DateTime _result;

  public:
   WallClock()
   :
         rtc()
   {}

   DateTime result() {
      // _result;
      return rtc.now();
   }

   bool is_ready() { return true; }

   void update() {
      switch (where_to_go()) {
      case Default:
         post_init_setup();
         go_next(Main);
      break;

      case Main:
      break;
      }
   }

   void log() {
      DateTime now = result();
      say(now.year(), DEC);
      say("-");
      say(now.month(), DEC);
      say("-");
      say(now.day(), DEC);
      say(" ");
      say(now.hour(), DEC);
      say(":");
      say(now.minute(), DEC);
      say(":");
      say(now.second(), DEC);
   }

  private:
   void post_init_setup() {
      // Post `setup` initialization (we don't want to do it in construction
      // before "main", because of arduino init orders.

      Wire.begin();
      rtc.begin();

      DateTime now = rtc.now();
      DateTime compiled = DateTime(__DATE__, __TIME__);

      if (now.unixtime() < compiled.unixtime()) {
         say("Wallclock time is older than the compile time! Updates!");
         // following line sets the RTC to the date & time this sketch was compiled
         rtc.adjust(DateTime(__DATE__, __TIME__));
      }
   }
};

#endif