#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
  
  int fahr;
  
  if (argc == 1 || argc == 3){
    printf("There was an error:\nCheck the arguments and try again\n");
  }
  else if(argc == 2){
    printf("Farenheit: %3d, Celsius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));
  }
  else if(argc == 4){
    for (fahr = atoi(argv[1]); fahr <= atoi(argv[2]); fahr = fahr + atoi(argv[3]))
    printf("Farenheit: %3d, Celsius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));
  }
  return 0;
}