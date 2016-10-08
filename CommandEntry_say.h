//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---

// # define check_say_ok_or_return if (global_command_router_singleton.dialogue_in_action() == true) return;
#define check_say_ok_or_return {if (false == true) return;}

void say(const char* str) {
   check_say_ok_or_return;
   Serial.println(str);
}

void say(int val) {
   check_say_ok_or_return;
   Serial.println(val);
}

void say(float val) {
   check_say_ok_or_return;
   Serial.println(val);
}

void say(float val, int decimals) {
   check_say_ok_or_return;
   Serial.println(val, decimals);
}

void say(double val) {
   check_say_ok_or_return;
   Serial.println(val);
}

void say(double val, int decimals) {
   check_say_ok_or_return;
   Serial.println(val, decimals);
}

//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---
//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---
