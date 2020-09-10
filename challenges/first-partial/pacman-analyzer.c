#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h>
#include <string.h>

#define REPORT_FILE "packages_report.txt"
#define MAX_PACKAGES 1000000

struct row {
    char packageName[257];
    char installDate[17];
    char lastUpdate[17];
    char removalDate[17];
    int updatesCounter;
} database[MAX_PACKAGES];

void analizeLog(char *logFile, char *report);
char *GetPackageName(char contentLine[], int startCharPos, int bufferSize);
char *GetSubstring(char contentLine[], int startCharPos, int endCharPos);
void SaveDataToRow(char *name, int event, char* date);
void PrintGeneralData();
void PrintIndividualData();

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage:./pacman-analizer.o pacman.log\n");
        return 1;
    }

    analizeLog(argv[1], REPORT_FILE);

    return 0;
}

void analizeLog(char *logFile, char *report) {
    printf("Generating Report from: [%s] log file\n", logFile);

    // Implement your solution here.
    int fileStream;
    int file = open(logFile, O_RDONLY);
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
            char *actualState = GetSubstring(contentLine, 26, 34);
            for(int i = 0; i < 3; i++){
                if(strcmp(actualState, stateName[i])==0){
                    int start = i == 0 ? 36 : i == 1? 34 : 35;
                    char *packageName = GetPackageName(contentLine, start, BUFSIZ);
                    char *eventDate = GetSubstring(contentLine, 1, 17);
                    
                    SaveDataToRow(packageName, i, eventDate);
                    
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
    PrintGeneralData();
    PrintIndividualData();

    close(file);
    printf("Report is generated at: [%s]\n", report);
}

char *GetPackageName(char contentLine [], int startCharPos, int bufferSize){
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

char *GetSubstring(char contentLine [], int startCharPos, int endCharPos){
    char *substring = malloc(sizeof(char)*(endCharPos-startCharPos+1));
    for (int i = 0; i<endCharPos-startCharPos; i++){
        substring[i] = contentLine[i+startCharPos];
    }
    substring[endCharPos-startCharPos] = '\0';
    return substring;
}

void SaveDataToRow(char *name, int event, char *date){
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

void PrintGeneralData(){
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
            printf("\nPacman Packages Report\n");
            printf("----------------------\n");
            printf("- Installed packages : %d\n", i);
            printf("- Removed packages   : %d\n", removed);
            printf("- Upgraded packages  : %d\n", updated);
            printf("- Current installed  : %d\n\n", i-removed);
            break;
        }
    }
    
}

void PrintIndividualData(){
    printf("\nList of packages\n");
    printf("----------------\n");
    for (int i = 0; i < MAX_PACKAGES; i++){
        if(strcmp(database[i].packageName, "") != 0){
            printf("- Package Name        : %s\n",database[i].packageName);
            printf("  - Install date      : %s\n",database[i].installDate);
            printf("  - Last update date  : %s\n",database[i].lastUpdate);
            printf("  - How many updates  : %d\n",database[i].updatesCounter);
            printf("  - Removal date      : %s\n",database[i].removalDate);
        }
    }
}