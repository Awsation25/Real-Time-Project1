#include "header.h"
#include "variable.h"

void calculateEffort(int energy[2][4], float effort[2][4]);
float calculateTarget(float efforts[2][4]);
int checkMiddleLineCrossing();
void readFromPLayers(int f);
void writeToDraw(int f);
void createPlayers();
void terminatePlayers();


pid_t players[2][4];

GameState game;

char *fifo = "/tmp/cal_fifo";
char *fifoWithDraw = "/tmp/cal_fifo2";


char message[BUFSIZ];
int energy[2][4];
//float efforts[2][4];
pid_t players[2][4];
pid_t draw;
time_t start_time;
int won;



int main(int argn , char ** args){

    remove(fifo);
    if (access(fifo, F_OK) == -1) {
        if (mkfifo(fifo, 0666) == -1) {
            perror("mkfifo error");
            exit(1);
        }
    }

    remove(fifoWithDraw);
    if (access(fifoWithDraw, F_OK) == -1) {
        if (mkfifo(fifoWithDraw, 0666) == -1) {
            perror("mkfifo error");
            exit(1);
        }
    }

    draw = fork();
    if (draw == 0){
        execlp("./draw","draw",NULL);
        perror("execute draw error");
        exit(-1);
    }

     start_time = time(NULL);


   createPlayers();
    

    int f1 = open(fifo, O_RDONLY | O_NONBLOCK);
    int f2;

    sleep(1);
while(1){
    readFromPLayers(f1);    
calculateEffort(energy,game.efforts);
game.target = calculateTarget(game.efforts);
printf("\tthe target is: %f\n",game.target);
won = checkMiddleLineCrossing();
printf("the win is: %d\n",won);
writeToDraw(f2);

if (game.team1Wins == 3 || game.team2Wins ==3){
    sleep(5);
    close(f1);
    close(f2);
        exit(1);
    }




    sleep(1);

    }

    close(f1);
    close(f2);
  
    

    return 0;


}

void writeToDraw(int f){

    if ((f = open(fifoWithDraw, O_RDWR | O_NONBLOCK)) == -1) {
        perror("fifo open error");
        exit(-3);
    }

    else {
        time_t now = time(NULL);
        game.elapsedTime = (int)(now - start_time);;
        printf("[Parent] Sent time: %d\n", game.elapsedTime);
        sprintf(message,"%d:%d:%f:%d:%d:%f:%f,%f,%f,%f,%f,%f,%f,%f",game.team1Wins,game.team2Wins,game.target,game.elapsedTime,won,positionOffset ,game.efforts[0][0], game.efforts[0][1], game.efforts[0][2], game.efforts[0][3], game.efforts[1][0], game.efforts[1][1], game.efforts[1][2], game.efforts[1][3]); 

        if(write(f,message,sizeof(message))==-1){
            perror("write to draw error ");
            exit(-2);
        }
    }

}

void readFromPLayers(int f){
    char message_r[BUFSIZ];
    for (int i = 0; i < 8; i++)
{
    /* code */


    if(f == -1)
    {
        perror("fifo open error in child");
        exit(-3);
    }

    else {
        if(read(f,message_r,sizeof(message_r))==-1){
            perror("read error");
            exit(-2);
        }
    }



    if(strcmp(message_r,"")==0){
        printf("the referee not recive any thing\n");
    }
    else{
        int eng;
        pid_t childID;
        sscanf(message_r, "%d:%d", &eng, &childID);

      //  printf("i am the parent and i recive %d from %d\n",eng,childID);
        if (i < 4)
        energy[0][i%4] = eng;
        else
        energy[1][i%4] = eng;

    }

}
    
}


void calculateEffort(int energy[2][4], float effort[2][4]) {
    for (int team = 0; team < 2; team++) {
        int idx[4] = {0, 1, 2, 3};
        int tempEffort[4];

        // Sort indexes based on energy values (descending)
        for (int i = 0; i < 3; i++) {
            for (int j = i + 1; j < 4; j++) {
                if (energy[team][idx[i]] < energy[team][idx[j]]) {
                    int temp = idx[i];
                    idx[i] = idx[j];
                    idx[j] = temp;
                }
            }
        }

        // Calculate raw effort (energy * weight)
        for (int rank = 0; rank < 4; rank++) {
            int playerIndex = idx[rank];
            tempEffort[playerIndex] = energy[team][playerIndex] * (4 - rank);
        }

        // Find max effort for normalization
        int maxEffort = tempEffort[0];
        for (int i = 1; i < 4; i++) {
            if (tempEffort[i] > maxEffort)
                maxEffort = tempEffort[i];
        }

        // Normalize efforts, avoid division by zero
        if (maxEffort == 0) {
            for (int i = 0; i < 4; i++) {
                effort[team][i] = 0.0f;
            }
        } else {
            for (int i = 0; i < 4; i++) {
                effort[team][i] = (float)tempEffort[i] / maxEffort;
            }
        }
    }
}

int checkMiddleLineCrossing() {

   if (game.target < 0) // If the rectangle reaches the right side, reset its position
   positionOffset -= move_speed;
   else if (game.target > 0)
   positionOffset += move_speed; 

    float leftPlayerX  = firstPlayerPos - space + positionOffset;

        float rightPlayerX = - firstPlayerPos + space + positionOffset;  // Team 2 players


        // Check if Team 1 player crosses the middle line (including the circumference)
        if (leftPlayerX + 0.05 >= 0  || (game.elapsedTime > finishTime) && (positionOffset > 0)) {
            start_time = time(NULL);
            game.team2Wins++;
            terminatePlayers();
            createPlayers();
            positionOffset = 0;
            sleep(1);
            return 2;
 
            
        }

        // Check if Team 2 player crosses the middle line
       else if (rightPlayerX - 0.05<= 0 || (game.elapsedTime > finishTime) && (positionOffset < 0)) {
        start_time = time(NULL);
        game.team1Wins++;
        terminatePlayers();
        createPlayers();
        positionOffset = 0;
        sleep(1);
            return 1;
            
 

        }

        else{
            return 0;

        }

}

float calculateTarget(float efforts[2][4]){
    float effort[2] = {0,0};
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            effort[i] += efforts[i][j];
        }
        
    }
    return (float)(effort [1] - effort [0]) / 10;
}

// Function to create child processes (players)
void createPlayers() {
    for (int i = 0; i < 4; i++) {
        // Create players for team 1
        players[0][i] = fork();
        if (players[0][i] == 0) {
            // Child process for team 1
            execlp("./players", "player", "20", "50", NULL);
            perror("execlp error for team 1");
            exit(-5);  // Exit if execlp fails
        }

        // Create players for team 2
        players[1][i] = fork();
        if (players[1][i] == 0) {
            // Child process for team 2
            execlp("./players", "player", "1", "20", NULL);
            perror("execlp error for team 2");
            exit(-5);  // Exit if execlp fails
        }
    }
}

// Function to terminate child processes
void terminatePlayers() {
    for (int i = 0; i < 4; i++) {
        if (players[0][i] > 0) {
            kill(players[0][i], SIGTERM);  // Send SIGTERM to team 1 players
        }

        if (players[1][i] > 0) {
            kill(players[1][i], SIGTERM);  // Send SIGTERM to team 2 players
        }
    }
}