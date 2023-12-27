#include "font.h"
#include "stdlib.h"

void DrawTextB(const char* text, int posX, int posY, int fontSize, Color color){
    //if(myfont == NULL) myfont = GetFontDefault();
    DrawTextEx(myfont, text, (Vector2){posX, posY}, fontSize, 0, color);
}