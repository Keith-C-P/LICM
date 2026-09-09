#include <stdio.h>

void fizzbuzz(int max){
  for (int i = 0; i <= max; i++) {
    int printed = 0;

    if (i % 3 == 0) {
      printf("Fizz");
      printed = 1;
    }

    if (i % 5 == 0) {
      printf("Buzz");
      printed = 1;
    }

    if (!printed) printf("%d", i);

    printf("\n");
  }
}

int main(){
  fizzbuzz(100);
  return 0;
}
