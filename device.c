/*
date          : 25-AUG-2017
author        : kneerunjun@gmail.com
purpose       : to try measuring Analog inputs on Rpi using an ADS1115 over I2C and then connect a MQ135 Co2 measurement unit
compilation   : gcc ./i2ctest.c -o ./bin/i2ctest -lwiringPi -lwiringPiDev -lm
run           : ./bin/i2ctest
*/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>    // exit, delay
// #include <sys/types.h> // open
// #include <sys/stat.h>  // open
// #include <fcntl.h>     // open
#include <unistd.h>    // read/write usleep
#include <inttypes.h>  // uint8_t, etc
// #include <linux/i2c-dev.h> // I2C bus definitions
#include <wiringPi.h>
#include<lcd.h>
#include <string.h>
#include<signal.h>
#include"./adc/adc.h"

#define RS 9
#define E 11
#define D0 14
#define D1 15
#define D2 18
#define D3 23
//trigger this for the visual and sonic indication
#define RED_GPIO 17
#define BLUE_GPIO 27
#define BUZZ_GPIO 22
#define BTN_SHUTDOWN 21
#define BTN_RESET 20
// refer to https://www.co2.earth/ for knowing the current concentration of C02
#define CO2_PPM_NOW 407.25
/* http://www.instructables.com/id/Arduino-Air-Monitor-Shield-Live-in-a-Safe-Environm
this actually can be seen from the characteristics of this sensor , for which you would have to rfer to datasheet
but then the above site makes it much more easier and gives values directly you can use
equation    : log(Rs/Ro) = m*log(ppm)+c
for knowing the various levels of Co2 https://www.kane.co.uk/knowledge-centre/what-are-safe-levels-of-co-and-co2-in-rooms
*/
#define Y_INTERCEPT 0.7597917824 //this has been considered the average c
#define SLOPE -0.370955166
#define RESIS_LOAD 2.5 // in Kohms , is the resistance on RL as measured
#define LOOP_MAX 3600
#define LOOP_SLEEP_SECS 2
// these extremeties need to adjusted / calibrated.
// this needs to be tested on a restart.
#define DARK_VOLTS 1.6851
#define BRIGHT_VOLTS 2.2831
#define DEBOUNCE 200
#define ADC_MQ135_CHN 0
#define ADC_LM35_CHN 1
#define ADC_LDR_CHN 2
int lcd; //pointer for the lcd object

void on_interrupt(int signal){
  digitalWrite(BLUE_GPIO, LOW);
  digitalWrite(RED_GPIO, LOW);
  digitalWrite(BUZZ_GPIO, LOW);
  lcdClear(lcd);
  lcdPuts(lcd, "Shutting down..!");
  lcdClear(lcd);
  exit(0);
}
void display_marquee(float temp, float light, float co2){
  char tempMessage[6], lightMessage[10], co2Message[16];
  // this is inline issue 2, we are getting the reading but i guess we are not adjusting to the correct decimal places.
  // we have now replaced the LDR with a brand new one.
  // sprintf(tempMessage,"T:%.2f\337 L:%.1f%%",temp, light);
  sprintf(tempMessage, "%.2f", temp);
  sprintf(lightMessage, " L:%.1f%%", light);
  sprintf(co2Message,"Co2(ppm):%.2f",co2);
  lcdClear(lcd);
  lcdPuts(lcd, tempMessage);
  lcdPutchar(lcd, 0); //0th character is defined as the deg ceclius symbol
  lcdPuts(lcd, lightMessage);
  lcdPosition(lcd,0,1);
  lcdPuts(lcd, co2Message);
}
// float read_voltage(uint8_t config[]){
//   const float VPS = 4.096/32767.0 ;// since we intend to keep the gain at 4.096V
//   const int slave_addr = 0x48;
//   int sps=128;
//   int fd; //this is the open file pointer.
//   int16_t val;
//   uint8_t readBuffer[3] ;
//   if ((fd = open("/dev/i2c-1", O_RDWR)) <0) {
//     printf("Could not open device %d\n",fd );
//     return -1;
//   }
//   if(ioctl(fd, I2C_SLAVE, slave_addr) < 0){
//     printf("Failed to connect to I2C slave\n");
//     return -1;
//   }
//   if (write(fd, config, 3)!=3) {
//     perror("configuration write error:");
//     printf("There was error writing to the configuration register\n");
//     exit(-1);
//   }
//   do {
//     if (read(fd, config, 2)!=2) {
//       printf("Could not read the register \n" );
//       exit(-1);
//     }
//   } while(config[0] & 0x80 ==0);
//   usleep((1/(float)sps)*1000000+2000);
//   readBuffer[0] = 0;
//   if (write(fd, readBuffer,1)!=1) {
//     perror("Error switching the register");
//     exit(-1);
//   }
//   // and then we go ahead to read from the conversion register
//   if (read(fd, readBuffer, 2)!=2) {
//     printf("Error reading the conversion register\n");
//     exit(-1);
//   }
//   val  = (readBuffer[0] <<8 | readBuffer[1]);
//   if (val <0) {
//     val =0.00;
//   }
//   close(fd);
//   return val * VPS;
// }
void indicate_led_buzz(float ppm){
  // we dont want the GPIO to be erquested to change state even when it is in the required state
  // so we change the state only when necessary
  if (ppm <850.00) {
    // the light is blue, buzzer is off
    if(digitalRead(BLUE_GPIO) ==0){digitalWrite(BLUE_GPIO, HIGH);}
    if(digitalRead(RED_GPIO)==1){digitalWrite(RED_GPIO, LOW);}
    if(digitalRead(BUZZ_GPIO)==1){digitalWrite(BUZZ_GPIO, LOW);}
  }
  else if(ppm <=1700.00){
    // the light is magenta, buzzer is off
    if(digitalRead(BLUE_GPIO) ==0){digitalWrite(BLUE_GPIO, HIGH);}
    if(digitalRead(RED_GPIO)==0){digitalWrite(RED_GPIO, HIGH);}
    if(digitalRead(BUZZ_GPIO)==1){digitalWrite(BUZZ_GPIO, LOW);}
  }
  else {
    // the light is red, buzzer is on
    if(digitalRead(BLUE_GPIO) ==1){digitalWrite(BLUE_GPIO, LOW);}
    if(digitalRead(RED_GPIO)==0){digitalWrite(RED_GPIO, HIGH);}
    if(digitalRead(BUZZ_GPIO)==0){
      digitalWrite(BUZZ_GPIO, HIGH);
      sleep(2);
      digitalWrite(BUZZ_GPIO, LOW);
    }
  }
}
int main(int argc, char const *argv[]) {
  // uint8_t writeBuffer[3] ;
  float a0, a1, lightVolts, lightPercent;
  // register a signal
  // here we are testing only for the SIGINT
  signal(SIGINT, &on_interrupt);
  signal(SIGTERM, &on_interrupt);
  // this is being setup from the upstart service , re setting here woudl lead to some problem
  wiringPiSetupGpio();
  lcd = lcdInit (2,16,4,RS,E,D0,D1,D2,D3,0,0,0,0);
  unsigned char char_degcelcius[8] = {0x18,0x18,0x3,0x4,0x4,0x4,0x3,0x0}; //defining a new symbol
  lcdCharDef (lcd, 0, char_degcelcius) ;
  lcdPuts(lcd, "Sensing...");
  pinMode (RED_GPIO, OUTPUT) ; digitalWrite(RED_GPIO, LOW);
  pinMode (BLUE_GPIO, OUTPUT) ; digitalWrite(BLUE_GPIO, LOW);
  pinMode (BUZZ_GPIO, OUTPUT) ; digitalWrite(BUZZ_GPIO, LOW);
  // here do some prep calculations for C02 measurement
  float ratio_rs_ro=pow(10, ((SLOPE*(log10(CO2_PPM_NOW))+Y_INTERCEPT)));
  // writeBuffer[0]=1;
  // writeBuffer[1]=0b11000011; //this configuration signifies
  // writeBuffer[2]=0b10000011;
  int ok =0;
  float Vrl= ads115_read_channel(0x48,ADC_MQ135_CHN, GAIN_TWO, DR_128,&ok);
  if(ok!=0){perror("We have a problem reading the ADC channel");}
  float Rs=(5.00 * RESIS_LOAD/Vrl)- RESIS_LOAD;
  float Ro=Rs/ratio_rs_ro; //this is one time activity .. we woudl no longer do this in a loop
  float ppm;
  while (1) {
    // writeBuffer[0]=1;
    // writeBuffer[1]=0b11000011; //this configuration signifies
    // writeBuffer[2]=0b10000011;
    Vrl=ads115_read_channel(0x48,ADC_MQ135_CHN, GAIN_TWO, DR_128,&ok);
    Rs=(5.00 * RESIS_LOAD/Vrl)- RESIS_LOAD;
    ppm = pow(10,((log10(Rs/Ro)-Y_INTERCEPT)/SLOPE));
    // writeBuffer[0]=1;
    // writeBuffer[1]=0b11010011; //this configuration signifies
    // writeBuffer[2]=0b10000011;
    a1=ads115_read_channel(0x48,ADC_LM35_CHN, GAIN_FOUR, DR_128,&ok);
    if(ok!=0){perror("We have a problem reading the temperature channel on the ADS");}
    // writeBuffer[0]=1;
    // writeBuffer[1]=0b11100011; //this configuration signifies
    // writeBuffer[2]=0b10000011;
    lightVolts=ads115_read_channel(0x48,ADC_LDR_CHN,GAIN_ONE,DR_128,&ok);
    // we here need to convert the voltage to an proportionate light brightness reading
    lightPercent= ((float)(lightVolts- DARK_VOLTS)/(BRIGHT_VOLTS-DARK_VOLTS));
    if (lightPercent<0.00) {
      lightPercent=0.00;
    }
    if (lightPercent>1.00) {
      lightPercent=1.00;
    }
    display_marquee(a1*100,lightPercent*100,ppm);
    indicate_led_buzz(ppm);
    sleep(LOOP_SLEEP_SECS);
  }
  prep_hw_shutdown();
  return 0;
}
