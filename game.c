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


Texture2D flag;
void executeLeftClick(){
    TraceLog(LOG_INFO,"Left Click \n");
}
void executeRightClick(){
    TraceLog(LOG_INFO,"Right Click \n");
}

void flood_fill(Game* game, int r, int c, int curr_size){

    if(
        (r < 0 || r >= ROWS || c < 0 || c>= COLS) // out of bounds
        || game->grid[r][c].visited// already visited
    ){

        return;
    }
    if(check_surrounding(game, r, c) > 0){
        game->grid[r][c].visited = true;
        if( !game->grid[r][c].has_mine && !game->grid[r][c].flagged ){
            game->grid[r][c].active = true;
            return;
        }
    }

    game->grid[r][c].visited = true;
    if( !game->grid[r][c].has_mine && !game->grid[r][c].flagged )
        game->grid[r][c].active = true;

    curr_size++;

    // recursiv call flood_fill for all 4 adjacent cells
    flood_fill(game, r, c + 1, curr_size);
    flood_fill(game, r, c - 1, curr_size);
    flood_fill(game, r + 1, c, curr_size);
    flood_fill(game, r - 1, c, curr_size);
}

int main() {


    InitWindow(SCREEN_W,HEADER_SIZE+ SCREEN_H, "Minesweeper V01");

    Game game = {0};
    GameStatus status = PLAY;
    game.status = status;

    Image image = LoadImage("assets/flag.png");

    flag = LoadTextureFromImage(image);
    UnloadImage(image);
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
            case PLAY:
                handle_events(&game);
                break;
            case WON:
                DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.8f));
                DrawText(wonText, SCREEN_W / 2 - MeasureText(wonText,20) /2, SCREEN_H  /2- MeasureText(wonText,20)/2, 20, LIGHTGRAY);
                if(IsKeyPressed(KEY_R)) {
                    reset(&game);
                }
                break;
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

typedef struct {
    int row;
    int col;
    int sizeX;
    int sizeY;
} MouseGridPosition;

MouseGridPosition get_mouse_position(Game* game){

    Vector2 vec2 = GetMousePosition();
    int rw = SCREEN_W / COLS;
    int cl = SCREEN_H / ROWS;
    int r = floor(vec2.y / rw);
    int c = floor(vec2.x / cl);

    MouseGridPosition gp = {.row = r, .col = c, .sizeX = rw, .sizeY = cl};
    return gp;
}

void handle_events(Game* game) {

    if(game->status == PLAY) {
        if( IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ) {
            MouseGridPosition mgp = get_mouse_position(game);
            int r = mgp.row;
            int c = mgp.col;

            if(game->grid[r][c].flagged){
                return;
            }
            if( game->grid[r][c].active == false) {
                TraceLog(LOG_INFO,"ROW: %d | COL: %d \n", r, c);
                game->grid[r][c].active = true;;
            }
        }

    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
            MouseGridPosition mgp = get_mouse_position(game);

            int r = mgp.row;
            int c = mgp.col;
           
            if(game->grid[r][c].active == true 
            || (game->grid[r][c].flagged == false && game->maxFlags <= 0)){
            return;
        }
        game->grid[r][c].flagged = !game->grid[r][c].flagged;

        if(game->grid[r][c].flagged){
            game->maxFlags--;
        }else{
            game->maxFlags++;
        }
        TraceLog(LOG_INFO,"Max Flags: %d \n", game->maxFlags);
    }
}

int check_surrounding(Game* game, int i, int j){
    int num_of_mines = 0;
    for (int x = i - 1; x <= i + 1; x ++) {
        for (int y = j - 1; y <= j + 1; y ++) {
            if (x >= 0 && x < ROWS && y >= 0 && y < COLS) {
                if(game->grid[x][y].has_mine) {
                   // TraceLog(LOG_INFO,"Found mine at %d -> %d \n", x, y);
                    num_of_mines ++;
                }
            }
        }
    }
    return num_of_mines;
}

void draw_grid(Game* game){
    int rows = ROWS;
    int cols = COLS;
    bool won = true;

    int font_size = RECT_SIZE * 0.7f;
    const char* flagText = TextFormat("Flags: ", game->maxFlags);
    const char* flagNum = TextFormat("%d", game->maxFlags);

    DrawText(flagText, 10,  SCREEN_H + 15, font_size, BLACK);
    DrawText(flagNum, 10 + MeasureText(flagText, font_size), SCREEN_H +15, font_size, RED);

    for (int i = 0; i < ROWS; i ++) {
        for (int j = 0; j <  COLS; j ++) {

            if(game->grid[i][j].active) {
                if(game->grid[i][j].has_mine) {
                    DrawRectangleRec(game->grid[i][j].rec, RED);
                    game->status = LOST;
                }else{
                    int num_of_mines = check_surrounding(game, i, j);
                    if(num_of_mines == 0 && game->grid[i][j].active == true ){
                        int revealed = 0;
                        flood_fill(game, i, j, revealed);
                    }

                    DrawRectangleRec(game->grid[i][j].rec, ColorFromHSV(r-10,g-10,b-10));

                    float centerX = (game->grid[i][j].rec.x + game->grid[i][j].rec.width) - game->grid[i][j].rec.width / 2;
                    float centerY = (game->grid[i][j].rec.y + game->grid[i][j].rec.height) - game->grid[i][j].rec.height / 2;

                    if(num_of_mines > 0){
                        int textSize = CELL_W/2;
                        const char* text = TextFormat("%d", num_of_mines);
                        int textX = centerX - (MeasureText(text, textSize) / 2);
                        int textY = centerY - (textSize / 2);
                        DrawText(text, textX, textY, textSize, BLACK);
                    }
                }
            }else if(game->grid[i][j].flagged) {
                float centerX = (game->grid[i][j].rec.x + game->grid[i][j].rec.width) - game->grid[i][j].rec.width / 2;
                float centerY = (game->grid[i][j].rec.y + game->grid[i][j].rec.height) - game->grid[i][j].rec.height / 2;
                // Calculate position to start drawing the flag
                // so that it is centered in the cell
                float flagX = centerX - (float)flag.width / 2;
                float flagY = centerY - (float)flag.height / 2;

                DrawRectangleRec(game->grid[i][j].rec, Fade(BLUE, 0.3f));
                DrawTexture(flag, flagX, flagY, WHITE);
            }else if(game->grid[i][j].neutralized) {
                DrawRectangleRec(game->grid[i][j].rec, Fade(RED, 0.3f));
                //DrawRectangleRec(game->grid[i][j].rec, DARKGRAY);
            }else if(game->grid[i][j].hover) {
                DrawRectangleRec(game->grid[i][j].rec, Fade(GREEN, 0.3f));
            } else{
                // Draw the actual cell
                //DrawRectangleRec(game->grid[i][j].rec, GetCellColor(game->grid[i][j]));
                won = false;
            }

            DrawRectangleLines(game->grid[i][j].rec.x, game->grid[i][j].rec.y, RECT_SIZE, RECT_SIZE, LIGHTGRAY);

               Vector2 shadowPos = { game->grid[i][j].rec.x + 3, game->grid[i][j].rec.y + 3 };
                // Draw shadow
               Color shadowColor = (Color){0, 0, 0, 100};
            //DrawRectangle(shadowPos.x+2, shadowPos.y+2, game->grid[i][j].rec.width, game->grid[i][j].rec.height, Fade(WHITE, 0.2f));
            game->grid[i][j].hover = false;
        }
    }
    if(won) {
        game->status = WON;
    }
}
void reset(Game* game){
    game->maxFlags = 0;
    create_grid(game);
}
void create_grid(Game* game){

    game->status = PLAY;
    int num_of_mines = ROWS * COLS * ((float)MINE_PERCENT / 100.0f); 

    for (int i = 0; i < ROWS; i ++) {
        for (int j = 0; j < COLS; j ++) {
            int width = SCREEN_W / COLS;
            int height = SCREEN_H / ROWS;
            Rectangle rect = {.x = j * RECT_SIZE, .y = i* RECT_SIZE, .width = width, .height = height};

            GameRec gr = {
                .rec = rect,
                .neutralized = false,
                .active = false, 
                .flagged = false,
                .visited = false
            };
            game->grid[i][j] = gr;
        }
    }

    int i,j;
    TraceLog(LOG_INFO,"NUM OF MINES %d , TOTAL_FIELDS: %d ", num_of_mines, ROWS * COLS);
    while(num_of_mines > 0){
        i = GetRandomValue(0, ROWS - 1);
        j = GetRandomValue(0, COLS - 1);

        if(game->grid[i][j].has_mine == false){
            game->grid[i][j].has_mine = true;
            game->maxFlags++;
            // Debugging
            //game->grid[i][j].neutralized = true;
            num_of_mines--;
        }
    }

}
