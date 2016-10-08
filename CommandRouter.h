

#include "utils_types.h"

// *TODO*
// Continually watch serial - WHEN ACTIVATED
    // When "use @mod_name" is found
    // set that to active
    // "@mod_name" is available for one-shot

    // "use #addr-or-ix"
    // "#addr-or-ix" for addressing specific module-instance

    // "command!" the actual command

    // one or more params, as confed by depending mod (registers handlers for
    // commands)

// Example:
//
// use @ph
// use #0
// ph4!
// ph10!
// done!
//
// @ph#0 ph4!
// @ph#0 ph10!
// @ph#0 done!
//
// @heat_logic set-delta-thresh! 10.0
//
// @heat_logic set-temp-range! 29.5, 30.5


//--- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ---

enum CommandArgsEnum {
   NONE = 0;
   ONE_INT = 1;
   ONE_FLOAT = 2;
   TWO_INTS = 3;
   TWO_FLOATS = 4;
};

class CommandEntry {
   char*             name;
   CommandArgsEnum   args_type;

  public:
   CommandEntry(char* name_, CommandArgsEnum args_type_ = CommandArgsEnum::NONE) {
      name = name_;
      args_type = args_type_;
   }
};

class CommandSubscriber {
   IntertwinedFsm       subscriber = null;
   char*                name;
   Bool                 uses_instance_addr = true;
   CommandEntry         commands[10];  // *TODO* *HARD*

  public:
   CommandSubscriber(IntertwinedFsm sub, char* mod_name, Bool singleton) {
      subscriber = sub;
      name = mod_name;
      uses_instance_addr = !singleton;
   }

   void add_command(char* command_name, CommandArgsEnum args_type) {
      auto entry = new CommandEntry(command_name, args_type);
      commands << entry;
   }
};

class CommandRouter : IntertwinedFsm {
   CommandSubscriber    subscribers[20];  // *TODO* *HARD*
   Bool                 enabled = false;
   Bool                 dialogue_in_action = false;

  public:
   CommandRouter() {
      configure();
   }

   void enable() {
      enabled = true;
   }

   void disable() {
      enabled = false;
   }

   void configure() {
   }

   CommandSubscriber subscribe(IntertwinedFsm sub, char* mod_name, Bool singleton) {
      auto new_sub = new CommandSubscriber(sub, mod_name, singleton);
      subscribers << new_sub;
      return new_sub;
   }

   void update() {
      switch (fsm_state()) {
      case SETUP:

      break;
      case ENABLED:

      break;
      case DISABLED:
         // just check for flag to be enabled again

      break;
      case DIALOGUE:


      break;
      }

      if(check_serial_data()) {
         byte modeIndex = uartParse();

         phCalibration(modeIndex);   // If the correct calibration command is received, the calibration function should be called.

         EEPROM_read(SlopeValueAddress, slopeValue);    // After calibration, the new slope and intercept should be read ,to update current value.
         EEPROM_read(InterceptValueAddress, interceptValue);
      }

      static unsigned long sampleTimepoint = millis();

      if(millis() - sampleTimepoint > 40U) {
         sampleTimepoint = millis();
         analogBuffer[analogBufferIndex] = analogRead(SensorPin) / 1024.0 * VREF; //read the voltage and store into the buffer,every 40ms
         analogBufferIndex++;

         if(analogBufferIndex == SCOUNT)
            analogBufferIndex = 0;

         averageVoltage = getMedianNum(analogBuffer, SCOUNT);  // read the stable value by the median filtering algorithm
      }

      static unsigned long printTimepoint = millis();

      if(millis() - printTimepoint > 1000U) {
         printTimepoint = millis();
         if(enterCalibrationFlag) {         // in calibration mode, print the voltage to user, to watch the stability of voltage
            Serial.print("Voltage:");
            Serial.print(averageVoltage);
            Serial.println("mV");
         } else {
            Serial.print("pH:");           // in normal mode, print the ph value to user
            Serial.println(averageVoltage / 1000.0 * slopeValue + interceptValue);
         }
      }
   }

  private:
   Bool check_serial_data(void) {
      static TimeStamp receivedTimeOut = millis();
      char receivedChar;

      while (Serial.available() > 0) {
         if (millis() - receivedTimeOut > 1000U) {
            receivedBufferIndex = 0;
            memset(receivedBuffer, 0, (ReceivedBufferLength + 1));
         }

         receivedTimeOut = millis();
         receivedChar = Serial.read();

         if (receivedChar == '\n' || receivedBufferIndex == ReceivedBufferLength) {
            receivedBufferIndex = 0;
            strupr(receivedBuffer);
            return true;
         } else {
            receivedBuffer[receivedBufferIndex] = receivedChar;
            receivedBufferIndex++;
         }
      }
      return false;
   }

   Byte uartParse() {
      Byte modeIndex = 0;

      *TODO*

      gör åtskillnad:
         om ( mod || mod+inst || inst )      => set context mod, inst
         annars (mod+inst+cmd || mod+cmd )   => anv. mod+inst till cur cmd

      om select-mod-mode      => select active mode
      om select-instance-mode => select specific instance
      om parse-command-mode   => lookup command in selected-mod
      om args-mode            => match args according to current-command



      // for (int ix = 0; ix < 
      mod = subscribers.by_key(the_mod_name_key)
      inst = mod.by_key(the_inst_ix_or_addr)
      cmd = mod.commands.by_key(cmd_token)


      if(strstr(receivedBuffer, "CAL!") != NULL)
         modeIndex = 1;
      else if(strstr(receivedBuffer, "DONE!") != NULL)
         modeIndex = 4;
      else if(strstr(receivedBuffer, "PH4!") != NULL)
         modeIndex = 2;
      else if(strstr(receivedBuffer, "PH10!") != NULL)
         modeIndex = 3;
      return modeIndex;
   }

};


CommandRouter command_router();


#define check_say_ok_or_return {if (command_router.dialogue_in_action() == true) return;}

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
