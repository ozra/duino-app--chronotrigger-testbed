//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// DISTANCE SENSOR (NO CONTACT LEVEL CHECKER)
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// Coded for HC-SR04 Ping distance sensor, should work with any Sonar Pinger

class SonarPingSensor : public ChronoTwinedFsmModule {
   Intd        trigger_pin;
   Intd        echo_sense_pin;
   TimeSpan    breathing_room;

  public:
   Reald       result;

   SonarPingSensor(
               Intd     trigger_pin_,
               Intd     echo_sense_pin_,
               TimeSpan breathing_room_ = 35
   ) :
      trigger_pin(trigger_pin_),
      echo_sense_pin(echo_sense_pin_),
      breathing_room(breathing_room_)
   {
      pinMode(trigger_pin, OUTPUT);
      pinMode(echo_sense_pin, INPUT);
   }

   void log() {
      Serial.print(result);
      Serial.print(" cm");
   }

   bool is_ready() { return true; }

   void update() {
      if (is_sleeping())
         return;

      digitalWrite(trigger_pin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigger_pin, HIGH);
   //  delayMicroseconds(1000); - Removed this line
      delayMicroseconds(10); // Added this line
      digitalWrite(trigger_pin, LOW);

      long duration = pulseIn(echo_sense_pin, HIGH);
      result = (duration / 2.0) / 29.1;

      sleep(breathing_room);
   }

};
