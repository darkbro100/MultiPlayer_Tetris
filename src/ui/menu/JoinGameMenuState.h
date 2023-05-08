//
// Created by pauli on 4/30/2023.
//

#ifndef MPTETRIS_JOINGAMEMENUSTATE_H
#define MPTETRIS_JOINGAMEMENUSTATE_H

#include "MenuState.h"
#include <unordered_map>
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
        void attemptConnect();
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
        InputHolder inputs{};

        FontHolder font, smallFont;
        TextureHolder texture;
        std::mt19937 engine;

        std::unordered_map<uint32_t, std::shared_ptr<Player>> players;
        std::vector<uint32_t> placements;

        bool gameStarted = false;

        std::vector<unsigned int> lines;
    };
} // Tetris

#endif //MPTETRIS_JOINGAMEMENUSTATE_H
