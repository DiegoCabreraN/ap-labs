#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int mystrlen(char *str);
int mystrfind(char *origin, char *substr);
char *mystradd(char *origin, char *addition);

int main(int argc, char* argv[]) {
    if (strncmp(argv[1], "-add", 4) == 0){
        printf("ADD");
        int initialLength = mystrlen(argv[2]);
        printf("Initial Length: %d\n", initialLength);
        char *newStr = mystradd(argv[2], argv[3]);
        printf("New String: %s\n", newStr);
        int newLentgth = mystrlen(newStr);
        printf("New Length: %d\n", newLentgth);

        free(newStr);
    }
    else if(strncmp(argv[1], "-find", 4) == 0){
        int wasFound = mystrfind(argv[2], argv[3]);
        if (wasFound >= 0){
            printf("['%s'] string was found at [%d] position\n", argv[3], wasFound);
        }
        else{
            printf("Substring not found");
        }
    }
    else{
        printf("There was an error with the arguments, please try again\n");
    }
    return 0;
}
