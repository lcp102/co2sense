#include<stdlib.h>
#include<stdio.h>
#include <wiringPi.h>
#include<signal.h>
/*
this is to have enwrapping controlling code for device programme
at the onset this kickstarts the co2sensing program and if this service gets interrupted then it closes the device service as well

*/
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
int main(int argc, char const *argv[]) {
  signal(SIGINT, on_interrupt);
  signal(SIGTERM, on_interrupt);
  if (is_device_service_running() == 0){
    // if there are no contents that denotes the service is offline
    system("systemctl start co2sensing.service");
  }
  else{
    // if there are contents then that denotes the service is already online
    system("systemctl restart co2sensing.service");
  }
  return 0;
}
