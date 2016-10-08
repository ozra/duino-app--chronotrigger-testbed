#ifndef UTILS_SAY
#define UTILS_SAY

#include <Wire.h>

#include <stdlib.h> // for: dtostrf()


//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// CLANG workaround - for improved compile level error messages
// *TODO* workaround to compile with clang++ for better error messages:
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
#ifdef NICE_ERRORS
   #include <HardwareSerial.h>
   extern HardwareSerial Serial;
#endif


//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// `say` for outputting stuff on serial in an informal way (debug/log)
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
#ifdef DEBUG
   #define say(a...)    { check_fatal_msgs(); Serial.print(a); }
#else
   #define say(a...)    {}
#endif


//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
// Fatal errors debugging
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
#define FATAL_MSG_SIZE 250
Char  fatal_messages[FATAL_MSG_SIZE];
Intd  fatal_msg_ix = 0;
Bool  serial_activated = false;

void check_fatal_msgs() {
   if (fatal_msg_ix == 0)
      return;

   Serial.print("\e[31;1m");
   Serial.print("\n\n\n\n\n");
   Serial.print("! ! ! ! ! ! ! !\n");
   Serial.print(fatal_messages);
   Serial.print("! ! ! ! ! ! ! !\n");
   Serial.print("\n\n\n\n\n");
   Serial.print("\e[34m");

   delay(10000);
   fatal_msg_ix = 0;
}

// Intd strsize(

void add_fatal_message(const char* msg) {
   // this is for foolproofing - not performance!
   if (fatal_msg_ix == 0) {
      memcpy(fatal_messages, "\e[31;1m", 7);
      fatal_msg_ix += 7;
   }

   if (fatal_msg_ix + strlen(msg) >= FATAL_MSG_SIZE) {
      // Keep overwriting last msg (first and last are usualy most important)
      fatal_msg_ix = FATAL_MSG_SIZE - 80;
      fatal_messages[fatal_msg_ix - 1] = 10; // Newline
   }

   memcpy(fatal_messages+fatal_msg_ix, msg, strlen(msg));
   fatal_msg_ix += strlen(msg);

   // Ends the output. If more is printed, it will simply overwrite this.
   fatal_messages[fatal_msg_ix] = 0;
}

void fatal(const char* msg) {
   if (serial_activated) {
      say("\e[31;1m");
      say(msg);
   } else {
      add_fatal_message(msg);
   }
}

void fatal(Reald num) {
   char buf[7];

   if (serial_activated) {
      say(num);
   } else {
      dtostrf(num, 3, 2, buf);
      add_fatal_message(buf);
   }
}

#endif