#include <stdio.h>
#include<stdint.h>
#include<unistd.h>
#include <wiringPi.h>
#include "adc.h"

int main(int argc, char const *argv[]) {
  /* here we write code to test the adc working */
  wiringPiSetupGpio();
  // lets see if we have the ads reading here as well.
  uint8_t writeBuffer[3] ;
  int error =0;
  // ads115_get_config(1,4,1,1,4,0,0,0,3,writeBuffer, &error); // this for the A0 channel 128 sps , 4.096 PGA
  // ads115_get_config(0b1,0b101,0b1,0b1,0b100,0b0,0b0,0b0,0b101,writeBuffer, &error); // this for the A1 channel 128 sps , 4.096 PGA
  // printf("%d\n", writeBuffer[0]);
  // printf("%d\n", writeBuffer[1]);
  // printf("%d\n", writeBuffer[2]);
  printf("Starting conversions ..\n");
  // writeBuffer[0]=1;
  // writeBuffer[1]=0b11010011; //this configuration signifies
  // writeBuffer[2]=0b10000011;
  // float a1  =ads115_read_volts(writeBuffer, 0x48, &error);
  // writeBuffer[0]=1;
  // writeBuffer[1]=0b11100011; //this configuration signifies
  // writeBuffer[2]=0b10000011;
  // float a2  =ads115_read_volts(writeBuffer, 0x48, &error);
  // printf("%.3f\t\t%.3f\t\t%.3f", a0,a1,a2);
  printf("A0(Volts)\t\tA1(Volts)\t\tA2(Volts)\n");
  printf("=======================================\n");
  while(1){
    float* voltages =ads115_read_volts(0x48);
    printf("%.4f\t\t%.4f\t\t%.4f\t\t%.4f\n", *voltages,  *(voltages+1), *(voltages+2),*(voltages+3));
    sleep(3);
  }
  return 0;
}
