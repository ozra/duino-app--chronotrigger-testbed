#ifndef SIGNAL_PROCESSING_OUTLIER_FILTER
#define SIGNAL_PROCESSING_OUTLIER_FILTER

#include "SignalProcessingMath.h"

namespace SignalProcessing {




// *TODO*


         // unsigned long int avgValue;   //Store the average value of the sensor feedback
         // F32             b;
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

}

#endif
