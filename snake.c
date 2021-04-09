#include <ncurses.h>
#include <unistd.h>
#include <sys/select.h>
#include <locale.h>
#include <stdlib.h>

#define max_x 80
#define max_y 40
#define maxl 50    //length of snake to win game
#define wall "\u2592"
#define lag 100000

void initiate(int snake[maxl][2]);
int kbhit(void);
int setdirection(int input);
void buildwalls();
void moves(int snake[maxl][2], int length, int directionx, int directiony);
void printobjects(int snake[maxl][2], int length);
void food(int newy, int newx, int *length, int *foody, int *foodx, int *speedfactor);
int loose(int newy, int newx);
void showend(int score);


int main(){
    setlocale(LC_ALL, "");

    int input = 0;
    int directiony = 0;
    int directionx = 1;
    int speedfactor = 1;
    int foody = (random() % max_y) + 1;
    int foodx = (random() % max_x) + 1;

    int snake[maxl][2]; //Position of each snake part
    int length = 3;
    initiate(snake);

    //keep refreshing the screen until loosing condition is met
    while(loose(snake[0][0], snake[0][1])) {
        erase();
        buildwalls();
        moves(snake, length, directiony, directionx);
        food(snake[0][0], snake[0][1], &length, &foody, &foodx, &speedfactor);
        printobjects(snake, length);
        refresh();
        usleep(lag/speedfactor);

        if (kbhit()) {
            input = getch();
            switch (input) {
                case KEY_UP:
                    directiony = -1;
                    directionx = 0;
                    break;
                case KEY_RIGHT:
                    directiony = 0;
                    directionx = 1;
                    break;
                case KEY_DOWN:
                    directiony = 1;
                    directionx = 0;
                    break;
                case KEY_LEFT:
                    directiony = 0;
                    directionx = -1;
                    break;
            }
        }
    }

    showend(length);
};


//sets up ncurses and resets position for snake
void initiate(int snake[maxl][2]){

    initscr();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(FALSE);

    snake[0][0] = 6;
    snake[0][1] = 6;
    snake[1][0] = 6;
    snake[1][1] = 5;
    snake[2][0] = 6;
    snake[2][1] = 4;
}

//Checks for keystroke during the sleep interval
int kbhit(void) {
    struct timeval tv;
    fd_set read_fd;

    tv.tv_sec=0;
    tv.tv_usec=0;
    FD_ZERO(&read_fd);
    FD_SET(0,&read_fd);

    if(select(1, &read_fd, NULL, NULL, &tv) == -1)
        return 0;

    if(FD_ISSET(0,&read_fd))
        return 1;
    return 0;
}

//prints the walls for the game
void buildwalls(){
    for(int i = 0; i<= max_x; i++){
        mvprintw(0, i, wall);
        mvprintw(max_y, i, wall);
    }

    for(int i = 0; i< max_y; i++){
        mvprintw(i, 0, wall);
        mvprintw(i, max_x, wall);
    }
}

//updates the movement array in the game
void moves(int snake[maxl][2], int length, int directionx, int directiony){
    //moving head one block forward
    int oldy = snake[0][0];
    int oldx = snake[0][1];
    int sparey = 0;
    int sparex = 0;

    snake[0][0] += directionx;
    snake[0][1] += directiony;
    //moving all the old blocks to the new Position
    for(int i = 1; i < length; i++){
        sparey = snake[i][0];
        sparex = snake[i][1];

        snake[i][0] = oldy;
        snake[i][1] = oldx;

        oldy = sparey;
        oldx = sparex;
    }
}


//prints the snake
void printobjects(int snake[maxl][2], int length){
    for(int i = 0; i < length; i++){
        mvprintw(snake[i][0], snake[i][1], "o");
    }
}

//checks if food has been eaten if yes sets up a new random location
void food(int newy, int newx, int *length, int *foody, int *foodx, int *speedfactor){
    if(newy == *foody && newx == *foodx){
        *length += 1;
        *foody = (random() % max_y) + 1;
        *foodx = (random() % max_x) + 1;
        if(!(*length % 6)){
            *speedfactor += 1;
        }
    }
    mvprintw(*foody, *foodx, "@");
};

//checks for loose conditions
int loose(int newy, int newx){
    if(newy == 0 || newx == 0){
        return 0;
    }else if(newy == max_y || newx == max_x){
        mvprintw(max_y, max_x+15, "NANI? WHAT IS THIS POWER?");
        refresh();
        sleep(1);
    }
    return 1;
}

//shows the end screen
void showend(int score){
    clear();
    mvprintw(2,100,"Thanks for playing! Your score was: %d", score);
    getch();
    endwin();
}
