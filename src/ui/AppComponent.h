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
        /**
         * Constructor for AppComponent
         * @param app Pointer to the App
         * @param id ID representing this component
         * @param x X position of component
         * @param y Y position of component
         * @param width Width of component
         * @param height Height of component
         */
        AppComponent(Tetris::App * app, const std::string & id, int x, int y, int width, int height) : app(app), id(id), x(x), y(y), width(width), height(height), enabled(true) {};

        /**
         * Gets the enabled state of this component
         * @return True if this is running, false otherwise
         */
        bool isEnabled() const;

        /**
         * Renders this component, given the renderer and the timestep
         * @param renderer Pointer to the renderer
         * @param timestep Timestep since last frame
         */
        virtual void render(SDL_Renderer * renderer, float timestep) = 0;

        /**
         * Gets the x-position of this component
         * @return X-Pos
         */
        float getX() const;

        /**
         * Gets the y-position of this component
         * @return Y-Pos
         */
        float getY() const;

        /**
         * Gets the width of this component
         * @return Width
         */
        float getWidth() const;

        /**
         * Gets the height of this component
         * @return Height
         */
        float getHeight() const;
    protected:
        bool enabled;
        Tetris::App * app;
        float x,y,width,height;
        std::string id;
    };

    // Basic component implementations done here. These are the most common components that will be used in the game

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
        ButtonComponent(Tetris::App *app, const std::string & id, const std::string & texture, int x, int y, int width, int height, bool isTextButton = false);
        void render(SDL_Renderer * renderer, float timestep) override;
        void update(float timestep) override;

        void onClick(std::function<void()> callback) { onClickCallback = std::move(callback); }
        void click();
    protected:
        std::function<void()> onClickCallback = [&](){ };

        std::string buttonText;
        FontHolder fontHolder;
        TextureHolder textureHolder;
    };

} // Tetris

#endif //MPTETRIS_APPCOMPONENT_H
