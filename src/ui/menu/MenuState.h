//
// Created by pauli on 2/6/2023.
//

#ifndef MPTETRIS_MENUSTATE_H
#define MPTETRIS_MENUSTATE_H

#include "../AppComponent.h"
#include "SDL.h"
#include <vector>
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

    /**
     * Represents the Main Menu of our application. Will simply have 2 buttons for SinglePlayer and Multiplayer, player can select each one using the arrow keys and then press enter/return to go into the game.
     */
    class MainMenuState : public MenuState {
    public:
        explicit MainMenuState(App *app);
        void loadComponents() override;
        void update(float ts) override;
        void render(SDL_Renderer *renderer, float ts) override;
    private:
        bool lastDownPress = false, lastReturnPress = false;
        std::vector<std::shared_ptr<ButtonComponent>> buttons;
        int selectedButton = 0;
    };
}

#endif //MPTETRIS_MENUSTATE_H
