//
// Created by pauli on 4/30/2023.
//

#ifndef MPTETRIS_HOSTGAMEMENUSTATE_H
#define MPTETRIS_HOSTGAMEMENUSTATE_H

#include "../../game/GameCommon.h"
#include <map>
#include "MenuState.h"
#include "../../net/NetworkServer.h"
#include <random>

namespace Tetris {

    class HostGameMenuState : public MenuState {
    public:
        explicit HostGameMenuState(App
                                   *app);

        void loadComponents() override;

        void update(float ts) override;

        void render(SDL_Renderer *renderer, float ts) override;
    private:
        bool onPreConnect(std::shared_ptr<NetworkClient> & client);
        void onMessage(std::shared_ptr<NetworkClient> & client, NetworkMessage & msg);
        void onDisconnect(std::shared_ptr<NetworkClient> & client);
        void onConnect(std::shared_ptr<NetworkClient> & client);

        void onPingReceive(std::shared_ptr<NetworkClient>& client, NetworkMessage & message);

        FontHolder holder;
        TextureHolder texture;
        NetworkServer server;
        bool lastDownPress = false, lastReturnPress = false;
        std::vector<std::shared_ptr<ButtonComponent>> buttons;
        int selectedButton = 0;
        std::mt19937 engine;

        std::map<uint32_t, Player> players;
        bool gameStarted = false;
    };
} // Tetris

#endif //MPTETRIS_HOSTGAMEMENUSTATE_H
