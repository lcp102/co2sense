#ifndef ADC_H
#define ADC_H
typedef enum {GAIN_TWOTHIRDS=0, GAIN_ONE=1, GAIN_TWO=2, GAIN_FOUR=3, GAIN_EIGHT=4, GAIN_SIXTEEN=5} PGA;
int ads115_read_volts(int slaveaddr,float* readings);
float ads115_read_channel(int slaveaddr, int channel, PGA gain, int* ok);
#endif
