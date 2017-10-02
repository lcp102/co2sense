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
int ads115_read_volts(int slaveaddr,float* readings){
  const float VPS = 4.096/32767.0 ;// since we intend to keep the gain at 4.096V
  const int slave_addr =slaveaddr;
  uint8_t a1Config[3];
  a1Config[0]=1;
  a1Config[1]=0b11010011;
  a1Config[2]=0b10000011;
  int sps=128;
  int fd; //this is the open file pointer.
  int16_t val;
  uint8_t readBuffer[3] ;
  if ((fd = open("/dev/i2c-1", O_RDWR)) <0) {
    printf("Could not open device %d\n",fd );
    return -1;
  }
  if(ioctl(fd, I2C_SLAVE, slave_addr) < 0){
    printf("Failed to connect to I2C slave\n");
    return -1;
  }
  if (write(fd, a1Config, 3)!=3) {
    perror("configuration write error:");
    printf("There was error writing to the configuration register\n");
    return -1;
  }
  do {
    if (read(fd, a1Config, 2)!=2) {
      printf("Could not read the register \n" );
      return -1;
    }
  } while(a1Config[0] & 0x80 ==0);
  usleep((1/(float)sps)*1000000+2000);
  readBuffer[0] = 0;
  if (write(fd, readBuffer,1)!=1) {
    return -2;
  }
  // and then we go ahead to read from the conversion register
  if (read(fd, readBuffer, 2)!=2) {
    printf("Error reading the conversion register\n");
    return -2;
  }
  val  = (readBuffer[0] <<8 | readBuffer[1]);
  if (val <0) {
    val =0.00;
  }
  close(fd);
  *(readings) = 0.00;
  *(readings+1) =  val * VPS;
  return 0;
}
