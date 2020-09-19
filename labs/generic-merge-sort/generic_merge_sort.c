#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAXCHARS 1000

char* lines[MAXCHARS];

void MergeSort(char **A, int p, int r, int num);
void Merge(char **A, int p, int q, int r, int num);

int numcmp(char *, char* );
void printArray(int, char *[]);
int getAllValues(char *[]);
int getValue(char []);

int main(int argc, char *argv[])
{
  int size = getAllValues(lines);
  int numeric = 0;
  if(argc > 1 && strcmp(argv[1], "-n")==0){
    numeric = 1;
  }
  int p = 0;
  int r = size-1;
  
  MergeSort(lines, p, r, numeric);

  printArray(size, lines);
  return 0;
}

void MergeSort(char **A, int p, int r, int num){
  if (p < r){
    int q = (int)floor((p+r)/2);
    MergeSort(A,p,q,num);
    MergeSort(A,q+1,r,num);
    Merge(A,p,q,r,num);
  }
}

void Merge(char **A, int p, int q, int r, int num){
  int n1 = q - p + 1;
  int n2 = r - q;
  char *Left[n1+1];
  char *Right[n2+1];
  for(int i = 0; i<n1; i++){
    Left[i] = A[p+i];
  }
  for(int j = 0; j<n2; j++)
    Right[j] = A[q+j+1];

  if (num){
    Left[n1] = "10000000";
    Right[n2] = "10000000";
  }
  else{
    Left[n1] = "~";
    Right[n2] = "~";
  }

  int i = 0;
  int j = 0;
  for(int k = p; k < r+1; k++){
    if (num == 1){
      if (numcmp(Left[i], Right[j])){
        A[k] = Left[i];
        i++;
      }
      else{
        A[k] = Right[j];
        j++;
      }
    }
    else{
      if (strcmp(Left[i], Right[j]) <= 0){
        A[k] = Left[i];
        i++;
      }
      else{
        A[k] = Right[j];
        j++;
      }
    }
  }
}

int numcmp(char *s1, char *s2) {
    double val1, val2;
    val1 = atof(s1);
    val2 = atof(s2);

    if(val1 <= val2) {
        return 1;
    }
    return 0;
}

void printArray(int size, char *data[]){
    for(int i = 0; i < size; i++){
      if (i < size-1)
        printf("%s, ", data[i]);
      else
        printf("%s", data[i]);
    }
    printf("\n");
}

int getAllValues(char* data[]){
    int total = 0;
    int len = 0;
    char *p, line[MAXCHARS];
    while((len = getValue(line)) > 0 && total < MAXCHARS && (p = (char*) malloc(len)) != NULL) {
        strcpy(p, line);
        data[total++] = p;
    }
    return total; 
}

int getValue(char line[]){
    char c;
    int len = 0;
    while((c=getchar())!=EOF && c != '\n' && len < MAXCHARS-1){
        line[len++] = c;
    }
    if(c=='\n'){
        line[len] = 0;
    }
    return len;
}