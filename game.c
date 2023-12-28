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
    game->flags = 0;
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


GridPosition get_mouse_position(Game* game){

    Vector2 vec2 = GetMousePosition();
    int rw = SCREEN_W / COLS;
    int cl = SCREEN_H / ROWS;
    int r = floor(vec2.y / rw);
    int c = floor(vec2.x / cl);

    GridPosition gp = {.row = r, .col = c, .sizeX = rw, .sizeY = cl};
    return gp;
}

void handle_events(Game* game) {

    if(game->status == PLAY || game->status == PAUSED) {
        if( IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ) {
            game->status = PLAY;
            GridPosition mgp = get_mouse_position(game);
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
        GridPosition mgp = get_mouse_position(game);

        int r = mgp.row;
        int c = mgp.col;

        if(game->grid[r][c].active == true 
            || (game->grid[r][c].flagged == false && game->flags <= 0)){
            return;
        }
        game->grid[r][c].flagged = !game->grid[r][c].flagged;

        if(game->grid[r][c].flagged){
            game->flags--;
        }else{
            game->flags++;
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



Cell* getCell(Game* game, int row, int col){
    return &game->grid[row][col];
}



void draw_grid(Game* game){
    int rows = ROWS;
    int cols = COLS;
    bool won = true;

    GridPosition* mineCell = NULL;

    for (int i = 0; i < ROWS; i ++) {
        for (int j = 0; j <  COLS; j ++) {

            // Get reference to current cell
            Cell* cell =  getCell(game, i, j);

            if(game->grid[i][j].active) {
                if(game->grid[i][j].has_mine) {
                    // If field is active nad has a mine, we save its position in mineCell, 
                    // so we can draw the mine at the end, so it is on top of everything  
                    mineCell = &(GridPosition){.row = i, .col = j, .sizeX = RECT_SIZE, .sizeY = RECT_SIZE};
                    game->status = LOST;
                }else{


                    int num_of_mines = check_surrounding(game, i, j);
                    if(num_of_mines == 0 && game->grid[i][j].active == true ){
                        int revealed = 0;
                        flood_fill(game, i, j, revealed);
                    }

                    DrawRectangleRec(game->grid[i][j].rec, ColorFromHSV(r-10,g-10,b-10));

                    Color color[] = {GREEN, SKYBLUE, YELLOW, ORANGE,MAGENTA, RED};
                    if(num_of_mines > 0){
                        int numCol = num_of_mines % 5;
                        int textSize = CELL_H/2;
                        const char* text = TextFormat("%d", num_of_mines);
                        int textX = cell->centerX - ((float)MeasureText(text, textSize) / 2);
                        int textY = cell->centerY - ((float)textSize / 2);
                        DrawTextB(text, textX, textY, textSize, color[numCol]);
                    }
                }
            }else if(game->grid[i][j].flagged) {
                // Calculate position to start drawing the flag
                Texture flag_tex = assets_tex_from_img(game->assets, getAssetPath("flag"));
                
                float flagX = cell->centerX - (float)flag_tex.width / 2;
                float flagY = cell->centerY - (float)flag_tex.height / 2;

                DrawRectangleRec(game->grid[i][j].rec, Fade(BLUE, 0.3f));
                DrawTexture(flag_tex, flagX, flagY, WHITE);
            }else if(game->grid[i][j].neutralized) {
                DrawRectangleRec(game->grid[i][j].rec, Fade(RED, 0.3f));
                //DrawRectangleRec(game->grid[i][j].rec, DARKGRAY);
            }else if(game->grid[i][j].hover) {
                DrawRectangleRec(game->grid[i][j].rec, Fade(GREEN, 0.3f));
            } else{
                // IF at least one cell is not active, the game is not won
                won = false;
            }
            // we draw the grid
            DrawRectangleLines(game->grid[i][j].rec.x, game->grid[i][j].rec.y, RECT_SIZE, RECT_SIZE, LIGHTGRAY);
            game->grid[i][j].hover = false;
        }
    }
    if(mineCell != NULL){
   
        // Get reference to mine cell
        Cell* cell = getCell(game, mineCell->row, mineCell->col);

        // Load mine texture and calculate position to start drawing the mine
        Texture mine_tex = assets_tex_from_img(game->assets, "./assets/mine.png");
        float mineScale = 0.02;
        float mineX = cell->centerX - (float)mine_tex.width*mineScale / 2;
        float mineY = cell->centerY - (float)mine_tex.height*mineScale / 2;

        // We calcualte position to start drawing the explosion
        Texture explosion_tex = assets_tex_from_img(game->assets, "./assets/explosion.png");
        float explosionScale = 0.2;
        float explosionX = cell->centerX - (float)explosion_tex.width*explosionScale / 2;
        float explosionY = cell->centerY - (float)explosion_tex.height*explosionScale / 2;

        // We draw the mine and the explosion
        DrawTextureEx(explosion_tex, (Vector2){explosionX, explosionY}, 0, explosionScale, WHITE);
        DrawTextureEx(mine_tex, (Vector2){mineX, mineY}, 0, mineScale, WHITE);
    }
    if(won) {
        game->status = WON;
    }
}
void reset(Game* game){
    game->status = PAUSED;
    game->timer = 0.0;
    game->flags = 0;
    create_grid(game);
}
// This function is called once
void create_grid(Game* game){

    int num_of_mines = ROWS * COLS * ((float)MINE_PERCENT / 100.0f); 

    // Generate grid
    for (int i = 0; i < ROWS; i ++) {
        for (int j = 0; j < COLS; j ++) {
            int width = SCREEN_W / COLS;
            int height = SCREEN_H / ROWS;
            Rectangle rect = {.x = j * RECT_SIZE, .y = i* RECT_SIZE, .width = width, .height = height};

            Cell gr = {
                .rec = rect,
                .neutralized = false,
                .active = false, 
                .flagged = false,
                .visited = false,
                .centerX = rect.x + rect.width / 2,
                .centerY = rect.y + rect.height / 2
            };
            game->grid[i][j] = gr;
        }
    }

    // Place mines at random positions
    int i,j;
    while(num_of_mines > 0){
        i = GetRandomValue(0, ROWS - 1);
        j = GetRandomValue(0, COLS - 1);

        if(game->grid[i][j].has_mine == false){
            game->grid[i][j].has_mine = true;
            game->flags++;
            // Debugging
            // game->grid[i][j].neutralized = true;
            num_of_mines--;
        }
    }

}
