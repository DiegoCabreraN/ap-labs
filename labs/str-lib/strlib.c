#include <stdlib.h>

int mystrlen(char *str){
    int c = 0;
    while (str[c] != '\0'){
        c++;
    }
    return c;
}

char *mystradd(char *origin, char *addition){
    int originLength = mystrlen(origin);
    int additionLength = mystrlen(addition);
    char *newWord = (char *)malloc(originLength+additionLength);
    for (int i = 0; i < originLength; i++){
        newWord[i] = origin[i];
    }
    for (int j = 0; j < additionLength; j++){
        newWord[originLength+j] = addition[j];
    }
    return newWord;
}

int mystrfind(char *origin, char *substr){
    int substrLength = mystrlen(substr);
    int originLength = mystrlen(origin);

    if (substrLength > originLength){
        return -1;
    }
    
    int found = -1;

    for(int i = 0; i < originLength; i++){
        if (origin[i] == substr[0]){
            for(int j = 0; j < substrLength; j++){
                if (origin[i+j] == substr[j]){
                    found = i;
                }
            }
        }
    }

    return found;
}
