//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// DHT11 HUMIDITY (AND TEMP) SENSORS
// *TODO* - remove the interrupt-based version
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -

#include "utils_types.h"
#include <Dht11.h>



class HumiditySensor : public ChronoTwinedFsmModule {
   Intd        data_pin;
   Reald       scale_adjust_factor;
   TimeSpan    poll_interval;
   Dht11       dht11;

   Bool     ready = false;

  public:
   Reald    humidity_result = 0;
   Reald    temp_result = 0;

   static const Intd MaybeGetReading = 1;

   bool is_ready() {
      return ready;
   }

   HumiditySensor(
            Intd     data_pin_,
            TimeSpan poll_interval_ = 2000,
            Reald    scale_adjust_factor_ = 1.0
   ) :
      data_pin(data_pin_),
      scale_adjust_factor(scale_adjust_factor_),
      poll_interval(poll_interval_),
      dht11(data_pin)
   {
      go_next(MaybeGetReading);
   }

   void log(void) {
      int ix = 0;
      // for(int ix = 0; ix < actualTempSensorsCount; ++ix) {
         // Serial.print(" RH-T[0]: ");
         // Serial.print(temp_result);
         // Serial.print("C, ");
         Serial.print("  RH[");
         Serial.print(ix);
         Serial.print("]: ");
         Serial.print(humidity_result);
         Serial.print("% ");
      // }
      // Serial.println();
   }

   void update() {
      switch (where_to_go()) {
      // case RequestReading:
      //    // Serial.println("RequestReading");
      //    go_after_sleep(AwaitReading, DHT_CHECK_INTERVAL_SPAN);
      // break;

      case MaybeGetReading:
         switch (dht11.read()) {
         case Dht11::OK:
            humidity_result = dht11.getHumidity() * scale_adjust_factor;
            temp_result = dht11.getTemperature();
            ready = true;
            // Serial.println("DHT11: GOT humidity values");
         break;

         case Dht11::ERROR_CHECKSUM:
            Serial.println("DHT11: ERR: Checksum error");
         break;

         case Dht11::ERROR_TIMEOUT:
            Serial.println("DHT11: ERR: Timeout error");
         break;

         default:
            Serial.println("DHT11: ERR: Unknown error");
         break;
         }

         sleep(poll_interval);
      break;
      }
   }
};
