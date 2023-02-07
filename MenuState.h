//
// Created by pauli on 2/6/2023.
//

#ifndef MPTETRIS_MENUSTATE_H
#define MPTETRIS_MENUSTATE_H

#include "EntityComponent.h"
#include <memory>

namespace Tetris {

    class App;

    class MenuState {
    public:
        explicit MenuState(App * app);

        /**
         * Loads all the components for this menu state. Called when the menu state is first initialized
         */
        virtual void loadComponents();

        /**
         * Updates all the components for this menu state every frame
         */
        virtual void update(float ts);

        /*
         * Renders all the components for this menu state every frame
         */
        virtual void render(SDL_Renderer * renderer, float ts);
    protected:
        App * app;
        std::list<std::shared_ptr<AppComponent>> components;
    };

    class MainMenuState : public MenuState {
    public:
        explicit MainMenuState(App *app);
        void loadComponents() override;
        void update(float ts) override;
        void render(SDL_Renderer *renderer, float ts) override;
    };
}

#endif //MPTETRIS_MENUSTATE_H
