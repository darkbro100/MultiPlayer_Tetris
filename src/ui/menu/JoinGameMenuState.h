//
// Created by pauli on 4/30/2023.
//

#ifndef MPTETRIS_JOINGAMEMENUSTATE_H
#define MPTETRIS_JOINGAMEMENUSTATE_H

#include "MenuState.h"
#include <map>
#include "../../game/GameCommon.h"
#include "../../net/NetworkClient.h"
#include "../../net/NetworkMessage.h"
#include "../../net/ConcurrentQueue.h"
#include "asio.hpp"
#include <random>

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
        void onMessage(NetworkMessage & msg);
        void onPingReceive(NetworkMessage& message);
        void onConnect(std::shared_ptr<NetworkClient> & who);
        void onDisconnect(std::shared_ptr<NetworkClient> & who);

        bool lastDownPress = false, lastReturnPress = false;
        std::vector<std::shared_ptr<ButtonComponent>> buttons;
        int selectedButton = 0;

        // Net related stuff
        ConcurrentQueue<OwnedNetworkMessage> incomingMessages{};
        asio::io_context io_context;
        std::shared_ptr<NetworkClient> client;

        std::atomic_int8_t connectionStatus;
        std::atomic<uint16_t> ping;

        FontHolder font, smallFont;
        TextureHolder texture;
        std::mt19937 engine;

        std::map<uint32_t, Player> players;
        bool gameStarted = false;
    };
} // Tetris

#endif //MPTETRIS_JOINGAMEMENUSTATE_H
