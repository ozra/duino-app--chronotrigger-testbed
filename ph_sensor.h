#include "ChronoTwinedModule.h"
#include "SignalProcessingEma.h"
#include "utils.h"

class PhOne : public ChronoTwinedFsmModule {
   F32                        result;
   I8                         sensor_pin = -1;
   SignalProcessing::Ema<F32> ema = SignalProcessing::Ema<F32>(); // *TODO* use outlier filtering over a window size and % of sorted middle taken for avg'ing
   TimeSpan                   measure_interval;

  public:
   PhOne(
      I8 sensor_pin_,
      TimeSpan measure_interval_ = 100,
      F32 filter_window_ = 20.0
   ) :
      sensor_pin(sensor_pin_),
      measure_interval(measure_interval_)
   {
      ema.change_k(1.0 / filter_window_);
      pinMode(sensor_pin, INPUT);
   }

   void log() {
      say("pH: ");
      say(result, 2);
   }

   void update() {
      switch (where_to_go()) {
      case Default:
         if (sensor_pin == -1) {
            fatal("\n\n\nPhOne has not been configured via `setup()`!\n\n\n");
            delay(30000);
         } else {
            go_next(Main);
         }
      break;

      case Main:
         I16 reading = analogRead(sensor_pin);
         F32 smoothed_reading = ema(reading);

         //convert the analog into millivolt
         F32 phValueMillivolts = (F32) smoothed_reading * 5.0 / 1024; // / 6;

         //convert the millivolt into pH value
         result = 3.5 * phValueMillivolts;

         sleep(measure_interval);
      break;
      }
   }
};

