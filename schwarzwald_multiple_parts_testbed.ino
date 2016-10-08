//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// For quickly disabling running of circuit without pulling cables etc.:
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
#define COMPLETE_HALT 0
// #define COMPLETE_HALT 1

#define DEBUG 1
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -



//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
#include <Arduino.h>
#include <Wire.h>
#include "utils.h"


// class PreInitListsSetup {
//   public:
//    PreInitListsSetup() {
//       Serial.begin(9600);
//       sayln("Serial begun");
//       delay(2000);
//       sayln("And so on...");
//    }
// };

// auto pre_init_lists_setup  = PreInitListsSetup();


#include "StateBreaker.h"

//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -

#include <OneWire.h>

   #include <DallasTemperature.h>
#include "temperature_sensors.h"

#include "ph_sensor.h"

   #include <Dht11.h>
#include "humidity_sensors.h"

#include "sonar_sensor.h"

   #include <Servo.h>
#include "lazy_servo_control.h"

#include "heat_regulation_logic.h"

//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// PIN MAPPINGS
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
#define DEBUG_LOG_INTERVAL_SPAN 3000
#define DEBUG_LOG__STATE_REPORT 1  // Might as well go with simply "DEFAULT" - we're just using the defer-functionality in practise

auto onewire_bus_1         = OneWire(2);
auto temp_sensors          = TempSensors<10>(onewire_bus_1, 0.5, 750, 300);
auto humidity_sensor       = HumiditySensor(3,  2000, 2.0);
auto ph_sensor             = PhOne(12,  100, 20.0);
auto sonar_sensor          = SonarPingSensor(8, 9,  35);
auto heat_controller       = LazyServoControl(4,  0.03, 1500);
auto heat_regulation_logic = HeatRegulationLogic<10, Reald>(temp_sensors, humidity_sensor, heat_controller, 5.0);

//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// MAIN INIT AND LOOP
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
void log_update();


void setup() {
   if (COMPLETE_HALT) return;

   Serial.begin(9600);
   serial_activated = true;

   say("> > > Serial Kicked Off < < <\n");
}

void loop() {
   if (COMPLETE_HALT) return;

   // Sensors
   temp_sensors.update();
   humidity_sensor.update();
   ph_sensor.update();
   sonar_sensor.update();

   // Logic
   heat_regulation_logic.update();

   // Controllers
   heat_controller.update();

   #ifdef DEBUG
      log_update();
   #endif
}

void log_update() {
   static FsmHelper fsm;

   if (fsm.is_sleeping())
      return;

   temp_sensors.log();
   say("\n");

   humidity_sensor.log();
   say(" - ");

   sonar_sensor.log();
   say(" - ");

   ph_sensor.log();

   say("\n");

   fsm.sleep(DEBUG_LOG_INTERVAL_SPAN);
}
