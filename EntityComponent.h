//
// Created by pauli on 2/3/2023.
//

#ifndef MPTETRIS_ENTITYCOMPONENT_H
#define MPTETRIS_ENTITYCOMPONENT_H

#include "AppComponent.h"

namespace Tetris {

    // Forward declare App
    class App;

    class EntityComponent : public UpdatingComponent {
    public:
        EntityComponent(Tetris::App *app, const std::string &id, const std::string &texture, int x, int y, int width,
                        int height);

        void setX(int x);

        void setY(int y);

    protected:
        TextureHolder holder;
    };

    class ExampleEntity : public EntityComponent {
    public:
        ExampleEntity(Tetris::App *app, const std::string &id, int x, int y, int width, int height);

        void update(float timestep) override;

        void render(SDL_Renderer *renderer, float timestep) override;

        const static int TEXTURE_SIZE = 512;
    };
}

#endif //MPTETRIS_ENTITYCOMPONENT_H
