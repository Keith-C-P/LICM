#include <stdio.h>

int factorial(int n){
  if (n <= 1)
    return n;
  return n * factorial(n - 1);
}

int main(){
  int x = 5; //expected 120
  printf("%d", factorial(x));
  return 0;
}
