#ifndef SIGNAL_PROCESSING_EMA
#define SIGNAL_PROCESSING_EMA

#include "SignalProcessingMath.h"

namespace SignalProcessing {

template <typename T>
class Ema {
   T     outer_type = 0;
   F32   prev_value = FAKE_INT_NIL;
   F32   k_value;

  public:
   Ema(F32 k_value_ = 0.5) {
      change_k(k_value_);
   }

   void change_k(F32 k_value_) {
      if (k_value_ > 1.0) {
         fatal("SignalProcessing::Ema:change_k - ABOVE 1.0! (");
         fatal(k_value_);
         fatal("). Have you messed up the arguments order?\n");

         k_value = 1.0;

      } else {
         k_value = k_value_;
      }
   }

   F32 operator()(T outer_reading) {
      if (prev_value == FAKE_INT_NIL) // On the first reading ever
         prev_value = (F32) outer_reading;

      F32 new_val = (
         (((F32)outer_reading) * k_value) +
         (prev_value * (1 - k_value))
      );

      prev_value = new_val;
      // return (T) new_val;
      return new_val;
   }
};


}

#endif
