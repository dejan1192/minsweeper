#ifndef GAME_H
#define GAME_H

#include "raylib.h"

#define ROWS  40
#define COLS  40
#define RECT_SIZE 40
#define SCREEN_W 800
#define SCREEN_H 600
#define MINE_PERCENT 15


#define NUM_FRAMES_PER_LINE 5
#define NUM_LINES 5


typedef enum GameStatus {
    PLAY,
    WON,
    LOST,
    SELECTION,
} GameStatus;

typedef void (*CommandFunction)(void);

typedef struct Command {
    CommandFunction func;
} Command;



typedef struct GameRec {
    Rectangle rec;
    bool has_mine;
    bool flagged;
    bool neutralized;
    bool hover;
    bool active;
    bool visited;
} GameRec;

typedef struct {
    GameStatus status;
    GameRec grid[SCREEN_W / ROWS][SCREEN_H / COLS];
    int currentFrame;
    int currentLine;
    Vector2 position;
    int framesCounter;
    float frameWidth;
    float frameHeight;
    Rectangle frameRec;
} Game;

void draw_grid(Game* game);
void handle_events(Game* game);
void create_grid(Game* game);
void selection(Game* game, int i, int j);
void neutralize(Game* game, int i, int j);
void reset(Game* game);



#endif // GAME_H
