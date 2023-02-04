//
// Created by pauli on 2/3/2023.
//
#ifndef MPTETRIS_APPCOMPONENT_H
#define MPTETRIS_APPCOMPONENT_H

#include "AssetHelper.h"
#include "TetrisApp.h"

namespace Tetris {

    // Ensure App is forward declared here
    class App;

    /**
     * Base class for all components
     */
    class AppComponent {
    public:
        AppComponent(Tetris::App * app, const std::string & id, int x, int y, int width, int height) : app(app), id(id), x(x), y(y), width(width), height(height) {};
        virtual void render(SDL_Renderer * renderer) = 0;
    protected:
        Tetris::App * app;
        int x,y,width,height;
        std::string id;
    };

    /*
     * A simple text component
     */
    class TextComponent : public AppComponent {
    public:
        TextComponent(Tetris::App *app, const std::string & id, int x, int y, int width, int height, const std::string &text, const std::string & fontName);

        void setText(const std::string & text);
        void render(SDL_Renderer * renderer) override;
    private:
        std::string text;
        FontHolder holder;
        SDL_Color color;
    };

} // Tetris

#endif //MPTETRIS_APPCOMPONENT_H
