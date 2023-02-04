//
// Created by pauli on 2/3/2023.
//
#include "AppComponent.h"

#define FPS_SAMPLE_SIZE 1000

namespace Tetris {

    TextComponent::TextComponent(Tetris::App *app, const std::string &id, int x, int y, int width, int height,
                                 const std::string &text, const std::string &fontName)
            : AppComponent(app, id, x, y, width, height) {
        this->text = text;
        this->holder = app->getFont(fontName);
        this->color = {255, 255, 255, 255};
    }

    void TextComponent::setText(const std::string &text) {
        this->text = text;
    }

    void TextComponent::render(SDL_Renderer *renderer, float timestep) {
        TTF_Font *font = holder.font;
        SDL_Rect pos = {x, y, width, height};
        renderText(renderer, font, text, color, &pos);
    }

    void FPSComponent::update(float timestep) {
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
        SDL_Rect pos = {x, y, width, height};
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

    UpdatingComponent::UpdatingComponent(Tetris::App *app, const std::string &id, int x, int y, int width, int height)
            : AppComponent(app, id, x, y, width, height) {
    }
} // Tetris