#include <stdio.h>
#include<stdint.h>
#include<unistd.h>
#include <wiringPi.h>
#include "adc.h"

int main(int argc, char const *argv[]) {
  wiringPiSetupGpio();
  uint8_t writeBuffer[3] ;
  int error =0;
  printf("Starting conversions ..\n");
  float voltages[4]; //all the channels' readings
  while(1){
    // int result =ads115_read_volts(0x48, voltages);
    // if (result==0) {
    //   printf("%.4f\t\t%.4f\t\t%.4f\t\t%.4f\n", *voltages,  *(voltages+1), *(voltages+2),*(voltages+3));
    // }
    // this is where we go ahead to test the single channel read ..
    int ok =0;
    PGA gain = GAIN_EIGHT;
    float volts =ads115_read_channel(0x48, 1,gain, &ok);
    if (ok==0) {
      printf("%.4f\n",volts );
    }
    sleep(3);
  }
  return 0;
}
