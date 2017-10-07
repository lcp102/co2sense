#include "ldr.h"
#include <stdio.h>
#include <unistd.h>
int main(int argc, char const *argv[]) {
  int ok;
  while(1){
    float light =light_percent(&ok,2,2.2831,1.6851);
    printf("%.2f\n",light );
    sleep(3);
  }
  return 0;
}
