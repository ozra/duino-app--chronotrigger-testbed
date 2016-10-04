#include <Arduino.h>
#include <Wire.h>







//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// For quickly disabling running of circuit without pulling cables etc.:
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
#define COMPLETE_HALT 0
// #define COMPLETE_HALT 1
#define DEBUG 1

//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// PIN MAPPINGS
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
#define ONE_WIRE_BUS_PIN         2

#define DHT_DATA_PIN             3  // Used interrupt 0 for a while - thus pin 3

#define LAZYSERVO_SERVO_PIN      4

#define SONAR_TRIGGER_PIN        8
#define SONAR_ECHO_SENSING_PIN   9

#define PH_SENSOR_PIN            12

#define DEBUG_LOG_INTERVAL_SPAN 3000
#define DEBUG_LOG__STATE_REPORT 1  // Might as well go with simply "DEFAULT" - we're just using the defer-functionality in practise

//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// FILE DEPS
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -

//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// - UTIL MODULES -

// Finite State Machine
#define StateBreaker__DEPS_POLLING_INTERVAL_SPAN 1
#include "StateBreaker.h"

//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// - SENSOR MODULES -

// Sonar
#define sonar_sensors__BREATHING_ROOM_INTERVAL_SPAN 35
#include "sonar_sensors.h"

// Humidity
#define DHT_RH_SCALE_FACTOR 2.0

   #include <Dht11.h>
#include "humidity_sensors.h"

// pH
#include "ph_sensor.h"

// Temperature
// Setup a temperature_sensors__OnewireBus instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
#include <OneWire.h>
OneWire oneWireBus1(ONE_WIRE_BUS_PIN);
#define temperature_sensors__OnewireBus oneWireBus1

   #include <DallasTemperature.h>
#include "temperature_sensors.h"

//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// - CONTROL MODULES -

// Lazy (sleeping) Servos
   #include <Servo.h>
#include "lazy_servo_control.h"

//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// - LOGIC MODULES -

// Heat Regulation Logic
#include "heat_regulation_logic.h"

//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -


//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// MAIN INIT AND LOOP
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -

void setup(void) {
   Serial.begin(9600);

   if (COMPLETE_HALT) return;

   init_temp_sensors();
   init_humidity_sensors();
   init_ph_sensor();
   init_sonar_ping_sensors();

   init_heat_regulation_logic();

   init_lazy_servo_control();
}

void loop(void) {
   if (COMPLETE_HALT) return;

   attend_temp_sensors();
   attend_humidity_sensors();
   attend_ph_sensor();
   attend_sonar_ping_sensors();

   attend_heat_regulation_logic();

   attend_lazy_servo_control();

   #ifdef DEBUG
      debug_logging();
   #endif
}

void debug_logging(void) {
   static Fsm fsm = {DEBUG_LOG__STATE_REPORT, 0, 0, __STATE_NONE};

   if (check_state_deferring(&fsm)) return;

   log_temp_sensors();
   Serial.print(" - ");

   log_humidity_sensors();
   Serial.println();

   log_sonar_ping_sensors();
   Serial.print(" - ");

   log_ph_sensor();
   Serial.println();

   defer_state(&fsm, DEBUG_LOG__STATE_REPORT, DEBUG_LOG_INTERVAL_SPAN);
}
