#include<stdio.h>
#include<stdlib.h>
#include"mq135.h"
#include<unistd.h>
int main(int argc, char const *argv[]) {
  while (1) {
    int ok =0;
    float ppm =ppm_co2(&ok, 0, 0);
    if (ok==0) {
      /* code */
      printf("%.3f\n", ppm);
    }
    sleep(3);
  }
  return 0;
}
