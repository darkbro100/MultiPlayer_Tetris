//
// Created by pauli on 2/3/2023.
//

#include "EntityComponent.h"
#include "TetrisApp.h"
#include "TetrisApp.h"

namespace Tetris {

    EntityComponent::EntityComponent(Tetris::App *app, const std::string & id,const std::string &texture, int x, int y, int width, int height)
            : UpdatingComponent(app, id, x, y, width, height) {
        this->holder = app->getTexture(texture);
    }

    void EntityComponent::setX(int x) {
        this->x = x;
    }

    void EntityComponent::setY(int y) {
        this->y = y;
    }

    ExampleEntity::ExampleEntity(Tetris::App *app, const std::string &id, int x, int y, int width, int height)
            : EntityComponent(app, id, "tetronimo", x, y, width, height) { }

    void ExampleEntity::update(float timestep) {
        // move the entity
        if (app->isKeyPressed(SDL_SCANCODE_W) || app->isKeyPressed(SDL_SCANCODE_UP))
            y -= App::ENTITY_SPEED * timestep;
        if (app->isKeyPressed(SDL_SCANCODE_A) || app->isKeyPressed(SDL_SCANCODE_LEFT))
            x -= App::ENTITY_SPEED * timestep;
        if (app->isKeyPressed(SDL_SCANCODE_S) || app->isKeyPressed(SDL_SCANCODE_DOWN))
            y += App::ENTITY_SPEED * timestep;
        if (app->isKeyPressed(SDL_SCANCODE_D) || app->isKeyPressed(SDL_SCANCODE_RIGHT))
            x += App::ENTITY_SPEED * timestep;

        // right boundary
        if (x + width > App::WINDOW_WIDTH)
            x = App::WINDOW_WIDTH - width;

        // left boundary
        if (x < 0)
            x = 0;

        // bottom boundary
        if (y + height > App::WINDOW_HEIGHT)
            y = App::WINDOW_HEIGHT - height;

        // upper boundary
        if (y < 0)
            y = 0;
    }

    void ExampleEntity::render(SDL_Renderer *renderer, float timestep) {
        SDL_Rect src = {0, 0, TEXTURE_SIZE, TEXTURE_SIZE};
        SDL_FRect entityPos = {x, y, width, height};

        SDL_RenderCopyF(renderer, holder.texture, &src, &entityPos);
    }
}
