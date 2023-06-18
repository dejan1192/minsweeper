#include "stdio.h"
#include "stdlib.h"
#include "raylib.h"
#include "math.h"


void draw_grid();
void handle_events();
void create_grid();

#define ROWS  40
#define COLS  40
#define RECT_SIZE 40
#define SCREEN_W 800
#define SCREEN_H 600
#define MINE_PERCENT 10

#define COLOR 0x7497a8

// extract the red component
int r = (COLOR >> 16) & 0xFF;
// extract the green component
int g = (COLOR >> 8) & 0xFF;
// extract the blue component
// and then  bitwise AND with 0xFF to get the blue component
int b = COLOR & 0xFF;

typedef enum GameStatus {
    PLAY,
    WON,
    LOST
} GameStatus;

typedef struct GameRec {
    Rectangle rec;
    bool has_mine;
    bool flagged;
    bool active;
} GameRec;
GameStatus game_status;

GameRec grid[SCREEN_W / ROWS][SCREEN_H / COLS];
Texture2D flag;

int main() {


    InitWindow(800, 600, "Minesweeper V01");
    Image image = LoadImage("flag.png");
    Texture2D flag = LoadTextureFromImage(image);
    UnloadImage(image);
    create_grid();
    SetTargetFPS(60);
    char lostText[] = "You Lost, Press R to Restart";
    char wonText[] = "You Won! Press R to Restart";



    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        draw_grid();

        if(game_status == PLAY) {
            handle_events();
        }else if(game_status == WON) {
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.8f));
            DrawText(wonText, SCREEN_W / 2 - MeasureText(wonText,20) /2, SCREEN_H  /2- MeasureText(wonText,20)/2, 20, LIGHTGRAY);
            if(IsKeyPressed(KEY_R)) {
                create_grid();
            }
        }else if(game_status == LOST) {
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.8f));
            DrawText(lostText, SCREEN_W / 2 - MeasureText(lostText,20) /2, SCREEN_H  /2- MeasureText(lostText,20)/2, 20, LIGHTGRAY);
            
            if(IsKeyPressed(KEY_R)) {
                create_grid();

            }

        }
        EndDrawing();
    }

    return 0;
}

void handle_events() {

    if( IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ) {
        Vector2 vec2 = GetMousePosition();
        //TraceLog(LOG_INFO,"Mouse was pressed %f -> %f \n", vec2.x/ROWS, vec2.y);
        int r = floor(vec2.x / ROWS);
        int c = floor(vec2.y / COLS);
        TraceLog(LOG_INFO,"Mouse was pressed %d -> %d \n", r, c);
        if( grid[r][c].active == false) {
            grid[r][c].active = true;;
        }
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
        
        Vector2 vec2 = GetMousePosition();
        int r = floor(vec2.x / ROWS);
        int c = floor(vec2.y / COLS);
        grid[r][c].flagged = !grid[r][c].flagged;
        
    }
}
void check_sur(int i, int j){
    

}

int check_surrounding(int i, int j){
    int len = sizeof(grid) / sizeof(grid[0]);
    int num_of_mines = 0;
    for (int x = i - 1; x <= i + 1; x ++) {
        for (int y = j - 1; y <= j + 1; y ++) {
            if (x >= 0 && x < len && y >= 0 && y < len) {
                if(grid[x][y].has_mine) {
                    TraceLog(LOG_INFO,"Found mine at %d -> %d \n", x, y);
                    num_of_mines ++;
                }
            }
        }
    }
    return num_of_mines;
}
void reveal(int i, int j){
    int len = sizeof(grid) / sizeof(grid[0]);
    for (int x = i - 1; x <= i + 1; x ++) {
        for (int y = j - 1; y <= j + 1; y ++) {
            if (x >= 0 && x < len && y >= 0 && y < len) {
                if(!grid[x][y].has_mine) {
                    grid[x][y].active = true;
                }
            }
        }
    }
}

void draw_grid(){
    int rows = SCREEN_W / ROWS;
    int cols = SCREEN_H / COLS;
    bool won = true;
    for (int i = 0; i < rows; i ++) {
        for (int j = 0; j <  cols; j ++) {
            if(grid[i][j].active) {
                if(grid[i][j].has_mine) {
                    DrawRectangleRec(grid[i][j].rec, RED);
                    game_status = LOST;
                }else{
                    int num_of_mines = check_surrounding(i, j);
                    if(num_of_mines == 0 && grid[i][j].active == true) {
                        TraceLog(LOG_INFO,"State active %d -> %d \n", i, j);
                        reveal(i,j);
                    }
                    
                    DrawRectangleRec(grid[i][j].rec, ColorFromHSV(r-10,g-10,b-10));
                    if(num_of_mines > 0)
                        DrawText(TextFormat("%d", num_of_mines), i * ROWS+15, j * COLS+10, 20, BLACK);
                }
            }else if(grid[i][j].flagged) {
                DrawTexture(flag, grid[i][j].rec.x , grid[i][j].rec.y, RED);
                //DrawRectangleRec(grid[i][j].rec, BLUE);
            }else{
                won = false;
                DrawRectangleLines(grid[i][j].rec.x, grid[i][j].rec.y, grid[i][j].rec.width, grid[i][j].rec.height, LIGHTGRAY);
            }

        }
    }
    if(won) {
        game_status = WON;
    }
}
void create_grid(){

    
    game_status = PLAY;

    int num_of_mines = (SCREEN_W / ROWS) * (SCREEN_H / COLS) * MINE_PERCENT / 100; 
    for (int i = 0; i < SCREEN_W/ ROWS; i ++) {
        for (int j = 0; j < SCREEN_H / COLS; j ++) {
            Rectangle rect = {i * ROWS, j * COLS, RECT_SIZE, RECT_SIZE};
            GameRec gr = {
                .rec = rect,
                .active = false, 
                .flagged = false
            };
            
            if (GetRandomValue(0, 100) < 10) {
                gr.has_mine = true;
            }
            grid[i][j] = gr;
        }
    }
}
