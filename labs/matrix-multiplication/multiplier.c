#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "logger.h"

#define MATRIX_DIM 2000
#define NUM_THREADS 2000

int NUM_BUFFERS;
char *RESULT_MATRIX_FILE;
long *result;
long **buffers;

pthread_mutex_t *mutexes;
pthread_t threads[NUM_THREADS];

struct threadArgs{
    int row;
    int col;
    long *matA;
    long *matB;
};

int getLock();
int releaseLock(int lock);
int saveResultMatrix(long *result);
void raiseUsageError();
long dotProduct(long *vect1, long *vect2);
long *readMatrix(char *filename);
long *getColumn(int col, long *matrix);
long *getRow(int row, long *matrix);
long *multiply(long *matA, long *matB);

int main(int argc, char** argv){
    if(argc != 5){
        raiseUsageError();
    }
    
    for(int i = 1; i<4; i+=2){
        if(strcmp(argv[i], "-n") == 0){
            NUM_BUFFERS = atoi(argv[i+1]);
        }else if(strcmp(argv[i], "-out") == 0){
            RESULT_MATRIX_FILE = argv[i+1];
        }else{
            raiseUsageError();
        }
    }

    if(NUM_BUFFERS < 2){
        panicf("Error, the Buffer Size is too Small");
    }
 
    buffers = malloc(NUM_BUFFERS * sizeof(long *));
    mutexes = malloc(NUM_BUFFERS * sizeof(pthread_mutex_t));
    for(int i = 0; i<NUM_BUFFERS; i++)
        pthread_mutex_init(&mutexes[i], NULL);
    
    long *matA, *matB;
    infof("Reading Input Matrix");
    matA = readMatrix("./matA.dat");
    matB = readMatrix("./matB.dat");

    infof("Starting Multiplication");
    result = malloc(MATRIX_DIM*MATRIX_DIM * sizeof(long));
    result = multiply(matA, matB);

    infof("Saving Result");
    int saved = saveResultMatrix(result);

    free(buffers);
    free(mutexes);
    free(matA);
    free(matB);
    free(result);    

    if(saved != 0){
        panicf("There was an error while saving the file");
    }
       
    return 0;
}

int getLock(){
    for(int i = 0; i<NUM_BUFFERS; i++){
        if(pthread_mutex_trylock(&mutexes[i]) == 0){
            return i;
        }
    }
    return -1;
}
int releaseLock(int lock){
    return pthread_mutex_unlock(&mutexes[lock]);
}
int saveResultMatrix(long *result){
    int fd;
    fd = open(RESULT_MATRIX_FILE, O_CREAT|O_RDWR|O_APPEND|O_TRUNC, 
                                  S_IRUSR|S_IWUSR);
    if(fd < 0){
        panicf("Matrix file %s can't be created");
    }
    for(int i = 0; i < MATRIX_DIM*MATRIX_DIM; i++){
        char* var = malloc(100000 * sizeof(char));
        sprintf(var, "%lu\n", result[i]);
        if(write(fd, var, strlen(var))!= strlen(var)){
            errorf("Can't write to file");
            return -1;
        }
    }
    return 0;

    close(fd);
}
void raiseUsageError(){
    panicf("Usage ./multiplier -n <NUMBER> -out <DESTINATION>");
}
long dotProduct(long *vect1, long *vect2){
    long localResult = 0;
    for(int i = 0; i < MATRIX_DIM; i++){
        localResult += vect1[i] * vect2[i];
    }
    return localResult;
}
long multiplication(struct threadArgs *args){
    
    int bufA = -1;
    int bufB = -1;

    while (bufA == -1 || bufB == -1){
        if(bufA < 0){
            bufA = getLock();
        }
        else{
            bufB = getLock();
        }
    }
    
    buffers[bufA] = getRow(args->row, args->matA);
    buffers[bufB] = getColumn(args->col, args->matB);
    
    long res = dotProduct(buffers[bufA], buffers[bufB]);
    free(buffers[bufA]);
    free(buffers[bufB]);
    releaseLock(bufA);
    releaseLock(bufB);
    return res;
}
long *readMatrix(char *filename){
    long* matrix = (long *)malloc(MATRIX_DIM*MATRIX_DIM * sizeof(long));

    int fd, n;
    if((fd = open(filename, O_RDONLY)) == -1){
        panicf("Matrix file %s was not found on system");
    }

    int actualPos = 0;
    char actualChar;
    int concatPos = 0;
    char *valueConcat = (char *)malloc(sizeof(long) * sizeof(char));

    while((n = read(fd, &actualChar, 1) > 0)){
        if(actualChar != '\n'){
            valueConcat[concatPos++] = actualChar;
        }
        else{
            valueConcat[concatPos++] = '\0';
            concatPos = 0;
            matrix[actualPos++] = atol(valueConcat);
        }
    }
    if(concatPos > 0){
        matrix[actualPos++] = atol(valueConcat);
    }
    close(fd);
    return matrix;    
}
long *getColumn(int col, long *matrix){
    long * values = (long *)malloc( MATRIX_DIM*sizeof(long));
    long actualPos = 0;
    for(int i = col; i<MATRIX_DIM*MATRIX_DIM; i+=MATRIX_DIM)
        values[actualPos++] = matrix[i];
    return values;
}
long *getRow(int row, long *matrix){
    long * values = (long *)malloc( MATRIX_DIM*sizeof(long));
    long actualPos = 0;
    for(int i = MATRIX_DIM*row; i<MATRIX_DIM*row+MATRIX_DIM; i++)
        values[actualPos++] = matrix[i];
    return values;
}
long *multiply(long *matA, long *matB){
    long *newMatrix = malloc(MATRIX_DIM*MATRIX_DIM * sizeof(long));
    for(int i = 0; i<MATRIX_DIM; i++){
        for(int j = 0; j<MATRIX_DIM; j++){
            struct threadArgs *args = malloc(sizeof(struct threadArgs));
            args->row = i;
            args->col = j;
            args->matA = matA;
            args->matB = matB;
            pthread_create(&threads[j], NULL, (void * (*)(void *))multiplication, (void *)args);
        }
        

        for(int j = 0; j<MATRIX_DIM; j++){
            void *localResult;
            pthread_join(threads[j], &localResult);
            newMatrix[MATRIX_DIM*i+j] = (long)localResult;
        }
    } 
    return newMatrix;
}