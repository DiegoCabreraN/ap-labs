#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 1000

int main()
{
    char c;
    char word[MAX_SIZE];
    int count = 0;

    while ((c = getchar()) != EOF) {
        if(c == '\n'){
            for (int i = count; i >= 0; i--){
                printf("%c", word[i]);
            }
            printf("\n");
            count = 0;
        }
        else{
            word[count] = c;
            count++;
        }
    }
    printf("\n");
    return 0;
}