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
#include <sys/types.h> // open
#include <sys/stat.h>  // open
#include <fcntl.h>     // open
#include <unistd.h>    // read/write usleep
#include <inttypes.h>  // uint8_t, etc
#include <linux/i2c-dev.h> // I2C bus definitions
#include <wiringPi.h>
#include<lcd.h>
#include <string.h>
#include<signal.h>
// this is for making system calls via shell
// #include<system.h>

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
#define DARK_VOLTS 2.042
#define BRIGHT_VOLTS 2.049
int lcd;
void on_interrupt(int signal);
void flush_gpio();
void indicate_led_buzz(float ppm);
float read_voltage(uint8_t config[]);
void display_marquee(float temp, float light, float co2);
void on_restart();

int main(int argc, char const *argv[]) {
  uint8_t writeBuffer[3] ;
  size_t i;
  float a0, a1, lightVolts, lightPercent;
  // register a signal
  // here we are testing only for the SIGINT
  signal(SIGINT, &on_interrupt);
  signal(SIGTERM, &on_interrupt);
  // this is being setup from the upstart service , re setting here woudl lead to some problem
  wiringPiSetupGpio();
  // for the buzzer , and the RGB LED here we can setup pins
  lcd = lcdInit (2,16,4,RS,E,D0,D1,D2,D3,0,0,0,0);
  lcdPuts(lcd, "Sensing...");
  // here do some prep calculations for C02 measurement
  pinMode (RED_GPIO, OUTPUT) ; digitalWrite(RED_GPIO, LOW);
  pinMode (BLUE_GPIO, OUTPUT) ; digitalWrite(BLUE_GPIO, LOW);
  pinMode (BUZZ_GPIO, OUTPUT) ; digitalWrite(BUZZ_GPIO, LOW);

  float ratio_rs_ro=pow(10, ((SLOPE*(log10(CO2_PPM_NOW))+Y_INTERCEPT)));
  writeBuffer[0]=1;
  writeBuffer[1]=0b11000011; //this configuration signifies
  writeBuffer[2]=0b10000011;
  float Vrl=read_voltage(writeBuffer); //this is the voltage across the load resistor
  float Rs=(5.00 * RESIS_LOAD/Vrl)- RESIS_LOAD;
  float Ro=Rs/ratio_rs_ro; //this is one time activity .. we woudl no longer do this in a loop
  float ppm;
  while (1) {
    writeBuffer[0]=1;
    writeBuffer[1]=0b11000011; //this configuration signifies
    writeBuffer[2]=0b10000011;
    Vrl=read_voltage(writeBuffer); //this is the voltage across the load resistor
    Rs=(5.00 * RESIS_LOAD/Vrl)- RESIS_LOAD;
    ppm = pow(10,((log10(Rs/Ro)-Y_INTERCEPT)/SLOPE));
    writeBuffer[0]=1;
    writeBuffer[1]=0b11010011; //this configuration signifies
    writeBuffer[2]=0b10000011;
    a1=read_voltage(writeBuffer);
    writeBuffer[0]=1;
    writeBuffer[1]=0b11100011; //this configuration signifies
    writeBuffer[2]=0b10000011;
    lightVolts=read_voltage(writeBuffer);
    // we here need to convert the voltage to an proportionate light brightness reading
    lightPercent= 1 -((float)(lightVolts- DARK_VOLTS)/(BRIGHT_VOLTS-DARK_VOLTS));
    if (lightPercent<0.00) {
      lightPercent=0.00;
    }
    if (lightPercent>1.00) {
      lightPercent=1.00;
    }
    display_marquee(a1*100,lightPercent,ppm);
    indicate_led_buzz(ppm);
    sleep(LOOP_SLEEP_SECS);
  }
  flush_gpio();
  return 0;
}
void on_restart(){
  // this gets the systemctl service restarted
  // effectively this service is issuing commands to kill itself
  printf("We have a command to restart the program..\n");
  sleep(2);
  static unsigned long lastISRHit = 0;
  unsigned long currIsrHit = millis();
  if(currIsrHit-lastISRHit>=500){
    // to avoid issuing commands in hystersis or fast mode
    system("sudo systemctl restart co2sensing.service"); //we are just instructing the system to restart
  }
}
void display_marquee(float temp, float light, float co2){
  char tempMessage[50], lightMessage[50], co2Message[50];
  // this is inline issue 2, we are getting the reading but i guess we are not adjusting to the correct decimal places.
  // we have now replaced the LDR with a brand new one.
  sprintf(tempMessage,"T:%.2f L:%.4f",temp, light);
  sprintf(co2Message,"Co2(ppm):%.3f",co2);
  lcdClear(lcd);
  lcdPuts(lcd, tempMessage);
  lcdPosition(lcd,0,1);
  lcdPuts(lcd, co2Message);
}
float read_voltage(uint8_t config[]){
  const float VPS = 4.096/32767.0 ;// since we intend to keep the gain at 4.096V
  const int slave_addr = 0x48;
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
  if (write(fd, config, 3)!=3) {
    perror("configuration write error:");
    printf("There was error writing to the configuration register\n");
    exit(-1);
  }
  do {
    if (read(fd, config, 2)!=2) {
      printf("Could not read the register \n" );
      exit(-1);
    }
  } while(config[0] & 0x80 ==0);
  usleep((1/(float)sps)*1000000+2000);
  readBuffer[0] = 0;
  if (write(fd, readBuffer,1)!=1) {
    perror("Error switching the register");
    exit(-1);
  }
  // and then we go ahead to read from the conversion register
  if (read(fd, readBuffer, 2)!=2) {
    printf("Error reading the conversion register\n");
    exit(-1);
  }
  val  = (readBuffer[0] <<8 | readBuffer[1]);
  if (val <0) {
    val =0.00;
  }
  close(fd);
  return val * VPS;
}
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
    if(digitalRead(BUZZ_GPIO)==0){digitalWrite(BUZZ_GPIO, HIGH);}
  }
}
void flush_gpio(){
  digitalWrite(BLUE_GPIO, LOW);
  digitalWrite(RED_GPIO, LOW);
  digitalWrite(BUZZ_GPIO, LOW);
  lcdClear(lcd);
  lcdPuts(lcd, "Sleeping ..");
}
void on_interrupt(int signal){
  flush_gpio();
  exit(-1);
}
