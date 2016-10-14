//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// (SOLENOID) RELAY CONTROL LOGIC TEST
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -

class SolenoidLogicTest : public ChronoTwinedFsmModule {
   Intd        pin;
   TimeSpan    toggle_interval;
   TimeSpan    start_delay;
   Intd        state;

  public:

   SolenoidLogicTest(
                  Intd     pin_,
                  TimeSpan toggle_interval_,
                  TimeSpan start_delay_ = 0,
                  Intd     initial_state_ = 0
   ) :
         pin(pin_),
         toggle_interval(toggle_interval_),
         start_delay(start_delay_),
         state(initial_state_)
   {
      pinMode(pin, OUTPUT);
      go_after_sleep(Default, start_delay);
   }

   void log() {
      if (state) {
         say("Relay is on");
      } else {
         say("Relay is off");
      }
   }

   bool is_ready() { return true; }

   void update() {
      if (is_sleeping())
         return;

      if (state == 0) {
         state = 1;
         digitalWrite(pin, HIGH);
      } else {
         state = 0;
         digitalWrite(pin, LOW);
      }

      sleep(toggle_interval);
   }

};
