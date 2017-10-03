#include <stdio.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <unistd.h>
/*
author    :kneerunjun
This was created to modulate the load from device sensing loop
this would control the working of analog to digital conversions for the raspberry pi
*/

/*
config[0]     : the register id that you need to write to
config[1]     : msb of the configuration
config[2]     : lsb of the configuration
slaveaddr     : depending on the hardware configuraion you can actually set this, default 0x48
error         : this is the success indicator < 0 : Error  ==0 is Success , -1 == device error , -2 : register error
*/

float read_device(uint8_t conf[], int addr ,int* error){
  int fd; //this is the device pointer.
  int sps=128;
  const float VPS = 4.096/32767.0 ;// since we intend to keep the gain at 4.096V
  int16_t val;
  uint8_t readBuffer[3] ;
  if ((fd = open("/dev/i2c-1", O_RDWR)) <0) {
    printf("Could not open device %d\n",fd );
    return -1;
  }
  if(ioctl(fd, I2C_SLAVE, addr) < 0){
    printf("Failed to connect to I2C slave\n");
    return -1;
  }
  if (write(fd, conf, 3)!=3) {
    perror("configuration write error:");
    printf("There was error writing to the configuration register\n");
    *error= -1;
    return 0;
  }
  do {
    if (read(fd, conf, 2)!=2) {
      printf("Could not read the register \n" );
      *error= -1;
      return 0;
    }
  } while(conf[0] & 0x80 ==0);
  usleep((1/(float)sps)*1000000+2000);
  readBuffer[0] = 0;
  if (write(fd, readBuffer,1)!=1) {
    *error= -2;
    return 0;
  }
  // and then we go ahead to read from the conversion register
  if (read(fd, readBuffer, 2)!=2) {
    printf("Error reading the conversion register\n");
    *error= -2;
    return 0;
  }
  val  = (readBuffer[0] <<8 | readBuffer[1]);
  if (val <0) {
    val =0.00;
  }
  close(fd);
  return val*VPS;
}
int ads115_read_volts(int slaveaddr,float* readings){
  const int slave_addr =slaveaddr;
  uint8_t a1Config[3],a0Config[3], a2Config[3];
  a0Config[0]=1;
  a0Config[1]=0b11000011;
  a0Config[2]=0b10000011;
  a1Config[0]=1;
  a1Config[1]=0b11010011;
  a1Config[2]=0b10000011;
  a2Config[0]=1;
  a2Config[1]=0b11100011;
  a2Config[2]=0b10000011;
  int err =0;
  float  volts  = read_device(a0Config, 0x48,&err);
  // printf("%.5f\n", volts);
  if (err==0) {
    *(readings) = volts;
  }
  volts  = read_device(a1Config, 0x48,&err);
  // printf("%.5f\n", volts);
  if (err==0) {
    *(readings+1) = volts;
  }
  volts  = read_device(a2Config, 0x48,&err);
  // printf("%.5f\n", volts);
  if (err==0) {
    *(readings+2) = volts;
  }
  return 0;
}
