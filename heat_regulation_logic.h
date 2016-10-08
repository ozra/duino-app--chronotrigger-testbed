// *TODO* we should work with _target_value_ and _target_slack_allowance_
// the slack is what decides _how much_ oomph to put in when not _on_ target
// value.
//
// Hmm. This is fucking flawed - self regulating hysteres would be MUCH BETTER.
// Think about if it can be generalized - or if hints in fact are needed for
// hysteres.
#define VENT_FULL_OPEN_DELTA_TH 5.0


template <Intd NUM, typename T>
class HeatRegulationLogic : public ChronoTwinedFsmModule {
   TempSensors<NUM>&       temp_sensors;
   HumiditySensor&         humidity_sensor;
   PercentControl<T>&      controllable;
   Reald                   full_action_delta_thresh;

  public:
   HeatRegulationLogic(
                  TempSensors<NUM>& temp_sensors_,
                  HumiditySensor& humidity_sensor_,
                  PercentControl<T>& controllable_,
                  Reald full_action_delta_thresh_ = VENT_FULL_OPEN_DELTA_TH
   ) :
      temp_sensors(temp_sensors_),
      humidity_sensor(humidity_sensor_),
      controllable(controllable_),
      full_action_delta_thresh(full_action_delta_thresh_)
   {}

   void update() {
      switch(where_to_go()) {
      case Default:
         Serial.print("HeatRegulationLogic - waiting for sensors to stabilize\n");
         if (temp_sensors.is_ready() && humidity_sensor.is_ready()) {
            Serial.print("> > > Dependencies source values are ready for VENT CTRL!  > > > TAKING OFF!");
            go_next(Main);
         }
      break;

      case Main:
         Reald cur_delta = max(0.0, temp_sensors.results[2] - temp_sensors.results[0]);
         Reald ratio = cur_delta / full_action_delta_thresh;

         // Serial.print("HeatRegulationLogic: ");
         // Serial.print(cur_delta);
         // Serial.print(", ratio: ");
         // Serial.print(ratio);
         // Serial.println();

         controllable.set(min(1.0, ratio));
      break;
      }
   }

   void log() {}
};
