//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// FAN / BALL-VALVE-SERVO / ETC - VENTILATION / TEMPERATURE CONTROL
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -

#include <Servo.h>

// template <typename T>
class LazyServoControl : public PercentControl<Reald> {
   Intd  servo_pin;
   Reald move_lazyness_thresh;
   Intd  idle_duration_for_sleep;
   Intd  full_rotate_duration;
   Intd  min_rotate_duration;
   Reald min_usecs;
   Reald max_usecs;

   Reald prev_adjusted_value = -1.0;
   Reald target_value = 0.5;  // Center it as initial target_position_micros (value in unit percent)

   Servo servo;   // create servo object to control a servo

  public:
   static const Intd MonitorWhileAwake    = 1;
   static const Intd MonitorWhileAsleep   = 2;
   static const Intd ServoSleep           = 3;
   static const Intd ServoWakeup          = 4;

   LazyServoControl(
               Intd     servo_pin_,
               Reald    move_lazyness_thresh_ = 0.1,
               TimeSpan full_rotate_duration_ = 1500,
               Intd     idle_duration_for_sleep_ = 2000,
               TimeSpan min_rotate_duration_ = 20,
               Intd     min_usecs_ = 544,
               Intd     max_usecs_ = 2400
   ) :
      servo_pin(servo_pin_),
      move_lazyness_thresh(move_lazyness_thresh_),
      full_rotate_duration(full_rotate_duration_),
      idle_duration_for_sleep(idle_duration_for_sleep_),
      min_rotate_duration(min_rotate_duration_)
   {
      set_servo_limits(min_usecs_, max_usecs_),

      go_next(MonitorWhileAsleep);
   }

   void set_servo_limits(Intd min_usecs_, Intd max_usecs_) {
      min_usecs = min_usecs_;
      max_usecs = max_usecs_;
   }

   void set(Reald value_) {
      target_value = value_;
   }

   bool is_ready() { return true; }

   void update() {
      Intd  allowance;

      switch (where_to_go()) {
      case MonitorWhileAsleep:
         // Serial.println("MonitorWhileAsleep");

         if (breach_lazyness_threshold()) {
            go_next(ServoWakeup);
         } else {
            sleep(50);
         }
      break;

      case MonitorWhileAwake:
         // Serial.println("MonitorWhileAwake");
         if (breach_lazyness_threshold()) {
            allowance = adjust_servo(); // go_next(Adjust);
            sleep(allowance);
         } else {
            go_after(ServoSleep, idle_duration_for_sleep, false);
         }
      break;

      case ServoWakeup:
         Serial.println("ServoWakeup");
         servo.attach(servo_pin);

         allowance = adjust_servo();
         go_after_sleep(MonitorWhileAwake, allowance);
      break;

      case ServoSleep:
         Serial.println("ServoSleep");
         servo.detach();

         go_next(MonitorWhileAsleep);
      break;
      }
   }

   void log() {}

  private:
   Intd adjust_servo() {
      Serial.println("Adjust");

      // Serial.print("writes target_position_micros ");
      // Serial.print(target_position_micros);
      // Serial.print("to servo.");
      // Serial.println();

      Intd target_position_micros = (Intd) lround(min_usecs + ((max_usecs - min_usecs) * target_value));

      servo.writeMicroseconds(target_position_micros);

      Reald adjustment_delta = abs(target_value - prev_adjusted_value);

      Intd time_allowance = max(
         min_rotate_duration,
         min(
            full_rotate_duration,
            (adjustment_delta * full_rotate_duration)
         )
      );

      prev_adjusted_value = target_value;

      return time_allowance;
   }

   Bool breach_lazyness_threshold() {
      return (abs(target_value - prev_adjusted_value) > move_lazyness_thresh);
   }
};

