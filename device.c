/*
date          : 25-AUG-2017
author        : kneerunjun@gmail.com
purpose       : to try measuring Analog inputs on Rpi using an ADS1115 over I2C and then connect a MQ135 Co2 measurement unit
compilation   : gcc ./device.c -o ./bin/device -lwiringPi -lwiringPiDev -lm -L./adc -ladc
run           : ./bin/i2ctest
*/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>    // exit, delay
#include <wiringPi.h>
#include<lcd.h>
#include <string.h>
#include<signal.h>
#include "./mq135/mq135.h"
#include "./ldr/ldr.h"
#include "./lm35/lm35.h"
#include "./alerts/alerts.h"

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
/* http://www.instructables.com/id/Arduino-Air-Monitor-Shield-Live-in-a-Safe-Environm
this actually can be seen from the characteristics of this sensor , for which you would have to rfer to datasheet
but then the above site makes it much more easier and gives values directly you can use
equation    : log(Rs/Ro) = m*log(ppm)+c
for knowing the various levels of Co2 https://www.kane.co.uk/knowledge-centre/what-are-safe-levels-of-co-and-co2-in-rooms
*/
#define LOOP_MAX 3600
#define LOOP_SLEEP_SECS 2
// these extremeties need to adjusted / calibrated.
// this needs to be tested on a restart.
#define DARK_VOLTS 1.6851
#define BRIGHT_VOLTS 2.2831
#define DEBOUNCE 200
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
  sprintf(tempMessage, "%.2f", temp);
  sprintf(lightMessage, ":%.1f%%", light);
  sprintf(co2Message,":%.1f",co2);
  lcdClear(lcd);
  lcdPuts(lcd, tempMessage);
  lcdPutchar(lcd, 0); //0th character is defined as the deg ceclius symbol
  lcdPuts(lcd,"  ");
  lcdPutchar(lcd,1);
  lcdPuts(lcd, lightMessage);
  lcdPosition(lcd,0,1);
  lcdPutchar(lcd,2);
  lcdPuts(lcd, co2Message);
}
int main(int argc, char const *argv[]) {
  int ok =0;
  // register a signal
  // here we are testing only for the SIGINT
  signal(SIGINT, &on_interrupt);
  signal(SIGTERM, &on_interrupt);
  // this is being setup from the upstart service , re setting here woudl lead to some problem
  wiringPiSetupGpio();
  lcd = lcdInit (2,16,4,RS,E,D0,D1,D2,D3,0,0,0,0);
  unsigned char char_degcelcius[8] = {0x18,0x18,0x3,0x4,0x4,0x4,0x3,0x0}; //defining a new symbol
  unsigned char char_lightlambda[8]={0x4,0xa,0xa,0xa,0x11,0x1f,0xe,0x0};
  unsigned char char_co2[8]={0x2,0x4,0xe,0x10,0x10,0x11,0xe,0x0};
  lcdCharDef (lcd, 0, char_degcelcius) ;
  lcdCharDef (lcd, 1, char_lightlambda) ;
  lcdCharDef (lcd, 2, char_co2) ;
  lcdPuts(lcd, "Sensing...");

  // setting up the alerts
  setup_alert(BLUE_GPIO, RED_GPIO, BUZZ_GPIO);
  while (1) {

    float ppm=ppm_co2(&ok, 0, 0);
    if(ok!=0){perror("device.c: failed to get the co2 footprint");continue;}

    float temp  = airtemp_now(&ok,1, CELCIUS);
    if(ok!=0){perror("We have a problem reading the temperature channel on the ADS");}

    float light =light_percent(&ok, 2, BRIGHT_VOLTS, DARK_VOLTS);
    if(ok!=0){perror("We have a problem reading the light intensity");}

    display_marquee(temp,light*100,ppm);
    if (ppm <=700.00) {alert(&ok, 0, 0);}
    else if(ppm>700.00 && ppm<=1500.00){alert(&ok, 1, 0);}
    else if(ppm>1500.00){alert(&ok, 2, 0);}
    sleep(LOOP_SLEEP_SECS);
  }
  prep_hw_shutdown();
  return 0;
}
