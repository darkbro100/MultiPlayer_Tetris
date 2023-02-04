//Using SDL and standard IO
#include "TetrisApp.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int TOTAL_TEXTURES = 7;
const int TEXTURE_SIZE = 512; // for both width and height

//void renderText(SDL_Renderer * renderer, SDL_Rect dest, TTF_Font * font, std::string text) {
//    SDL_Color fg = { 255, 255, 255 };
//    SDL_Surface* surf = TTF_RenderText_Solid(font, text.c_str(), fg);
//
//    dest.w = surf->w;
//    dest.h = surf->h;
//
//    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
//
//    SDL_RenderCopy(renderer, tex, NULL, &dest);
//    SDL_DestroyTexture(tex);
//    SDL_FreeSurface(surf);
//}

int main(int argc, char *argv[]) {
    Tetris::App app;
    app.run();
    return 0;
}

