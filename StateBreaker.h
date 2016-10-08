#ifndef STATE_BREAKER
#define STATE_BREAKER

#include <Arduino.h>
#include <Wire.h>
#include "utils.h"

typedef Intd FsmState;

class FsmHelper {
   FsmState    state             = Default;
   FsmState    scheduled_state   = NoWhere;
   TimeSpan    trans_delay       = 0; // We can't just calculate scheduled-time, since we can re-schedule, and don't know the previous schedules duration then.
   TimeStamp   last_ts           = 0;
   Bool        in_sleeping_wait  = false;

  public:
   static const Intd  NoWhere      = 0;
   static const Intd  Default      = -1;
   static const Intd  Main         = -2;
   static const Intd  FsmSleeping  = -9999;

   FsmState where_to_go() {
      if (scheduled_state == NoWhere) {
         return state;

      } else if ((timestamp() - last_ts) > trans_delay) {
         state = scheduled_state;
         cancel_scheduled_state();
         return state;

      } else if (in_sleeping_wait) {
         return FsmSleeping;

      } else {
         return state;
      }
   }

   Bool is_sleeping() {
      return where_to_go() == FsmSleeping; //in_sleeping_wait;
   }

   // Bool has_scheduled() {
   //    if (scheduled_state) {
   //       return true;
   //    } else {
   //       return false;
   //    }
   // }

   void go_next(FsmState state_, Bool keep_scheduled = false) {
      state = state_;
      touch();

      if (keep_scheduled == false)
         cancel_scheduled_state();
   }

   void go_after(FsmState state_, TimeSpan delay, Bool reset_timestamp) {
      in_sleeping_wait = false;
      trans_delay = delay;
      scheduled_state = state_;
      if (reset_timestamp || scheduled_state == NoWhere) {
         touch();
      }
   }

   void go_after_sleep(FsmState state_, TimeSpan delay) {
      go_after(state_, delay, true);
      in_sleeping_wait = true;
   }

   void sleep(TimeSpan delay) {
      go_after_sleep(state, delay);
   }

   void cancel_scheduled_state() {
      scheduled_state = NoWhere;
      trans_delay = 0;
      touch();
   }

  private:
   TimeStamp timestamp() {
      return millis();  // should something else be needed...
   }

   void touch() {
      last_ts = timestamp();
   }

};


#endif