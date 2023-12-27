#include "stdio.h"
#include "stdlib.h"
#include "raylib.h"
#include "math.h"
#include "asset.h"
#include "game.h"
#include "font.h"

#define COLOR 0x7497a8

// extract the red component
int r = (COLOR >> 16) & 0xFF;
// extract the green component
int g = (COLOR >> 8) & 0xFF;
// extract the blue component
// and then  bitwise AND with 0xFF to get the blue component
int b = COLOR & 0xFF;

void executeLeftClick(){
    TraceLog(LOG_INFO,"Left Click \n");
}
void executeRightClick(){
    TraceLog(LOG_INFO,"Right Click \n");
}

void initGame(Game* game, Assets* assets){
    initAssets(assets);

    game->status = PAUSED;
    game->timer = 0.0f;
    game->maxFlags = 0;
    game->assets = assets;
    game->status = PAUSED;

    create_grid(game);
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

    if(game->status == PLAY || game->status == PAUSED) {
        if( IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ) {
            game->status = PLAY;
            MouseGridPosition mgp = get_mouse_position(game);
            int r = mgp.row;
            int c = mgp.col;

            if(game->grid[r][c].flagged){
                return;
            }
            if( game->grid[r][c].active == false) {
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
    }
}

int check_surrounding(Game* game, int i, int j){
    int num_of_mines = 0;
    for (int x = i - 1; x <= i + 1; x ++) {
        for (int y = j - 1; y <= j + 1; y ++) {
            if (x >= 0 && x < ROWS && y >= 0 && y < COLS) {
                if(game->grid[x][y].has_mine) {
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

    int margin = 15;
    int font_size = RECT_SIZE * 0.7f;
    const char* time = TextFormat("Time: %d", (int)game->timer);
    const char* flagText = TextFormat("Flags: ", game->maxFlags);
    const char* flagNum = TextFormat("%d", game->maxFlags);

    int flagTextLen = MeasureText(flagText, font_size);
    int flagNumLen = MeasureText(flagNum, font_size);

    DrawTextB(flagText, margin,  SCREEN_H + margin, font_size, BLACK);
    DrawTextB(flagNum,flagTextLen , SCREEN_H +margin, font_size, RED);
    DrawTextB(time, flagTextLen + flagNumLen + margin,  SCREEN_H + margin, font_size, BLACK);

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

                    Color color[] = {GREEN, SKYBLUE, YELLOW, ORANGE,MAGENTA, RED};
                    if(num_of_mines > 0){
                        int numCol = num_of_mines % 5;
                        int textSize = CELL_W/2;
                        const char* text = TextFormat("%d", num_of_mines);
                        int textX = centerX - (MeasureText(text, textSize) / 2);
                        int textY = centerY - (textSize / 2);
                        DrawTextB(text, textX, textY, textSize, color[numCol]);
                    }
                }
            }else if(game->grid[i][j].flagged) {
                float centerX = (game->grid[i][j].rec.x + game->grid[i][j].rec.width) - game->grid[i][j].rec.width / 2;
                float centerY = (game->grid[i][j].rec.y + game->grid[i][j].rec.height) - game->grid[i][j].rec.height / 2;
                // Calculate position to start drawing the flag
                Texture flag_tex = assets_tex_from_img(game->assets, getAssetPath("flag"));
                float flagX = centerX - (float)flag_tex.width / 2;
                float flagY = centerY - (float)flag_tex.height / 2;

                DrawRectangleRec(game->grid[i][j].rec, Fade(BLUE, 0.3f));
                DrawTexture(flag_tex, flagX, flagY, WHITE);
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
            game->grid[i][j].hover = false;
        }
    }
    if(won) {
        game->status = WON;
    }
}
void reset(Game* game){
    game->status = PAUSED;
    game->timer = 0.0;
    game->maxFlags = 0;
    create_grid(game);
}
void create_grid(Game* game){

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
