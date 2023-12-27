#include "asset.h"
#include "game.h"
#include "stdlib.h"
#include "stdio.h"
#include "raylib.h"
#include "font.h"

#define MAX_ASSETS 50

Font myfont;

int main() {

    InitWindow(SCREEN_W,HEADER_SIZE+ SCREEN_H, "Minesweeper V01");

    Game game;
    Assets assets;
    myfont = LoadFont("assets/fonts/Roboto-Bold.ttf");

    // AssetPath paths[MAX_ASSETS];

    // load_assets_from_file(&paths);

    initGame(&game, &assets);
    //Loads asset if not already loaded
    assets_image(&assets, getAssetPath("flag"));

    SetTargetFPS(60);
    char* lostText = "You Lost, Press R to Restart";
    char* wonText = "You Won! Press R to Restart";

    CommandFunction leftClick = executeLeftClick;
    CommandFunction rightClick = executeRightClick;


    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        Vector2 vec;

        draw_grid(&game);
        switch(game.status){
            case PLAY:
                handle_events(&game);
                break;
            case PAUSED:
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
    unloadAssets(game.assets);
    UnloadFont(myfont);

    return 0;
}

