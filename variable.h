
float refereeColor[3] = {0.0, 1.0, 0.0}; // Green

float team1Colors[4][3] = {{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}}; // Red Team
float team2Colors[4][3] = {{0, 0.0, 1.0}, {0, 0.0, 1.0}, {0, 0.0, 1.0}, {0, 0.0, 1.0}}; // blue Team

float efforts[2][4] = {{1,0.8,0.5,0.1},{1,0.8,0.5,0.1}};

int elapsedTime = 0;
int finishTime = 10;
float positionOffset = 0.0f; 
float target = 0.0f;

int team1Wins = 0;
int team2Wins = 0;
float space = 0.18;

float move_speed = 0.03; 	// Speed of movement

float playerSpacing = 0.13; // Space between players
float firstPlayerPos = -0.2; // First player's position (left side)
float headSize = 0.07; // Scaled head size

typedef struct {
    float target;
    int team1Wins;
    int team2Wins;
    int elapsedTime;
    float efforts[2][4];
} GameState;