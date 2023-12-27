#include "asset.h"
#include "game.h"
#include "stdlib.h"
#include "stdio.h"
#include "raylib.h"
#include "font.h"

Font myfont;

int main() {

    InitWindow(SCREEN_W,HEADER_SIZE+ SCREEN_H, "Minesweeper V01");

    Game game;
    Assets assets;
    myfont = LoadFont("assets/fonts/Roboto-Bold.ttf");
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
                game.timer += GetFrameTime();
                handle_events(&game);
                break;
            case PAUSED:
                handle_events(&game);
                break;
            case WON:
                DrawRectangle(0, 0, SCREEN_W, HEADER_SIZE + SCREEN_H, Fade(BLACK, 0.8f));
                DrawTextB(wonText, SCREEN_W / 2 - MeasureText(wonText,RECT_SIZE) /2, SCREEN_H  /2- MeasureText(wonText,RECT_SIZE)/2, RECT_SIZE, LIGHTGRAY);
                if(IsKeyPressed(KEY_R)) {
                    reset(&game);
                }
                break;
            case LOST:
                DrawRectangle(0, 0, SCREEN_W, HEADER_SIZE + SCREEN_H, Fade(BLACK, 0.8f));
                int fontSize = RECT_SIZE/1.4;
                int textWidth = MeasureText(lostText, fontSize);

                DrawTextB(lostText, (float)SCREEN_W / 2 - ((float)textWidth / 2 - 50), (float)SCREEN_H / 2 - ((float)fontSize / 2.0f), fontSize, LIGHTGRAY);
                Texture2D img = assets_tex_from_img(&assets, "./assets/lost.png");
                float imgScale = 0.25;
                int imgSize = img.width * imgScale;
                DrawTextureEx(img, (Vector2){(float)SCREEN_W / 2.0 - (float)imgSize/2, 150}, 0, imgScale, WHITE);
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

