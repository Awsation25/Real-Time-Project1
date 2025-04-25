#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include "header.h"
#include "variable.h"

char *fifoWithDraw = "/tmp/cal_fifo2";


int won =0;
int f2;


void readFromFIFO() {
    char message_r[BUFSIZ];

    if(f2 == -1)
    {
        perror("fifo open error in draw");
        exit(-3);
    }

    else {
        if(read(f2,message_r,sizeof(message_r))==-1){
            perror("read draw error");
            exit(-2);
        }
    }



    if(strcmp(message_r,"")==0){
        printf("the draw not recive any thing\n");
    }
    else{

        sscanf(message_r, "%d:%d:%f:%d:%d:%f:%f,%f,%f,%f,%f,%f,%f,%f\n",
            &team1Wins, &team2Wins, &target, &elapsedTime, &won,&positionOffset,
            &efforts[0][0], &efforts[0][1], &efforts[0][2], &efforts[0][3],
             &efforts[1][0], &efforts[1][1], &efforts[1][2], &efforts[1][3]);
         // elapsedTime = atoi(buffer); // Update global time
       //  printf("[OpenGL] Received : %s\n", message_r);
         glutPostRedisplay();

    }


    glutTimerFunc(0, readFromFIFO, 0); // Check every 100ms
}


// void timer(int value)
// {
//     glutPostRedisplay();
//     glutTimerFunc(1000 / 6, timer, 0);


//    // Update the x position of the rectangle
//    if (target < 0) // If the rectangle reaches the right side, reset its position
//    positionOffset -= move_speed;
//    else if (target > 0)
//    positionOffset += move_speed;


	
// }

// void updateTimer(int value) {
//     elapsedTime++;
//    // signal(SIGALRM,signal_handler);
//     //pause(); 
//     glutPostRedisplay();  // Request a redraw
//     glutTimerFunc(1000, updateTimer, 0); // Call again after 1 second
// }


// Function to sort efforts in ascending order
void sortEfforts() {
    for (int team = 0; team < 2; team++) { // Loop through both teams
        for (int i = 0; i < 3; i++) { // Bubble Sort (only 4 elements)
            for (int j = 0; j < 3 - i; j++) {
                if (efforts[team][j] > efforts[team][j + 1]) {
                    // Swap values
                    float temp = efforts[team][j];
                    efforts[team][j] = efforts[team][j + 1];
                    efforts[team][j + 1] = temp;
                }
            }
        }
    }
}


// Function to update player colors based on brightness values
void updateColor() {
    sortEfforts();
    for (int i = 0; i < 4; i++) { // Loop through each player
        for (int j = 0; j < 3; j++) {
            if (j == 0) {
                team1Colors[i][j] = 1; // Original red
            } else {
                team1Colors[i][j] = 1.0 - efforts[0][i];
            }

            if (j == 2) {
                team2Colors[i][j] = 1; // Original blue
            } else {
                team2Colors[i][j] = 1.0 - efforts[1][i];
            }
        }
    }
    
    glutPostRedisplay(); // Refresh the screen
    glutTimerFunc(1000, (void (*)(int))updateColor, 0); // Call again after 1 second
}

void resetGame() {
    positionOffset = 0.0f;  // Reset player positions
    elapsedTime = 0;        // Reset timer

    // Reset efforts (you can randomize efforts here if needed)
    float newEfforts[2][4] = {{1, 0.8, 0.5, 0.1}, {1, 0.8, 0.5, 0.1}};
    for (int team = 0; team < 2; team++) {
        for (int i = 0; i < 4; i++) {
            efforts[team][i] = newEfforts[team][i];
        }
    }

    // Reset colors
    for (int i = 0; i < 4; i++) {
        team1Colors[i][0] = 1.0;
        team1Colors[i][1] = 0.0;
        team1Colors[i][2] = 0.0; // Red Team
        
        team2Colors[i][0] = 0.0;
        team2Colors[i][1] = 0.0;
        team2Colors[i][2] = 1.0; // Blue Team
    }

    // Reset referee color to green
    refereeColor[0] = 0.0;
    refereeColor[1] = 1.0;
    refereeColor[2] = 0.0;

    glutPostRedisplay();
}



// Function to draw text on the screen
void drawText(float x, float y, char *string) {
    glRasterPos2f(x, y);
    while (*string) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *string);
        string++;
    }
}

void displayScore(int team1Wins, int team2Wins) {
    char scoreText[50];
    sprintf(scoreText, "Team 1: %d  |  Team 2: %d", team1Wins, team2Wins);

    // **Draw background box**
    glColor3f(0.9, 0.9, 0.9); // Light gray background
    glBegin(GL_QUADS);
    glVertex2f(-0.3, 0.75);
    glVertex2f( 0.3, 0.75);
    glVertex2f( 0.3, 0.85);
    glVertex2f(-0.3, 0.85);
    glEnd();

    // **Draw border**
    glColor3f(0, 0, 0); // Black border
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-0.3, 0.75);
    glVertex2f( 0.3, 0.75);
    glVertex2f( 0.3, 0.85);
    glVertex2f(-0.3, 0.85);
    glEnd();

    // **Draw text**
    glColor3f(0, 0, 0); // Black text
    glRasterPos2f(-0.25, 0.75); // Position text at the center
    for (int i = 0; scoreText[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreText[i]);
    }
}


int checkMiddleLineCrossing() {
    float leftPlayerX  = firstPlayerPos - space + positionOffset;

        float rightPlayerX = - firstPlayerPos + space + positionOffset;  // Team 2 players

        // Check if Team 1 player crosses the middle line (including the circumference)
        if (leftPlayerX + 0.05 >= 0) {
            refereeColor[0] = 0.0; // Change referee color to Red (alert)
            refereeColor[1] = 0.0; // Yellow mix
            refereeColor[2] = 1.0;
          //  Mix_PlayChannel(-1, winningSound, 0); // Play sound for blue team win

           
            drawText(-0.1, 0.4, "Blue Wins"); 
            return 2;
 
            
        }

        // Check if Team 2 player crosses the middle line
       else if (rightPlayerX - 0.05 <= 0) {
            refereeColor[0] = 1.0; // Change referee color to Red (alert)
            refereeColor[1] = 0.0; 
            refereeColor[2] = 0.0;

            drawText(-0.1, 0.4, "red Wins"); 
            return 1;
            
 

        }

        else if (elapsedTime > finishTime && positionOffset > 0){

            refereeColor[0] = 0.0; // Change referee color to Red (alert)
            refereeColor[1] = 0.0; // Yellow mix
            refereeColor[2] = 1.0;
          //  Mix_PlayChannel(-1, winningSound, 0); // Play sound for blue team win

           
            drawText(-0.1, 0.4, "Blue Wins"); 
            return 2;

 
       }

        else if (elapsedTime > finishTime && positionOffset < 0){

             refereeColor[0] = 1.0; // Change referee color to Red (alert)
            refereeColor[1] = 0.0; 
            refereeColor[2] = 0.0;

            drawText(-0.1, 0.4, "red Wins");  
            return 1;
            

 
        }

        else{
            refereeColor[0] = 0.0; // Change referee color to Red (alert)
            refereeColor[1] = 1.0; 
            refereeColor[2] = 0.0;
            return 0;

        }

  
    
}


// Function to draw a circle (player head)
void drawCircle(float x, float y, float radius) {
    int num_segments = 100;
    glBegin(GL_POLYGON);
    for (int i = 0; i < num_segments; i++) {
        float angle = 2.0f * 3.1415926f * i / num_segments;
        glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
    }
    glEnd();
}

// Function to draw a player, scaled for a window size of 800x500
void drawPlayer(float x, float y, float r, float g, float b) {

    glColor3f(r, g, b); // Set team color
    drawCircle(x, y, headSize); // Draw head

    // Eyes
    glColor3f(0, 0, 0); // Black for eyes
    drawCircle(x - (headSize * 0.3), y + (headSize * 0.3), headSize * 0.2);
    drawCircle(x + (headSize * 0.3), y + (headSize * 0.3), headSize * 0.2);

    // Mouth (simple line)
    glBegin(GL_LINES);
    glVertex2f(x - (headSize * 0.3), y - (headSize * 0.2));
    glVertex2f(x + (headSize * 0.3), y - (headSize * 0.2));
    glEnd();
}


// Function to draw the middle vertical line (between teams)
void drawMiddleLine() {
    glColor3f(0, 0, 0); // Black color for the line
    glBegin(GL_LINES);
    glVertex2f(0, -0.3); // Bottom point (same height as players)
    glVertex2f(0, 0.1);  // Top point (same height as players' heads)
    glEnd();
}

void drawLine(float centerX1, float centerY1, float centerX2, float centerY2)
{
	glColor3f(0.6, 0.5, 0.2); // Set color to black
	glBegin(GL_LINES);
	glVertex2f(centerX1, centerY1);
	glVertex2f(centerX2, centerY2);
    //gluOrtho2D(centerX1, centerY1, centerX2, centerY2);
	glEnd();
}


// Function to move players using keyboard input
void keyboard(int key, int x, int y) {
    if (key == GLUT_KEY_LEFT) {
        positionOffset -= 0.05; // Move left
    }
    if (key == GLUT_KEY_RIGHT) {
        positionOffset += 0.05; // Move right
    }
    glutPostRedisplay();
}

// Display function (Modified to show sorted effort values)
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw middle line (rope divider)
    drawMiddleLine();
    drawLine(-0.8 + positionOffset, -0.1, 0.8 + positionOffset, -0.1);

        // **Display Score with Beautiful Background**

    int won = checkMiddleLineCrossing();
    displayScore(team1Wins, team2Wins);

 
            // **Display Timer (Top-right corner)**
    char timerText[20];
    sprintf(timerText, "Time: %d s", elapsedTime);
    glColor3f(0, 0, 0); // Black color for timer text
    drawText(0.6, 0.8, timerText); // Position at top-right corner


    char effortText[10]; // Buffer for displaying effort values

    // Team 1 (Red)
    for (int i = 0; i < 4; i++) {
        float playerX = firstPlayerPos - (i * playerSpacing) - space + positionOffset;

        drawPlayer(playerX, -0.1, team1Colors[i][0], team1Colors[i][1], team1Colors[i][2]);

        // Convert float to string and display near player
        sprintf(effortText, "%.1f", efforts[0][i]); 
        drawText(playerX, -0.2, effortText);
    }

    // Team 2 (Blue)
    for (int i = 0; i < 4; i++) {
        float playerX = -firstPlayerPos + (i * playerSpacing) + space + positionOffset;

        drawPlayer(playerX, -0.1, team2Colors[i][0], team2Colors[i][1], team2Colors[i][2]);

        // Convert float to string and display near player
        sprintf(effortText, "%.1f", efforts[1][i]); 
        drawText(playerX, -0.2, effortText);
    }

    // Referee
    drawPlayer(0, 0.3, refereeColor[0], refereeColor[1], refereeColor[2]);


    glutSwapBuffers();

    if (won != 0){
        sleep(2);

        glutTimerFunc(0, (void (*)(int))resetGame, 0); // Restart after 2 seconds
            // Sort efforts before displaying
        sortEfforts();

    }

  
    if (team1Wins == 3 || team2Wins ==3){
        sleep(5);
            exit(1);
        }

}


// Initialization
void init() {
    glClearColor(1, 1, 1, 1); // White background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1); // Set coordinate system
}

// Main function
int main(int argc, char **argv) {

    f2 = open(fifoWithDraw, O_RDONLY);

    if(f2 == -1)
    {
        perror("fifo open error in draw");
        exit(-3);
    }
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 500);
    glutCreateWindow("Tug of War - Players Lightening Over Time");

    init();
    sortEfforts();  
    glutDisplayFunc(display);
    glutSpecialFunc(keyboard); // Handle left/right arrow key movement
    srand(time(NULL)); // Call this once at the start


    // Start timer function
    glutTimerFunc(0, updateColor, 0); 
 //  glutTimerFunc(1000, updateTimer, 0); // Start the timer function
   // glutTimerFunc(3000, updateEfforts, 0); 
   // glutTimerFunc(0, timer, 0);
    glutTimerFunc(0, readFromFIFO, 0); // Start FIFO polling




    glutMainLoop();
    return 0;
}
