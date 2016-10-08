//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// ONEWIRE DALLAS TEMPERATURE SENSORS
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -

#include <OneWire.h>
#include <DallasTemperature.h>
#include "SignalProcessingEma.h"
#include "utils.h"
#include "ChronoTwinedModule.h"


#define TEMPING_FULL_12BIT_WAIT_SPAN   750

typedef uint8_t   OneWireDeviceAddr[8];
typedef uint8_t*  OneWireDeviceAddrPtr;


template <Intd ALLOC_SENSORS>
class TempSensors : public ChronoTwinedFsmModule {
   OneWire&                   onewire_bus;
   DallasTemperature          temp_sensors_interface;
   SignalProcessing::Ema<F32> ema = SignalProcessing::Ema<F32>(); // *TODO* use outlier filtering over a window size and % of sorted middle taken for avg'ing

   TimeSpan          initial_wait;
   TimeSpan          check_interval;

   Bool              ready = false;
   Intd              actual_sensor_count;

   OneWireDeviceAddr temp_sensor_addr_ids[ALLOC_SENSORS];
   Bool              temp_gotten[ALLOC_SENSORS];

   Intd              gotten_results_count;

  public:
   Reald             results[ALLOC_SENSORS];

   static const Intd RequestReadings = 1;
   static const Intd AwaitReadings   = 2;

   TempSensors(
      OneWire& onewire_bus_,
      Reald smoothing_k_value_,
      TimeSpan initial_wait_ = TEMPING_FULL_12BIT_WAIT_SPAN,
      TimeSpan check_interval_ = 300
   ) :
      onewire_bus(onewire_bus_),
      temp_sensors_interface(&onewire_bus),
      initial_wait(initial_wait_),
      check_interval(check_interval_)
   {
      ema.change_k(smoothing_k_value_);




      // Move anything not a simple list-init back to `setup()` again!


      // say("TempSensors: Dallas Temperature IC Control");

      // Start up the library
      temp_sensors_interface.begin();
      temp_sensors_interface.setWaitForConversion(false);

      // say("isParasitePowerMode: ");
      // say(temp_sensors_interface.isParasitePowerMode());
      // say("\n");

      Intd  sensor_ix = 0;
      U8*   tmp_addr_id;

      while (true) {
         tmp_addr_id = temp_sensor_addr_ids[sensor_ix];

         if (onewire_bus.search(tmp_addr_id) == false)
            break;

         // *TODO* check that it actually is a temp-sensor device!

         // *TODO* check that ALLOC_SENSORS isn't surpassed - if so alert()!

         // log_addr_id(sensor_ix, tmp_addr_id);

         temp_gotten[sensor_ix] = false;
         ++sensor_ix;
      }

      actual_sensor_count = sensor_ix;

      go_next(RequestReadings);
   }

   bool is_ready() {
      return ready;
   }

   void log_addr_id(const OneWireDeviceAddrPtr tmp_addr_id) {
      for (Intd byte_ix = 0; byte_ix < 8; ++byte_ix) {
         say_hex(tmp_addr_id[byte_ix]);
      }
   }

   void log() {
      for(Intd ix = 0; ix < actual_sensor_count; ++ix) {
         say("  T[");
         say(ix);
         say("](");
         log_addr_id(temp_sensor_addr_ids[ix]);
         say("): ");
         say(results[ix]);
         say("C, ");
      }
   }

   void update() {
      OneWireDeviceAddrPtr tmp_addr_id;
      Reald                freshest_reading;

      switch (where_to_go()) {
      case RequestReadings:
         // say("TempSensors:RequestReadings\n");

         // call temp_sensors_interface.requestTemperatures() to issue a global temperature
         // request to all devices on the bus
         temp_sensors_interface.requestTemperatures(); // Send the command to get temperatures
         gotten_results_count = 0;
         for(Intd ix = 0; ix < actual_sensor_count; ++ix) {
            temp_gotten[ix] = false;
         }

         go_after_sleep(AwaitReadings, initial_wait);
      break;

      case AwaitReadings:
         // say("TEMPING__STATE_AWAIT_RESULTS\n");

         for(Intd ix = 0; ix < actual_sensor_count; ++ix) {
            if (temp_gotten[ix])
               continue;

            tmp_addr_id = temp_sensor_addr_ids[ix];
            if (temp_sensors_interface.isConversionAvailable(tmp_addr_id)) {
               // say("isConversionComplete?: ");
               // say(temp_sensors_interface.isConversionComplete());
               //say("\n");

               temp_gotten[ix] = true;
               gotten_results_count++;
               freshest_reading = temp_sensors_interface.getTempC(tmp_addr_id);
               results[ix] = ema(freshest_reading);

               // if (ix == actual_sensor_count - 1) {
               //    say("Got ");
               //    say(results[ix]);
               //    say(" for ");
               //    say(ix);
               //    say(" from freshest reading ");
               //    say(freshest_reading);
               //    say(" - which is err of ");
               //    say(freshest_reading - results[ix]);
               //    say("\n");
               // }
            }
         }

         if (gotten_results_count == actual_sensor_count) {
            ready = true;
            // say("TEMPING - GOT ALL RESULTS!\n");
            go_next(RequestReadings);
         } else {
            // say("Wait some more...\n");
            // Wait some more
            go_after_sleep(AwaitReadings, check_interval);
         }
      break;
      }
   }


};
