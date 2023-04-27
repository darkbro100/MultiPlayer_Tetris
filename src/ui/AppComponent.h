//
// Created by pauli on 2/3/2023.
//
#ifndef MPTETRIS_APPCOMPONENT_H
#define MPTETRIS_APPCOMPONENT_H

#define FPS_SAMPLE_SIZE 1000

#include "AssetHelper.h"
#include <list>
#include <numeric>
#include <functional>

namespace Tetris {

    // Ensure App is forward declared here
    class App;

    /**
     * Base class for all components
     */
    class AppComponent {
    public:
        AppComponent(Tetris::App * app, const std::string & id, int x, int y, int width, int height) : app(app), id(id), x(x), y(y), width(width), height(height), enabled(true) {};
        bool isEnabled() const;
        virtual void render(SDL_Renderer * renderer, float timestep) = 0;
        float getX() const;
        float getY() const;
        float getWidth() const;
        float getHeight() const;
    protected:
        bool enabled;
        Tetris::App * app;
        float x,y,width,height;
        std::string id;
    };

    /*
     * A simple text component
     */
    class TextComponent : public AppComponent {
    public:
        TextComponent(Tetris::App *app, const std::string & id, int x, int y, int width, int height, const std::string &text, const std::string & fontName);

        void setText(const std::string & text);
        void render(SDL_Renderer * renderer, float timestep) override;
    private:
        std::string text;
        FontHolder holder;
        SDL_Color color;
    };

    class UpdatingComponent : public AppComponent {
    public:
        UpdatingComponent(Tetris::App *app, const std::string & id, int x, int y, int width, int height);
        virtual void update(float timestep) = 0;
    };

    class FPSComponent : public UpdatingComponent {
    public:
        FPSComponent(Tetris::App *app, const std::string & id, int x, int y, int width, int height);
        void update(float timestep) override;
        void render(SDL_Renderer * renderer, float timestep) override;
    private:
        FontHolder fontHolder;
        float averageFps;
        float currentFps;
        bool wasPressed;
        std::list<float> fpsCalcs;
    };

    class ButtonComponent : public UpdatingComponent {
    public:
        ButtonComponent(Tetris::App *app, const std::string & id, const std::string & texture, int x, int y, int width, int height);
        void render(SDL_Renderer * renderer, float timestep) override;
        void update(float timestep) override;

        void onClick(std::function<void()> callback) { onClickCallback = std::move(callback); }
        void click();
    protected:
        std::function<void()> onClickCallback = [&](){ };

        TextureHolder holder;
    };

} // Tetris

#endif //MPTETRIS_APPCOMPONENT_H