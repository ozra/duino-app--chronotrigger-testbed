//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// FAN / BALL-VALVE-SERVO / ETC - VENTILATION / TEMPERATURE CONTROL
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -

#include <Servo.h>

#define LAZYSERVO_MOVEMENT_LAZYNESS_TH         0.1
#define LAZYSERVO_SERVO_FULL_SETTLING_SPAN     1500
#define LAZYSERVO_MINIMUM_MOVE_ALLOWANCE_SPAN  300

#define LAZYSERVO__STATE_MONITOR      1
#define LAZYSERVO__STATE_ADJUST       2
#define LAZYSERVO__STATE_ADJUSTING    3
#define LAZYSERVO__STATE_SLEEP        4
#define LAZYSERVO__STATE_WAKEUP       5

#define LAZYSERVO__STATE_MONITOR_SUB__IS_AWAKE 101
#define LAZYSERVO__STATE_MONITOR_SUB__IS_ASLEEP 102

Servo lazy_servo;   // create servo object to control a servo

float lazy_servo_target = 0.5;  // Center it as initial target_position_micros (value in unit percent)

void init_lazy_servo_control(void) {
}

void attend_lazy_servo_control(void) {
   static Fsm fsm = {__STATE_AWAIT_DEPS, 0, 0, __STATE_NONE};
   static int monitor_sub_state = LAZYSERVO__STATE_MONITOR_SUB__IS_ASLEEP;

   static int     prev_servo_target = -1.0;
   static int     target_position_micros;

   int   movement_delta;
   int   time_allowance;
   bool  trigger_change;
   // int   check_ret;

   switch (fsm.state) {
   case __STATE_AWAIT_DEPS:  // *TODO* could be generalized to simply "init" stage!
      Serial.println("__STATE_AWAIT_DEPS");
      if (temps_is_available_ && humidity_is_available_) {
         Serial.print("Dependencies source values are ready for VENT CTRL!  > > > TAKING OFF!");

         set_state(&fsm, LAZYSERVO__STATE_MONITOR);
      // } else {
      //    Serial.print("Still awaiting dependencies for Vent Ctrl!");
      //    defer_state(&fsm, __STATE_AWAIT_DEPS, StateBreaker__DEPS_POLLING_INTERVAL_SPAN);
      // JUST KEEP STATE AND KEEP ROTATING - SIMPLY!
      }
      break;

   case LAZYSERVO__STATE_MONITOR:
      // Serial.println("LAZYSERVO__STATE_MONITOR");
      target_position_micros = (int) lround(544.0 + (2400.0 - 600.0) * lazy_servo_target);

      trigger_change = false;

      if (abs(lazy_servo_target - prev_servo_target) > LAZYSERVO_MOVEMENT_LAZYNESS_TH) {
         trigger_change = true;
         // Serial.print(", ratio: "); Serial.print(ratio);
         // Serial.print(", clipped ratio: "); Serial.print(clipped_ratio);
         // Serial.print(", fpos: "); Serial.print((180.0 * min(1.0, cur_delta / LAZYSERVO_FULL_OPEN_DELTA_TH)));
         // Serial.print(", ");
         Serial.print("int target_position_micros:                     "); Serial.println(target_position_micros);
      }
      // } else {
         // Serial.println("LAZYSERVO__STATE_MONITOR  - No action yet...");
      // }
      // Serial.println("LAZYSERVO__STATE_MONITOR");
      if (trigger_change) {
         // Serial.println("LAZYSERVO__STATE_MONITOR   - TRIGGER change!");
         if (monitor_sub_state == LAZYSERVO__STATE_MONITOR_SUB__IS_ASLEEP) {
            set_state(&fsm, LAZYSERVO__STATE_WAKEUP);
         } else {
            set_state(&fsm, LAZYSERVO__STATE_ADJUST);
         }
      } else {
         if (check_state_deferring(&fsm) == 0) {
            // Serial.println("LAZYSERVO__STATE_MONITOR - no transition - update trans-target to monitor _or_ sleep!");
            if (monitor_sub_state == LAZYSERVO__STATE_MONITOR_SUB__IS_ASLEEP) {
               set_state(&fsm, LAZYSERVO__STATE_ADJUSTING);
               defer_state(&fsm, LAZYSERVO__STATE_MONITOR, 300, true); // *TODO* time_allowance);
            } else {
               defer_state(&fsm, LAZYSERVO__STATE_SLEEP, 2500, true); // *TODO* time_allowance);
            }
         }
      }

      break;

   case LAZYSERVO__STATE_WAKEUP:
      Serial.println("LAZYSERVO__STATE_WAKEUP");
      lazy_servo.attach(LAZYSERVO_SERVO_PIN);
      monitor_sub_state = LAZYSERVO__STATE_MONITOR_SUB__IS_AWAKE;
      set_state(&fsm, LAZYSERVO__STATE_ADJUST);
      break;

   case LAZYSERVO__STATE_ADJUST:
      Serial.println("LAZYSERVO__STATE_ADJUST");

      // Serial.print("writes target_position_micros ");
      // Serial.print(target_position_micros);
      // Serial.print("to servo.");
      // Serial.println();

      lazy_servo.writeMicroseconds(target_position_micros);

      movement_delta = abs(lazy_servo_target - prev_servo_target);
      // time_allowance = (movement_delta * LAZYSERVO_SERVO_FULL_SETTLING_SPAN) / 180 + 5;
      time_allowance = max(
         LAZYSERVO_MINIMUM_MOVE_ALLOWANCE_SPAN,
         min(
            LAZYSERVO_SERVO_FULL_SETTLING_SPAN,
            (movement_delta * LAZYSERVO_SERVO_FULL_SETTLING_SPAN) / 2400
         )
      );

      prev_servo_target = lazy_servo_target;

      Serial.print("- - - time_allowance: ");
      Serial.println(time_allowance);

      set_state(&fsm, LAZYSERVO__STATE_ADJUSTING);
      defer_state(&fsm, LAZYSERVO__STATE_MONITOR, time_allowance);
      break;

   case LAZYSERVO__STATE_ADJUSTING:
      // Serial.println("LAZYSERVO__STATE_ADJUSTING");

      // check_ret = check_state_deferring(&fsm);
      check_state_deferring(&fsm);
      // Serial.print("check_state_deferring(&fsm) = ");
      // Serial.println(check_ret);

      // if (false) { // trigger_change) {
      //    Serial.println("trigger_change!!");
      //    set_state(&fsm, LAZYSERVO__STATE_ADJUST);
      //    defer_state(&fsm, LAZYSERVO__STATE_SLEEP, LAZYSERVO_SERVO_FULL_SETTLING_SPAN);
      // } else {
         // if (check_ret == 2) {
         //    // Happens at the exakt event of state transition (= 2)
         //    Serial.println("LAZYSERVO__STATE_ADJUSTING - just when transitioning to sleep");
         // }
      // }

      // IF triggering changes occur in "adjusting", we should _keep_ the timeout
      // (or repeat - NO - keep the remained: which means we need
      // `retarget_defer()` - so that on "timeout" LAZYSERVO__STATE_ADJUST is next
      // target (again) after us - instead of sleep. So it's still given the
      // "rest-period" needed to reach a target and not starting to hatter.

      break;

   case LAZYSERVO__STATE_SLEEP:
      Serial.println("LAZYSERVO__STATE_SLEEP");
      lazy_servo.detach();
      monitor_sub_state = LAZYSERVO__STATE_MONITOR_SUB__IS_ASLEEP;
      set_state(&fsm, LAZYSERVO__STATE_MONITOR);
      break;
   }
}


