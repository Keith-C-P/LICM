#include <stdio.h>

int factorial(int x){
  int fact = 1;
  int i = 1;

  while (i <= x){
    fact = fact * i;
    i = i + 1;
  }

  return fact;
}

int main(){
  int x = 5; //expected 120
  // printf("%d", factorial(x));
  return 0;
}
