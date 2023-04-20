//
// Created by pauli on 2/3/2023.
//
#include "AppComponent.h"
#include "TetrisApp.h"

namespace Tetris {

    TextComponent::TextComponent(Tetris::App *app, const std::string &id, int x, int y, int width, int height,
                                 const std::string &text, const std::string &fontName)
            : AppComponent(app, id, x, y, width, height) {
        this->text = text;
        this->holder = app->getFont(fontName);
        this->color = {255, 255, 255, 255};
    }

    UpdatingComponent::UpdatingComponent(Tetris::App *app, const std::string &id, int x, int y, int width, int height)
            : AppComponent(app, id, x, y, width, height) {}

    void TextComponent::setText(const std::string &text) {
        this->text = text;
    }

    void TextComponent::render(SDL_Renderer *renderer, float timestep) {
        TTF_Font *font = holder.font;
        SDL_FRect pos = {x, y, width, height};
        renderText(renderer, font, text, color, &pos);
    }

    void FPSComponent::update(float timestep) {
        bool isPressed = app->isKeyPressed(SDL_SCANCODE_F3);
        bool wasPressed = this->wasPressed;
        this->wasPressed = isPressed;

        if (isPressed && !wasPressed)
            this->enabled = !this->enabled;

        float fps = 1.0f / timestep;
        fpsCalcs.push_back(fps);
        if (fpsCalcs.size() > FPS_SAMPLE_SIZE) {
            fpsCalcs.pop_front();
            float totalFps = 0.0f;

            for (auto f: fpsCalcs)
                totalFps += f;

            averageFps = totalFps / FPS_SAMPLE_SIZE;
        }

        currentFps = fps;
    }

    void FPSComponent::render(SDL_Renderer *renderer, float timestep) {
        SDL_FRect pos = {x, y, width, height};
        renderText(renderer, fontHolder.font, "FPS: " + std::to_string(currentFps), {255, 255, 255, 255}, &pos);
        pos = {x, y + height, width, height};
        renderText(renderer, fontHolder.font, "FPS (avg): " + std::to_string(averageFps), {255, 255, 255, 255}, &pos);
    }

    FPSComponent::FPSComponent(Tetris::App *app, const std::string &id, int x, int y, int width, int height)
            : UpdatingComponent(app, id, x, y, width, height) {
        averageFps = 0.0f;
        currentFps = 0.0f;
        fontHolder = app->getFont("opensans");
    }

    bool AppComponent::isEnabled() const {
        return enabled;
    }

    float AppComponent::getX() const {
        return x;
    }

    float AppComponent::getY() const {
        return y;
    }

    float AppComponent::getWidth() const {
        return width;
    }

    float AppComponent::getHeight() const {
        return height;
    }

    ButtonComponent::ButtonComponent(Tetris::App *app, const std::string &id, const std::string &texture, int x, int y,
                                     int width, int height) : UpdatingComponent(app, id, x, y, width, height) {
        this->holder = app->getTexture(texture);
    }

    void ButtonComponent::render(SDL_Renderer *renderer, float timestep) {
        SDL_FRect dest = {x - (width / 2), y - height, width, height};
        SDL_RenderCopyF(renderer, holder.texture, nullptr, &dest);
    }

    void ButtonComponent::update(float timestep) { }

    void ButtonComponent::click() {
        if (onClickCallback != nullptr)
            onClickCallback();
    }

} // Tetris