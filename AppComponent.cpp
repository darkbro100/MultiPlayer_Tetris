//
// Created by pauli on 2/3/2023.
//
#include "AppComponent.h"

namespace Tetris {

    TextComponent::TextComponent(Tetris::App * app, const std::string & id, int x, int y, int width, int height, const std::string &text, const std::string & fontName)
    : AppComponent(app, id, x, y, width, height)
    {
        this->text = text;
        this->holder = app->getFont(fontName);
        this->color = {255, 255, 255, 255};
    }

    void TextComponent::setText(const std::string &text) {
        this->text = text;
    }

    void TextComponent::render(SDL_Renderer *renderer) {
        TTF_Font * font = holder.font;
        SDL_Rect pos = {x, y, width, height};
        renderText(renderer, font, text, color, &pos);
    }
} // Tetris