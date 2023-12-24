#ifndef GAME_H
#define GAME_H

#include "raylib.h"

#define ROWS  40
#define COLS  40
#define RECT_SIZE 40
#define SCREEN_W 800
#define SCREEN_H 600
#define MINE_PERCENT 15

#define COLOR 0x7497a8

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
} GameRec;

void draw_grid();
void handle_events();
void create_grid();
void selection(int i, int j);
void neutralize(int i, int j);
void reset();



#endif // GAME_H
