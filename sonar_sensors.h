//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// DISTANCE SENSOR (NO CONTACT LEVEL CHECKER)
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// Coded for HC-SR04 Ping distance sensor, should work with any Sonar Pinger

#ifndef sonar_sensors__BREATHING_ROOM_INTERVAL_SPAN
   #define sonar_sensors__BREATHING_ROOM_INTERVAL_SPAN 35 // 29ms is about minimum to not cause cross-sensor ping issues
#endif

float sonar_ping_result;

void init_sonar_ping_sensors() {
   pinMode(SONAR_TRIGGER_PIN, OUTPUT);
   pinMode(SONAR_ECHO_SENSING_PIN, INPUT);
}

void log_sonar_ping_sensors() {
   Serial.print(sonar_ping_result);
   Serial.print(" cm");
}

void attend_sonar_ping_sensors() {
   static Fsm fsm = {__STATE_NONE, 0, 0, __STATE_NONE};
   long duration;

   if (check_state_deferring(&fsm)) return;

   digitalWrite(SONAR_TRIGGER_PIN, LOW);
   delayMicroseconds(2);
   digitalWrite(SONAR_TRIGGER_PIN, HIGH);
//  delayMicroseconds(1000); - Removed this line
   delayMicroseconds(10); // Added this line
   digitalWrite(SONAR_TRIGGER_PIN, LOW);
   duration = pulseIn(SONAR_ECHO_SENSING_PIN, HIGH);
   sonar_ping_result = (duration / 2.0) / 29.1;
   // delay(500);

   defer_state(&fsm, __STATE_NONE, sonar_sensors__BREATHING_ROOM_INTERVAL_SPAN);
}
