#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "asset.h"

#define ROWS  15
#define COLS  20
#define RECT_SIZE 30
#define HEADER_SIZE 50
#define SCREEN_W (RECT_SIZE * COLS)
#define SCREEN_H (RECT_SIZE * ROWS)
#define CELL_W RECT_SIZE
#define CELL_H RECT_SIZE
#define MINE_PERCENT 11


#define NUM_FRAMES_PER_LINE 5
#define NUM_LINES 5

typedef struct {
    int row;
    int col;
    int sizeX;
    int sizeY;
} GridPosition;

typedef enum GameStatus {
    PLAY,
    MENU,
    PAUSED,
    WON,
    LOST,
} GameStatus;

typedef struct CellCenter {
    float x;
    float y;
} CellCenter;

typedef void (*CommandFunction)(void);

typedef struct Command {
    CommandFunction func;
} Command;


typedef struct Cell {
    Rectangle rec;
    bool has_mine;
    bool flagged;
    // Currently only for debugging
    bool neutralized;
    bool hover;
    bool active;
    bool visited;
    float centerX;
    float centerY;
} Cell;

typedef struct {
    GameStatus status;
    Cell grid[ROWS][COLS];
    Assets* assets;
    double timer;
    int flags;
} Game;

void initGame(Game* game, Assets* assets);

int check_surrounding(Game* game, int i, int j);
void draw_grid(Game* game);
void handle_events(Game* game);
void create_grid(Game* game);
void reset(Game* game);
void draw_game_panel(Game* game);


void executeLeftClick();
void executeRightClick();

#endif // GAME_H
