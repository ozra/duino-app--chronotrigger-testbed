#ifndef CHRONO_TWINED_MODULE
#define CHRONO_TWINED_MODULE

#include "StateBreaker.h"

class ChronoTwinedModule {
  public:
   virtual void update() = 0;
   virtual void log() = 0;
};


class ChronoTwinedFsmModule : public ChronoTwinedModule, public FsmHelper {
};


// Generalized abstract classes for common traits - all units that simply can be
// controlled with 0.0 - 1.0, and where that param is all that is needed for
// consumer use, should inherit this.
template <typename T>
class PercentControl : public ChronoTwinedFsmModule {
  public:
   virtual void set(T value) = 0;
};





// //--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---
// //--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---
// // EXAMPLE
// //--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---
// //--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---

// class MyModule : ChronoTwinedFsmModule {
//    static const int  REQUEST_READING = 1;
//    static const int  CHECK_IF_READING = 2;
//    static const int  DO_READOUT = 3;

//   public:
//    void setup(some_pin, other_pin, etc) {

//    }

//    void log() {
//       say("My my my");
//    }

//    void update() {
//       switch(where_to_go()) {
//       case DEFAULT:
//          go_next(REQUEST_READING);
//       break;
//       case REQUEST_READING:
//          do_some_stuff()
//          go_after_sleep(CHECK_IF_READING);
//       break;
//    }
// }

#endif
