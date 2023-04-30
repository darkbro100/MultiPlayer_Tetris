//
// Created by pauli on 4/30/2023.
//

#ifndef MPTETRIS_HOSTGAMEMENUSTATE_H
#define MPTETRIS_HOSTGAMEMENUSTATE_H

#include "MenuState.h"
#include "../../net/NetworkServer.h"

namespace Tetris {

    class HostGameMenuState : public MenuState {
    public:
        explicit HostGameMenuState(App
                                   *app);

        void loadComponents() override;

        void update(float ts) override;

        void render(SDL_Renderer *renderer, float ts) override;

    private:
        NetworkServer server;
        bool lastDownPress = false, lastReturnPress = false;
        std::vector<std::shared_ptr<ButtonComponent>> buttons;
        int selectedButton = 0;
    };
} // Tetris

#endif //MPTETRIS_HOSTGAMEMENUSTATE_H
