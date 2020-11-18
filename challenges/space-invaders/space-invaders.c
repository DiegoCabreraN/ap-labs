#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


int DEBUG = 0;
int RUNNING;
int Xpos = 17;
int lives = 1;
int score = 0;

int board[31][12];

void initBoard();
void drawBoard();
void drawUI(int debug);
void *playerBulletThread(void *args);
void *playerMonitor(void *args);


int main(int argc, char **argv){
    if(argc > 1){
        DEBUG = 1;
    }
    //Start game
    RUNNING = 1;
    
    pthread_t monitor;
    pthread_create(&monitor, NULL, playerMonitor, NULL);

    initBoard();
    drawUI(DEBUG);
    while(RUNNING){}
    printf("GAME OVER!\n");

    return 0;
}

void initBoard(){
    for(int i = 0; i<31; i++)
        for(int j = 0; j<12; j++)
            board[i][j] = 0;
    board[16][0] = 2;
    board[Xpos-1][11] = 1;
}

void drawBoard(){
    printf("\n\n|-------------------------------|\n");
    for(int j = 0; j<12; j++){
        printf("|");
        for(int i = 0; i<31; i++){
            printf("%d", board[i][j]);
        }
        printf("|\n");
    }
    printf("|-------------------------------|\n");
}

void drawUI(int debug){
    system("clear");
    printf("\n|-------------------------------|\n");
    printf("|         Space Invaders        |\n");
    printf("|-------------------------------|\n");
    printf("|Lives: %d\tScore: %d\t|\n", lives, score);
    printf("|-------------------------------|\n");
    for(int j = 0; j<12; j++){
        printf("|");
        for(int i = 0; i<31; i++){
            if(board[i][j] == 1)
                printf("^");
            else if(board[i][j] == 2)
                printf("T");
            else if(board[i][j] == 3)
                printf("|");
            else
                printf(" ");
        }
        printf("|%d\n",j);
    }
    printf("|-------------------------------|\n");
    if (debug)
        drawBoard();
}

void *playerBulletThread(void *args){
    int bulletYpos = 10;
    int bulletXpos = *(int *)args;
    while(bulletYpos > -1){
        if(board[bulletXpos][bulletYpos] != 2){
            board[bulletXpos][bulletYpos] = 3;
            drawUI(DEBUG);
            sleep(1);
            
            board[bulletXpos][bulletYpos] = 0;
            bulletYpos--;
            if(bulletYpos > 0)
                board[bulletXpos][bulletYpos] = 3;
        }
        else{
            score++;
            board[bulletXpos][bulletYpos] = 0;
            bulletYpos--;
        }
        
    }
    drawUI(DEBUG);
    
    pthread_exit(NULL);
}

void *enemyBulletThread(void *args){
    int* pos = (int *)args;
    int bulletYpos = pos[1]+1;
    int bulletXpos = pos[0];
    while(bulletYpos < 12){
        board[bulletXpos][bulletYpos] = 3;
        drawUI(DEBUG);
        sleep(1);
        
        board[bulletXpos][bulletYpos] = 0;
        bulletYpos++;
        if(board[bulletXpos][bulletYpos] == 1){
            if(lives > 1)
                lives--;
            else{
                lives--;
                board[bulletXpos][bulletYpos] = 0;
            }
            break;
        }
        if(bulletYpos < 12)
            board[bulletXpos][bulletYpos] = 3;
        
    }
    drawUI(DEBUG);
    if(lives==0){
        RUNNING = 0;
    }
    
    
    pthread_exit(NULL);
}

void *playerMonitor(void *args){
    while(RUNNING){
        if(lives < 1){
            RUNNING = 0;
            break;
        }

        //Controls
        system("stty -icanon");
        char letter = getchar();
        if(letter == 'a' && Xpos>1){
            if(board[Xpos-2][11] == 3){
                if(lives > 1)
                    lives--;
                else{
                    lives--;
                    board[Xpos-1][11] = 0;
                }
            }
            //Free Place on board
            board[Xpos-1][11] = 0;
            Xpos--;
            //Ocupied by User
            board[Xpos-1][11] = 1;
        } else if(letter == 'd' && Xpos<31){
            if(board[Xpos][11] == 3){
                if(lives > 1)
                    lives--;
                else{
                    lives--;
                    board[Xpos-1][11] = 0;
                }
            }
            //Free Place on board
            board[Xpos-1][11] = 0;
            Xpos++;
            //Ocupied by User
            board[Xpos-1][11] = 1;
        } else if(letter == ' ' && board[Xpos-1][10] != 3){
            pthread_t bullet;
            int pos = Xpos - 1;
            pthread_create(&bullet, NULL, playerBulletThread, (void *)&pos);
        }else if(letter == 'c' && board[Xpos-1][1] != 3){
            pthread_t bullet;
            int pos[2] = {16, 0};
            pthread_create(&bullet, NULL, enemyBulletThread, (void *)&pos);
        }
        system("stty cooked");
        drawUI(DEBUG);
    }
    return NULL;
}
