#include <stdio.h>
#include<stdint.h>
#include<unistd.h>
#include <wiringPi.h>
#include "adc.h"
void test_singlechannel_read(){
  int ok =0;
  PGA gain = GAIN_ONE;
  DTRATE dr = DR_128;
  float voltsA1=ads115_read_channel(0x48, 1,gain, dr,&ok);
  float voltsA2 =ads115_read_channel(0x48, 2,gain, dr,&ok);
  if (ok==0) {
    printf("%.4f\t\t%.4f\n",voltsA1, voltsA2 );
  }
}
void test_allchannels_read(){
  float voltages[4]; //all the channels' readings
  int result =ads115_read_all_channels(0x48, voltages);
  if (result==0) {
    printf("%.4f\t\t%.4f\t\t%.4f\t\t%.4f\n", *voltages,  *(voltages+1), *(voltages+2),*(voltages+3));
  }
}
int main(int argc, char const *argv[]) {
  wiringPiSetupGpio();
  int error =0;
  printf("Starting conversions ..\n");
  while(1){
    test_allchannels_read();
    // test_singlechannel_read();
    sleep(3);
  }
  return 0;
}
