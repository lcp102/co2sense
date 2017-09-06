#include<stdlib.h>
#include<stdio.h>
#include <wiringPi.h>
#include<signal.h>
/*
this is to have enwrapping controlling code for device programme
at the onset this kickstarts the co2sensing program and if this service gets interrupted then it closes the device service as well
*/
#define BTN_ENDSERVICE 21
void on_interrupt(int signal);
void on_user_interrupt();
int is_device_service_running();

int main(int argc, char const *argv[]) {
  //we are going to observe the falling of this button
  //this is when this very service is being closed.
  signal(SIGINT, &on_interrupt);
  signal(SIGTERM, &on_interrupt);
  wiringPiSetupGpio();
  pinMode(BTN_ENDSERVICE, INPUT);
  pullUpDnControl(BTN_ENDSERVICE, PUD_UP);
  wiringPiISR(BTN_ENDSERVICE, INT_EDGE_FALLING, &on_user_interrupt);
  if (is_device_service_running() == 0){
    // if there are no contents that denotes the service is offline
    system("systemctl start co2sensing.service");
  }
  else{
    // if there are contents then that denotes the service is already online
    system("systemctl restart co2sensing.service");
  }
  while (1) {
    sleep(2);
  }
  return 0;
}
void on_user_interrupt(){
  if (is_device_service_running() ==1){
    system("systemctl stop co2sensing.service");
  }
  else{
    // this is the case when perhaps the user wishes to restart the service which was stopped
    system("systemctl restart co2sensing.service");
  }
}
void on_interrupt(int signal){
  system("systemctl stop co2sensing.service");
  exit(0);
}
int is_device_service_running(){
  system("systemctl status co2sensing.service | grep running > /var/tmp/co2sensingtemp.txt");
  FILE *fd ;
  if((fd = fopen("/var/tmp/co2sensingtemp.txt", "r")) >0){
    fseek(fd, 0, SEEK_END);
    long fsize = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    fclose(fd);
    if (fsize == 0){
      // if there are no contents that denotes the service is offline
      return 0;
    }
    else{
      // if there are contents then that denotes the service is already online
      return 1;
    }
  }
  else{exit(1);}
}
