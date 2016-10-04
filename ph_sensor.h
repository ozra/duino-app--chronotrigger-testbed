/*
# This sample codes is for testing the pH meter V1.0.
 # Editor : YouYou
 # Date    : 2013.10.12
 # Ver      : 0.1
 # Product: pH meter
 # SKU      : SEN0161
*/

#define PH_MEASURE_INTERVAL_SPAN 100
float ph_reading;

void init_ph_sensor() {
   pinMode(PH_SENSOR_PIN, INPUT);
}

void log_ph_sensor() {
   Serial.print("pH: ");
   Serial.print(ph_reading, 2);
}

void attend_ph_sensor() {
   static Fsm fsm = {__STATE_NONE, 0, 0, __STATE_NONE};
   static SignalFilterEma<float> ema(0.5);

   if (check_state_deferring(&fsm)) return;

   // unsigned long int avgValue;   //Store the average value of the sensor feedback
   // float             b;
   // int               buf[10];
   // int               temp;

   // //Get 10 sample value from the sensor for smooth the value
   // for(int i=0; i<10; i++) {
   //    buf[i] = analogRead(PH_SENSOR_PIN);

   //    delay(10);

   // }

   // //sort the last 10 samples, ascending
   // for(int i=0; i<9; i++) {
   //    for(int j=i+1; j<10; j++) {
   //       if(buf[i] > buf[j]) {
   //          temp = buf[i];
   //          buf[i] = buf[j];
   //          buf[j] = temp;
   //       }
   //    }
   // }

   // avgValue = 0;

   // //take the average value of 6 center samples (remove possible outliers)
   // for(int i=2; i<8; i++)
   //    avgValue += buf[i];

   int   reading = analogRead(PH_SENSOR_PIN);
   // float smoothed_reading = reading;
   float smoothed_reading = ema(reading);

   //convert the analog into millivolt
   float phValueMillivolts = (float) smoothed_reading * 5.0 / 1024 / 6;

   //convert the millivolt into pH value
   ph_reading = 3.5 * phValueMillivolts;

   defer_state(&fsm, __STATE_NONE, PH_MEASURE_INTERVAL_SPAN);
}


