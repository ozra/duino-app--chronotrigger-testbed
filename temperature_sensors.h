//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// ONEWIRE DALLAS TEMPERATURE SENSORS
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -

#include <OneWire.h>
#include <DallasTemperature.h>

void debug_print_temp_sensor_addr(int sensorIx, uint8_t *addrId);

#define TEMPING_MAX_SENSORS            10
#define TEMPING_FULL_12BIT_WAIT_SPAN   750
#define TEMPING_INITIAL_WAIT_SPAN      750
#define TEMPING_CHECK_INTERVAL_SPAN    300

//| Smoothing Strength in (1.0 / lookback) unit
//| Is set to a "lookback"(*) of 2 by default - that is: (1.0 / 2.0) = 0.5
//| If your sensors are erratic and you can tolerate slower update of the value,
//| you can "increase the lookback", that is: _lower the factor_.
//|
//| (*) The steps refer to the similarity with an SMA and thereby a window size.
//|
#define TEMPING_FILTER_SIGNAL_EMA_K    0.5

#define TEMPING__STATE_REQUEST         1
#define TEMPING__STATE_INITIAL_WAIT    2
#define TEMPING__STATE_AWAIT_RESULTS   3

// Pass our temperature_sensors__OnewireBus reference to Dallas Temperature.
bool temps_is_available_ = false;

DallasTemperature tempSensorsInterface(&temperature_sensors__OnewireBus);
typedef uint8_t OneWireDeviceAddress[8];

OneWireDeviceAddress tempSensorAddrIds[TEMPING_MAX_SENSORS];
float temp_measures[TEMPING_MAX_SENSORS];
bool tempGotten[TEMPING_MAX_SENSORS];
int actualTempSensorsCount;

void init_temp_sensors(void) {
   Serial.println("Initializing Dallas Temperature IC Control");

   // Start up the library
   tempSensorsInterface.begin();
   tempSensorsInterface.setWaitForConversion(false);

   Serial.print("isParasitePowerMode: ");
   Serial.print(tempSensorsInterface.isParasitePowerMode());
   Serial.println();

   int sensorIx = 0;
   uint8_t *currentSensorAddrId;

   while (true) {
      currentSensorAddrId = tempSensorAddrIds[sensorIx];

      if (temperature_sensors__OnewireBus.search(currentSensorAddrId) == false)
         break;

      debug_print_temp_sensor_addr(sensorIx, currentSensorAddrId);

      tempGotten[sensorIx] = false;
      ++sensorIx;
   }

   actualTempSensorsCount = sensorIx;

}

void debug_print_temp_sensor_addr(int sensorIx, uint8_t *addrId) {
   Serial.print("Temp sensor ");
   Serial.print(sensorIx);
   Serial.print(": ");

   for (int byte_ix = 0; byte_ix < 8; ++byte_ix) {
      if (addrId[byte_ix] < 10)
         Serial.print("00");
      else if (addrId[byte_ix] < 100)
         Serial.print("0");

      Serial.print(addrId[byte_ix]);
   }

   Serial.println();
}

void log_temp_sensors(void) {
   for(int ix = 0; ix < actualTempSensorsCount; ++ix) {
      Serial.print("  T[");
      Serial.print(ix);
      Serial.print("]: ");
      Serial.print(temp_measures[ix]);
      Serial.print("C, ");
   }
}

void attend_temp_sensors(void) {
   static Fsm fsm = {TEMPING__STATE_REQUEST, 0, 0, __STATE_NONE};
   static int tempSensorsResultCount;
   uint8_t *addrId;
   float temp_in_celcius = 0.0;
   float prev_value = 0.0;
   float freshest_reading = FAKE_INT_NIL;

   switch (fsm.state) {
   case TEMPING__STATE_REQUEST:
      // Serial.println("TEMPING__STATE_REQUEST");

      // call tempSensorsInterface.requestTemperatures() to issue a global temperature
      // request to all devices on the bus
      tempSensorsInterface.requestTemperatures(); // Send the command to get temperatures
      tempSensorsResultCount = 0;
      for(int ix = 0; ix < actualTempSensorsCount; ++ix) {
         tempGotten[ix] = false;
      }
      set_state(&fsm, TEMPING__STATE_INITIAL_WAIT);
      break;

   case TEMPING__STATE_INITIAL_WAIT:
      if (check_state_deferring(&fsm)) return;
      // Serial.println("TEMPING__STATE_INITIAL_WAIT");

      defer_state(&fsm, TEMPING__STATE_AWAIT_RESULTS, TEMPING_INITIAL_WAIT_SPAN);
      break;

   case TEMPING__STATE_AWAIT_RESULTS:
      if (check_state_deferring(&fsm)) return;

      // Serial.println("TEMPING__STATE_AWAIT_RESULTS");

      for(int ix = 0; ix < actualTempSensorsCount; ++ix) {
         if (tempGotten[ix])
            continue;

         addrId = tempSensorAddrIds[ix];
         if (tempSensorsInterface.isConversionAvailable(addrId)) {
            // Serial.print("isConversionComplete?: ");
            // Serial.print(tempSensorsInterface.isConversionComplete());
            //Serial.println();

            tempGotten[ix] = true;
            tempSensorsResultCount++;

            prev_value = temp_measures[ix];
            freshest_reading = tempSensorsInterface.getTempC(addrId);

            // First reading ever - could have its own state for it instead.
            if (prev_value == FAKE_INT_NIL) {
               temp_in_celcius = freshest_reading;
            } else {
               temp_in_celcius = (
                  (freshest_reading * TEMPING_FILTER_SIGNAL_EMA_K) +
                  (prev_value * (1 - TEMPING_FILTER_SIGNAL_EMA_K))
               );
            }

            temp_measures[ix] = temp_in_celcius;

            if (ix == actualTempSensorsCount - 1) {
               Serial.print("Got ");
               Serial.print(temp_measures[ix]);
               Serial.print(" for ");
               Serial.print(ix);
               Serial.print(" from freshest reading ");
               Serial.print(freshest_reading);
               Serial.print(" - which is err of ");
               Serial.print(freshest_reading - temp_measures[ix]);
               Serial.println();
            }
         }
      }

      if (tempSensorsResultCount == actualTempSensorsCount) {
         temps_is_available_ = true;
         // Serial.println("TEMPING - GOT ALL RESULTS!");
         set_state(&fsm, TEMPING__STATE_REQUEST);
      } else {
         // Serial.println("Wait some more...");
         // Wait some more
         defer_state(&fsm, TEMPING__STATE_AWAIT_RESULTS, TEMPING_CHECK_INTERVAL_SPAN);
      }
      break;
   }
}
