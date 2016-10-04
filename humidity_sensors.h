//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// DHT11 HUMIDITY (AND TEMP) SENSORS
// *TODO* - remove the interrupt-based version
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -

#include <Dht11.h>

#ifndef DHT_RH_SCALE_FACTOR
   #define DHT_RH_SCALE_FACTOR   1.0
#endif

#define DHT_POLL_INTERVAL_SPAN   2000
#define DHT_CHECK_INTERVAL_SPAN  1

#define DHT__STATE_AQUIRE  1
#define DHT__STATE_AWAIT   2

Dht11 dht11(DHT_DATA_PIN);
bool  humidity_is_available_ = false;
float dht_humidity = 0;
float dht_temp = 0;


void init_humidity_sensors(void) {
}

void log_humidity_sensors(void) {
   int ix = 0;
   // for(int ix = 0; ix < actualTempSensorsCount; ++ix) {
      // Serial.print(" RH-T[0]: ");
      // Serial.print(dht_temp);
      // Serial.print("C, ");
      Serial.print("  RH[");
      Serial.print(ix);
      Serial.print("]: ");
      Serial.print(dht_humidity);
      Serial.print("% ");
   // }
   // Serial.println();
}

void attend_humidity_sensors(void) {
   static Fsm fsm = {DHT__STATE_AQUIRE, 0, 0, __STATE_NONE};

   switch (fsm.state) {
   case DHT__STATE_AQUIRE:
      if (check_state_deferring(&fsm)) return;
      // Serial.println("DHT__STATE_AQUIRE");

      defer_state(&fsm, DHT__STATE_AWAIT, DHT_CHECK_INTERVAL_SPAN);

      break;

   case DHT__STATE_AWAIT:
      if (check_state_deferring(&fsm)) return;
      // Serial.println("DHT__STATE_AWAIT");

      switch (dht11.read()) {
      case Dht11::OK:
         dht_humidity = dht11.getHumidity() * DHT_RH_SCALE_FACTOR;
         dht_temp = dht11.getTemperature();
         humidity_is_available_ = true;
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

      defer_state(&fsm, DHT__STATE_AQUIRE, DHT_POLL_INTERVAL_SPAN);
   }
}

