#include <stdio.h>
#include <stdlib.h>

static int daytab[2][13] = {
  {0,31,28,31,30,31,30,31,31,30,31,30,31},
  {0,31,29,31,30,31,30,31,31,30,31,30,31}
};

char *month_name(int n){
  static char *name[] = {
    "Illegal month", "Jan", "Feb", "Mar",
    "Apr", "May", "Jun", "Jul", "Aug",
    "Sep", "Oct", "Nov", "Dec"
  };

  return (n < 1 || n > 12) ? name[0]: name[n];
}

void month_day(int year, int yearday, int *pmonth, int *pday){
  int leap = year%4 == 0 && year%100 != 0 || year%400 == 0;
  *pmonth += 1;

  while (yearday >= daytab[0][*pmonth] && *pmonth < 12){
    pday = &daytab[leap][*pmonth];
    yearday -= *pday;
    *pmonth += 1;
  }
  
  if ( yearday > daytab[leap][*pmonth] || *pmonth > 12 || *pmonth < 1){
    printf("Error: The value was too large for a day\n");
    return;
  }
  
  char *monthToString = month_name(*pmonth);
  printf("%s %d, %d\n", monthToString, yearday, year);
}


int main(int argc, char const *argv[])
{
  int pmonth = 0;
  int pday = 0;

  if (argc < 3){
    printf("Error: You need more arguments\n");
    return 1;
  }

  int year = atoi(argv[1]);
  int day = atoi(argv[2]);

  month_day(year,day, &pmonth, &pday);
  return 0;
}