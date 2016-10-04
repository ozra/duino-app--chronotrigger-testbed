#include <Arduino.h>
#include <Wire.h>


//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---
// *TODO* move this to "signal_processing"
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---
// *TODO* Lib-stuff!
// *TODO* join forces with `SignalFilterEma`
#define FAKE_INT_NIL    -31760

template <typename T>
class SignalFilterEma {
   T     outer_type = 0;
   float prev_value = FAKE_INT_NIL;
   float k_value;

  public:
   SignalFilterEma(float k_value_) {
      k_value = k_value_;
   }

   float operator()(T outer_reading) {
      if (prev_value == FAKE_INT_NIL) // On the first reading ever
         prev_value = (float) outer_reading;

      float new_val = (
         (((float)outer_reading) * k_value) +
         (prev_value * (1 - k_value))
      );

      prev_value = new_val;
      // return (T) new_val;
      return new_val;
   }
};
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---






#define __STATE_NONE 0
#define __STATE_AWAIT_DEPS -1
#define __STATE_DEFAULT -2

#ifndef StateBreaker__DEPS_POLLING_INTERVAL_SPAN
    #define StateBreaker__DEPS_POLLING_INTERVAL_SPAN 0
#endif


typedef unsigned long TimeStamp;

struct Fsm {
  int           state;
  unsigned int  trans_delay;
  TimeStamp     trans_ts;
  int           scheduled_state;
};

void set_state(Fsm *fsm, int state) {
    fsm->state = state;
    fsm->scheduled_state = __STATE_NONE;
}

// *TODO* maybe keeping trans_ts should be standard when scheduled_state is
// already set - thus meaning we're chaning/re-directing from an already planned
// target. If we're setting from scratch (no already plannet) then it timestamps.
// And if override is wished - THEN we flag "force_reset_ts" instead.
void defer_state(Fsm *fsm, int state, int defer, bool leave_ts_untouched = false) {
    if (leave_ts_untouched == false) {
        fsm->trans_ts = millis();
    }
    fsm->trans_delay = defer;
    // fsm->state = ST_ALL_DEFERRED_TRANSISTION;
    fsm->scheduled_state = state;
}

int check_state_deferring(Fsm *fsm) {
    // if (fsm->state == ST_ALL_DEFERRED_TRANSISTION) {
    if (fsm->scheduled_state == __STATE_NONE)
        return 0;

    if ((millis() - fsm->trans_ts) < fsm->trans_delay)
        return 1;

    fsm->state = fsm->scheduled_state;
    fsm->scheduled_state = __STATE_NONE;
    return 2;
}
