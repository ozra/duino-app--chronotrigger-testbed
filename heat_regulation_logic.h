// *TODO* we should work with _target_value_ and _target_slack_allowance_
// the slack is what decides _how much_ oomph to put in when not _on_ target
// value.
//
// Hmm. This is fucking flawed - self regulating hysteres would be MUCH BETTER.
// Think about if it can be generalized - or if hints in fact are needed for 
// hysteres.
#define VENT_FULL_OPEN_DELTA_TH 5.0


void init_heat_regulation_logic() {
}

void attend_heat_regulation_logic() {
   float cur_delta = max(0, temp_measures[2] - temp_measures[0]);

   // Serial.println("VENT__STATE_MONITOR Acting time - movement");
   float ratio = cur_delta / VENT_FULL_OPEN_DELTA_TH;
   lazy_servo_target = min(1.0, ratio);

}
