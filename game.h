#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "asset.h"

#define ROWS  20
#define COLS  25
#define RECT_SIZE 30
#define HEADER_SIZE 50
#define SCREEN_W (RECT_SIZE * COLS)
#define SCREEN_H (RECT_SIZE * ROWS)
#define CELL_W RECT_SIZE
#define CELL_H RECT_SIZE
#define MINE_PERCENT 11


#define NUM_FRAMES_PER_LINE 5
#define NUM_LINES 5


typedef enum GameStatus {
    PLAY,
    PAUSED,
    WON,
    LOST,
} GameStatus;

typedef void (*CommandFunction)(void);

typedef struct Command {
    CommandFunction func;
} Command;


typedef struct GameRec {
    Rectangle rec;
    bool has_mine;
    bool flagged;
    // Currently only for debugging
    bool neutralized;
    bool hover;
    bool active;
    bool visited;
} GameRec;

typedef struct {
    GameStatus status;
    GameRec grid[ROWS][COLS];
    Assets* assets;
    double timer;
    int maxFlags;
    int currentFrame;
    int framesCounter;
    float frameWidth;
    float frameHeight;
} Game;

void initGame(Game* game, Assets* assets);

void draw_grid(Game* game);
void handle_events(Game* game);
void create_grid(Game* game);
void selection(Game* game, int i, int j);
void neutralize(Game* game, int i, int j);
void reset(Game* game);



#endif // GAME_H
