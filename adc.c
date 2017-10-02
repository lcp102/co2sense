#include <stdio.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
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
error            : this is the success indicator < 0 : Error  ==0 is Success , -1 == device error , -2 : register error
*/
float ads115_read_volts(uint8_t config[], int slaveaddr, int* error){
  const float VPS = 4.096/32767.0 ;// since we intend to keep the gain at 4.096V
  const int slave_addr =slaveaddr;
  int sps=128;
  int fd; //this is the open file pointer.
  int16_t val;
  uint8_t readBuffer[3] ;
  if ((fd = open("/dev/i2c-1", O_RDWR)) <0) {
    printf("Could not open device %d\n",fd );
    *error = -1; //device level error
    return 0;
  }
  if(ioctl(fd, I2C_SLAVE, slave_addr) < 0){
    printf("Failed to connect to I2C slave\n");
    *error  = -1;
    return 0;
  }
  if (write(fd, config, 3)!=3) {
    perror("configuration write error:");
    printf("There was error writing to the configuration register\n");
    *error = -2;
    return 0;
  }
  do {
    if (read(fd, config, 2)!=2) {
      printf("Could not read the register \n" );
      *error = -2;
      return 0;
    }
  } while(config[0] & 0x80 ==0);
  usleep((1/(float)sps)*1000000+2000);
  readBuffer[0] = 0;
  if (write(fd, readBuffer,1)!=1) {
    *error = -2;
    return 0;
  }
  // and then we go ahead to read from the conversion register
  if (read(fd, readBuffer, 2)!=2) {
    printf("Error reading the conversion register\n");
    *error = -2;
    return 0;
  }
  val  = (readBuffer[0] <<8 | readBuffer[1]);
  if (val <0) {
    val =0.00;
  }
  close(fd);
  *error =0;
  return val * VPS;
}
