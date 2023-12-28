#include "asset.h"
#include "game.h"
#include "stdlib.h"
#include "stdio.h"
#include "raylib.h"
#include "font.h"

#define MARGIN_15 15

Font myfont;

int main() {

    InitWindow(SCREEN_W,HEADER_SIZE+ SCREEN_H, "Minesweeper V01");

    Game game;
    Assets assets;
    myfont = LoadFont("assets/fonts/Roboto-Bold.ttf");
    initGame(&game, &assets);

    // Preload all assets
    {
        assets_tex_from_img(game.assets, getAssetPath("flag"));
        assets_tex_from_img(game.assets, "./assets/lost.png");
        assets_tex_from_img(game.assets, "./assets/mine.png");
        assets_tex_from_img(game.assets, "./assets/explosion.png");
    }

    SetTargetFPS(60);
    char* lostText = "You Lost, Press R to Restart";
    char* wonText = "You Won! Press R to Restart";


    CommandFunction leftClick = executeLeftClick;
    CommandFunction rightClick = executeRightClick;


    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        Vector2 vec;

        draw_game_panel(&game);
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
                DrawTextureEx(img, (Vector2){(float)SCREEN_W / 2 - (float)imgSize/2, (SCREEN_H / 2 - (float)imgSize - fontSize) }, 0, imgScale, WHITE);
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


void draw_game_panel(Game* game){
    int panel_font_size = RECT_SIZE * 0.7f;

    const char* time = TextFormat("Time: %d", (int)game->timer);
    const char* flagText = TextFormat("Flags: ", game->flags);
    const char* flagNum = TextFormat("%d", game->flags);

    int flagTextLen = MeasureText(flagText, panel_font_size);
    int flagNumLen = MeasureText(flagNum, panel_font_size);

    DrawTextB(flagText, MARGIN_15,  SCREEN_H + MARGIN_15, panel_font_size, BLACK);
    DrawTextB(flagNum,flagTextLen , SCREEN_H +MARGIN_15, panel_font_size, RED);
    DrawTextB(time, flagTextLen + flagNumLen + MARGIN_15,  SCREEN_H + MARGIN_15, panel_font_size, BLACK);
}
