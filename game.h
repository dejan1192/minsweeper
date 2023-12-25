#ifndef GAME_H
#define GAME_H

#include "raylib.h"

#define ROWS  16
#define COLS  30
#define RECT_SIZE 40
#define HEADER_SIZE 100
#define SCREEN_W (RECT_SIZE * COLS)
#define SCREEN_H (RECT_SIZE * ROWS)
#define CELL_W RECT_SIZE
#define CELL_H RECT_SIZE
#define MINE_PERCENT 15


#define NUM_FRAMES_PER_LINE 5
#define NUM_LINES 5


typedef enum GameStatus {
    PLAY,
    PAUSED,
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
    GameRec grid[ROWS][COLS];
    double timer;
    int maxFlags;
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
