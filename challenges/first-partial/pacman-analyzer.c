#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>
#include <string.h>

#define MAX_PACKAGES 1000000

struct row {
    char packageName[257];
    char installDate[17];
    char lastUpdate[17];
    char removalDate[17];
    int updatesCounter;
} database[MAX_PACKAGES];

void analizeLog(char *inFile, char *report);
char *getPackageName(char contentLine[], int startCharPos, int bufferSize);
char *getSubstring(char contentLine[], int startCharPos, int endCharPos);
void saveDataToRow(char *name, int event, char* date);
void writeGeneralData(int file);
void writeIndividualData(int file);
void writeToFile(char *destinationFile);

int main(int argc, char **argv) {
    if (argc < 5 || strcmp(argv[1],"-input") != 0 || strcmp(argv[3],"-report") != 0) {
        printf("Usage:./pacman-analizer.o -input <inputFile> -report <destinationFile>\n");
        return 1;
    }

    analizeLog(argv[2], argv[4]);

    return 0;
}

void analizeLog(char *inFile, char *report) {
    printf("Generating Report from: [%s] log file\n", inFile);

    // Implement your solution here.
    int fileStream;
    int file = open(inFile, O_RDONLY);
    char buffer;

    char *stateName[3] = {"installe", "removed ", "upgraded"};
    
    if (file == -1){
        perror("ERROR 404");
        exit(EXIT_FAILURE);
    }

    char contentLine[BUFSIZ];
    int charPos = 0;

    while ((fileStream = read(file, &buffer, 1)) > 0){
        if (buffer == '\n'){
            char *actualState = getSubstring(contentLine, 26, 34);
            for(int i = 0; i < 3; i++){
                if(strcmp(actualState, stateName[i])==0){
                    int start = i == 0 ? 36 : i == 1? 34 : 35;
                    char *packageName = getPackageName(contentLine, start, BUFSIZ);
                    char *eventDate = getSubstring(contentLine, 1, 17);
                    
                    saveDataToRow(packageName, i, eventDate);
                    
                    free(eventDate);
                    free(packageName);
                    break;
                }
            }
            free(actualState);
            memset(contentLine, 0, sizeof(contentLine));
            charPos = 0;
            continue;
        }
        contentLine[charPos] = buffer;
        charPos++;
    }
    // Print Results
    writeToFile(report);

    close(file);
    printf("Report is generated at: [%s]\n", report);
}

char *getPackageName(char contentLine [], int startCharPos, int bufferSize){
    char *name = malloc(sizeof(char)*100);
    int nextChar = 0;
    for(int i = startCharPos; i<bufferSize; i++){
        if(contentLine[i] == ' '){
            break;
        }
        else{
            name[nextChar++] = contentLine[i];
        }
    }
    name[nextChar] ='\0';
    return name;
}

char *getSubstring(char contentLine [], int startCharPos, int endCharPos){
    char *substring = malloc(sizeof(char)*(endCharPos-startCharPos+1));
    for (int i = 0; i<endCharPos-startCharPos; i++){
        substring[i] = contentLine[i+startCharPos];
    }
    substring[endCharPos-startCharPos] = '\0';
    return substring;
}

void saveDataToRow(char *name, int event, char *date){
    int successFlag = 0;
    for (int i = 0; i < MAX_PACKAGES; i++){
        if(strcmp(database[i].packageName, "") == 0){
            successFlag = 1;
            strcpy(database[i].packageName, name);
            strcpy(database[i].installDate, date);
            strcpy(database[i].lastUpdate, "-");
            strcpy(database[i].removalDate, "-");
            break;
        }
        else if(strcmp(database[i].packageName, name) == 0){
            if (event == 1){
                strcpy(database[i].removalDate, date);
            }
            else if (event == 2){
                strcpy(database[i].lastUpdate, date);
                database[i].updatesCounter += 1;
            }
            else{
                successFlag = 1;
                strcpy(database[i].packageName, name);
                strcpy(database[i].installDate, date);
                strcpy(database[i].lastUpdate, "-");
                strcpy(database[i].removalDate, "-");
                break;
            }
            successFlag = 1;
            break;
        }
    }
    if (successFlag == 0){
        printf("ERROR: There's no more empty space in Database\n");
        exit(EXIT_FAILURE);
    }
}

void writeGeneralData(int file){
    int updated = 0;
    int removed = 0;
    for (int i = 0; i < MAX_PACKAGES; i++){
        if(strcmp(database[i].packageName, "") != 0){
            if(strcmp(database[i].removalDate, "-") != 0){
                removed += 1;
            }
            else if(strcmp(database[i].lastUpdate, "-") != 0){
                updated += 1;
            }
        }
        else{
            char* word = (char*)malloc(sizeof(char)*25);
            
            sprintf(word, "Pacman Packages Report\n");
            write(file, word, strlen(word));
            sprintf(word, "----------------------\n");
            write(file, word, strlen(word));
            sprintf(word, "- Installed packages : %d\n",i);
            write(file, word, strlen(word));
            sprintf(word, "- Removed packages   : %d\n", removed);
            write(file, word, strlen(word));
            sprintf(word, "- Upgraded packages  : %d\n", updated);
            write(file, word, strlen(word));
            sprintf(word, "- Current installed  : %d\n",i-removed);
            write(file, word, strlen(word));

            free(word);
            break;
        }
    }
}

void writeIndividualData(int file){
    char* word = (char*)malloc(sizeof(char)*25);
    sprintf(word,"\nList of packages\n");
    write(file, word, strlen(word));
    sprintf(word,"----------------\n");
    write(file, word, strlen(word));
    for (int i = 0; i < MAX_PACKAGES; i++){
        if(strcmp(database[i].packageName, "") != 0){
            sprintf(word,"- Package Name        : %s\n",database[i].packageName);
            write(file, word, strlen(word));
            sprintf(word,"  - Install date      : %s\n",database[i].installDate);
            write(file, word, strlen(word));
            sprintf(word,"  - Last update date  : %s\n",database[i].lastUpdate);
            write(file, word, strlen(word));
            sprintf(word,"  - How many updates  : %d\n",database[i].updatesCounter);
            write(file, word, strlen(word));
            sprintf(word,"  - Removal date      : %s\n",database[i].removalDate);
            write(file, word, strlen(word));
        }
    }
    free(word);
}

void writeToFile(char *destinationFile){
    int fileStream;
    int file = open(destinationFile, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (file == -1){
        printf("Error while creating destination file\n");
        exit(EXIT_FAILURE);
    }
    writeGeneralData(file);
    writeIndividualData(file);

    close(file);
}