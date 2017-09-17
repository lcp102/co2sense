#include<stdlib.h>
#include<stdio.h>
#include <wiringPi.h>
#include<signal.h>

/*
this is to have enwrapping controlling code for device programme
at the onset this kickstarts the co2sensing program and if this service gets interrupted then it closes the device service as well
*/
#define BTN_ENDSERVICE 21
#define DEBOUNCE 200 // time in ms between which the interrupt does not count.

void on_sys_interrupt(int signal);
void on_btn_interrupt();
int sensing_service_toggle();

int main(int argc, char const *argv[]) {
  //we are going to observe the falling of this button
  //this is when this very service is being closed.
  signal(SIGINT, &on_sys_interrupt);
  signal(SIGTERM, &on_sys_interrupt);
  wiringPiSetupGpio();
  sleep(1);
  pinMode(BTN_ENDSERVICE, INPUT);
  pullUpDnControl(BTN_ENDSERVICE, PUD_UP);
  wiringPiISR(BTN_ENDSERVICE, INT_EDGE_FALLING, &on_btn_interrupt);
  sensing_service_toggle();
  // this is just to keep the program ticking and suspended.
  while (1) {
    sleep(3);
  }
  return 0;
}
/*This helps in intercepting the button interrupts, please note that the debounce time is important to offset the hsyterisis */
void on_btn_interrupt(){
  static unsigned long lastISRTime =0;
  unsigned long currentISRTime = millis();
  if(currentISRTime-lastISRTime >DEBOUNCE){
    sensing_service_toggle();
  }
  lastISRTime = currentISRTime;
}
void on_sys_interrupt(int signal){
  // this is when we experience one of the termination signals from the system
  system("systemctl stop co2sensing.service");
  exit(0);
}
/*
This assesses the state  of the sensing service and toggles it when required
runs the system command and redirects the output to an temp file.
this file when read , lets us know the state
it also fires the sensing service depending on the state
*/
int sensing_service_toggle(){
  system("systemctl status co2sensing.service | grep running > /var/tmp/co2sensingtemp.txt");
  FILE *fd ;
  if((fd = fopen("/var/tmp/co2sensingtemp.txt", "r")) >0){
    fseek(fd, 0, SEEK_END);
    long fsize = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    fclose(fd);
    if (fsize == 0){
      // if there are no contents that denotes the service is offline
      system("systemctl start co2sensing.service");
      return 0;
    }
    else{
      // if there are contents then that denotes the service is already online
      system("systemctl stop co2sensing.service");
      return 1;
    }
  }
  else{exit(1);}
}
