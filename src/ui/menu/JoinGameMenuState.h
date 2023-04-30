//
// Created by pauli on 4/30/2023.
//

#ifndef MPTETRIS_JOINGAMEMENUSTATE_H
#define MPTETRIS_JOINGAMEMENUSTATE_H

#include "MenuState.h"
#include "../../net/NetworkClient.h"
#include "../../net/NetworkMessage.h"
#include "../../net/ConcurrentQueue.h"
#include "asio.hpp"

namespace Tetris {

    class JoinGameMenuState : public MenuState {
    public:
        explicit JoinGameMenuState(App
                                   *app);

        ~JoinGameMenuState();

        void loadComponents() override;

        void update(float ts) override;

        void render(SDL_Renderer *renderer, float ts) override;

    private:
        bool lastDownPress = false, lastReturnPress = false;
        std::vector<std::shared_ptr<ButtonComponent>> buttons;
        int selectedButton = 0;

        // Net related stuff
        ConcurrentQueue<OwnedNetworkMessage> incomingMessages{};
        asio::io_context io_context;
        std::shared_ptr<NetworkClient> client;
    };
} // Tetris

#endif //MPTETRIS_JOINGAMEMENUSTATE_H
