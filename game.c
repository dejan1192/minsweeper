#include "stdio.h"
#include "stdlib.h"
#include "raylib.h"
#include "math.h"
#include "game.h"

#define COLOR 0x7497a8

// extract the red component
int r = (COLOR >> 16) & 0xFF;
// extract the green component
int g = (COLOR >> 8) & 0xFF;
// extract the blue component
// and then  bitwise AND with 0xFF to get the blue component
int b = COLOR & 0xFF;



Texture2D explosion;
Texture2D flag;
bool hasNeutralized = false;
bool explosionActive = false;

void executeLeftClick(){
    TraceLog(LOG_INFO,"Left Click \n");
}
void executeRightClick(){
    TraceLog(LOG_INFO,"Right Click \n");
}


int main() {


    InitWindow(800, 600, "Minesweeper V01");

    Game game = {0};
    GameStatus status = PLAY;
    game.status = status;
    
    explosion = LoadTexture("assets/explosion.png");
    game.frameWidth = (float) ((float)explosion.width / NUM_FRAMES_PER_LINE);
    game.frameHeight = (float) ((float)explosion.height / NUM_LINES);
    game.frameRec = (Rectangle) {0,0,game.frameWidth,game.frameHeight};

    //Image image = LoadImage("flag.png");

    //flag = LoadTextureFromImage(image);
    //UnloadImage(image);
    create_grid(&game);
    SetTargetFPS(60);

    char* lostText = "You Lost, Press R to Restart";
    char* wonText = "You Won! Press R to Restart";

    CommandFunction leftClick = executeLeftClick;
    CommandFunction rightClick = executeRightClick;

     game.framesCounter = 0;

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        Vector2 vec;

        draw_grid(&game);
        switch(game.status){
            case SELECTION:
                 vec = GetMousePosition();
                int r = floor(vec.x / ROWS);
                int c = floor(vec.y / COLS);
                selection(&game, r,c);
                handle_events(&game);
                // selection(v.x, v.y);
                break;
            case PLAY:
                handle_events(&game);
                break;
            case WON:
                DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.8f));
                DrawText(wonText, SCREEN_W / 2 - MeasureText(wonText,20) /2, SCREEN_H  /2- MeasureText(wonText,20)/2, 20, LIGHTGRAY);
                    break;
                if(IsKeyPressed(KEY_R)) {
                    reset(&game);
                }
            case LOST:
                DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.8f));
                DrawText(lostText, SCREEN_W / 2 - MeasureText(lostText,20) /2, SCREEN_H  /2- MeasureText(lostText,20)/2, 20, LIGHTGRAY);
                if(IsKeyPressed(KEY_R)) {
                    reset(&game);
                }
                break;
        }
        EndDrawing();
    }

    return 0;
}

void handle_events(Game* game) {

    if(game->status == PLAY) {
        if( IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ) {
            Vector2 vec2 = GetMousePosition();
            //TraceLog(LOG_INFO,"Mouse was pressed %f -> %f \n", vec2.x/ROWS, vec2.y);
            int r = floor(vec2.x / ROWS);
            int c = floor(vec2.y / COLS);
            TraceLog(LOG_INFO,"Mouse was pressed %d -> %d \n", r, c);
            if( game->grid[r][c].active == false) {
                game->grid[r][c].active = true;;
            }
        }

    }else if(game->status == SELECTION){
        TraceLog(LOG_INFO,"Game Status SELECTION %d \n", game->status);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            Vector2 vec2 = GetMousePosition();
            int r = floor(vec2.x / ROWS);
            int c = floor(vec2.y / COLS);
            //grid[r][c].flagged = !grid[r][c].flagged;
            explosionActive = true;
            game->position =  (Vector2) {.x = GetMouseX() -game->frameWidth / 2 ,.y = GetMouseY() - game->frameHeight /2 };
            TraceLog(LOG_INFO, "Position %f -> %f \n", floor(game->position.x / ROWS), floor(game->position.y / COLS));

                neutralize(game, r,c);
            game->status = PLAY;

        }
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !hasNeutralized){
        hasNeutralized = true;

        game->status = SELECTION;
        TraceLog(LOG_INFO,"Game Status: %d \n", game->status);

        if(IsKeyPressed(KEY_ESCAPE)){
            game->status = PLAY;
        }
        
    }
}
void neutralize(Game* game, int i,int j){
{
    int len = sizeof(game->grid) / sizeof(game->grid[0]);

    for (int x = i - 1; x <= i + 1; x ++) {
        for (int y = j - 1; y <= j + 1; y ++) {
            if (x >= 0 && x < len && y >= 0 && y < len) {
                    game->grid[x][y].neutralized = true;
                    game->grid[x][y].active = true;
            }
        }
    }
   }

}
void selection(Game* game, int i, int j){
    
    int len = sizeof(game->grid) / sizeof(game->grid[0]);

    for (int x = i - 1; x <= i + 1; x ++) {
        for (int y = j - 1; y <= j + 1; y ++) {
            if (x >= 0 && x < len && y >= 0 && y < len) {
                game->grid[x][y].hover = true;
            }
        }
    }

}

int check_surrounding(Game* game, int i, int j){
    int len = sizeof(game->grid) / sizeof(game->grid[0]);
    int num_of_mines = 0;
    for (int x = i - 1; x <= i + 1; x ++) {
        for (int y = j - 1; y <= j + 1; y ++) {
            if (x >= 0 && x < len && y >= 0 && y < len) {
                if(game->grid[x][y].has_mine) {
                    // TraceLog(LOG_INFO,"Found mine at %d -> %d \n", x, y);
                    num_of_mines ++;
                }
            }
        }
    }
    return num_of_mines;
}
void reveal(Game* game, int i, int j){
    int len = sizeof(game->grid) / sizeof(game->grid[0]);
    for (int x = i - 1; x <= i + 1; x ++) {
        for (int y = j - 1; y <= j + 1; y ++) {
            if (x >= 0 && x < len && y >= 0 && y < len) {
                if(!game->grid[x][y].has_mine) {
                    game->grid[x][y].active = true;
                }
            }
        }
    }
}

void draw_grid(Game* game){
    int rows = SCREEN_W / ROWS;
    int cols = SCREEN_H / COLS;
    bool won = true;
    for (int i = 0; i < rows; i ++) {
        for (int j = 0; j <  cols; j ++) {
            if(game->grid[i][j].active && !game->grid[i][j].neutralized) {
                if(game->grid[i][j].has_mine) {
                    DrawRectangleRec(game->grid[i][j].rec, RED);
                    game->status = LOST;
                }else{
                    int num_of_mines = check_surrounding(game, i, j);
                    if(num_of_mines == 0 && game->grid[i][j].active == true ){
                        // TraceLog(LOG_INFO,"State active %d -> %d \n", i, j);
                        reveal(game, i,j);
                    }
                    
                    DrawRectangleRec(game->grid[i][j].rec, ColorFromHSV(r-10,g-10,b-10));
                    if(num_of_mines > 0)
                        DrawText(TextFormat("%d", num_of_mines), i * ROWS+15, j * COLS+10, 20, BLACK);
                }
            }else if(game->grid[i][j].flagged) {
                //DrawTexture(flag, grid[i][j].rec.x , grid[i][j].rec.y, WHITE);
                //DrawRectangleRec(grid[i][j].rec, BLUE);
            }else if(game->grid[i][j].neutralized) {
                DrawRectangleRec(game->grid[i][j].rec, DARKGRAY);
            }else if(game->grid[i][j].hover) {
                DrawRectangleRec(game->grid[i][j].rec, Fade(GREEN, 0.3f));
            }
            else{
                won = false;
            }

            //TraceLog(LOG_INFO, " Game grid %d -> %d  | game grid rec X: %d Y: %d  \n", i, j, game->grid[i][j].rec.x, game.grid[i][j].rec.y);
            DrawRectangleLines(game->grid[i][j].rec.x, game->grid[i][j].rec.y, game->grid[i][j].rec.width, game->grid[i][j].rec.height, LIGHTGRAY);
            game->grid[i][j].hover = false;
        }
    }
    game->framesCounter++;
    if(explosionActive){
        if (game->framesCounter > 2){
            game->currentFrame++;
            if (game->currentFrame >= NUM_FRAMES_PER_LINE){
                game->currentFrame = 0;
                game->currentLine++;
                if (game->currentLine >= NUM_LINES){
                    game->currentLine = 0;
                    explosionActive = false;
                }
            }
            game->framesCounter = 0;
        }
    }
    game->frameRec.x = game->frameWidth*game->currentFrame;
    game->frameRec.y = game->frameHeight*game->currentLine;
    if(explosionActive) DrawTextureRec(explosion, game->frameRec, game->position, WHITE);
    
    if(won) {
        game->status = WON;
    }
}
void reset(Game* game){
    hasNeutralized = false;
    create_grid(game);
}
void create_grid(Game* game){

    game->status = PLAY;
    int num_of_mines = (SCREEN_W / ROWS) * (SCREEN_H / COLS) * MINE_PERCENT / 100; 

    for (int i = 0; i < SCREEN_W/ ROWS; i ++) {
        for (int j = 0; j < SCREEN_H / COLS; j ++) {
            Rectangle rect = {.x = i * ROWS, .y = j * COLS, .width = RECT_SIZE, .height = RECT_SIZE};

            GameRec gr = {
                .rec = rect,
                .neutralized = false,
                .active = false, 
                .flagged = false
            };
            
            if (GetRandomValue(0, 100) < 10) {
                gr.has_mine = true;
            }
            game->grid[i][j] = gr;
        }
    }
}
