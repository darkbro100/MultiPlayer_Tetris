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

    /**
     * Represents a client who is connecting to an existing host. At first, it will wait for you to type in the IP address of the host. Once done, you hit enter and it will attempt to connect to the server. On fail or disconnect, you will be brought back to the main menu.
     */
    class JoinGameMenuState : public MenuState {
    public:
        explicit JoinGameMenuState(App
                                   *app);

        ~JoinGameMenuState();

        void loadComponents() override;

        void update(float ts) override;

        void render(SDL_Renderer *renderer, float ts) override;

    private:
        /**
         * Called when we should attempt to connect to the server. Will use the input
         */
        void attemptConnect();

        /**
         * Called when this client has received a message from the server.
         * @param msg Reference to the message that was received
         */
        void onMessage(NetworkMessage & msg);

        /**
         * Called when we receive a ping message. will bounce it back to the server
         * @param message Ping message received
         */
        void onPingReceive(NetworkMessage& message);

        /**
         * Called when another client has connected to the server
         * @param who Reference to the pointer of the connected client
         */
        void onConnect(std::shared_ptr<NetworkClient> & who);

        /**
         * Called when another client has disconnected from the server
         * @param who Reference to the pointer of who disconnected
         */
        void onDisconnect(std::shared_ptr<NetworkClient> & who);

        // Net related stuff
        ConcurrentQueue<OwnedNetworkMessage> incomingMessages{};
        asio::io_context io_context;
        std::shared_ptr<NetworkClient> client;
        std::atomic_int8_t connectionStatus;
        std::atomic<uint16_t> ping;

        // Game related stuff
        InputHolder inputs{};

        // UI related stuff
        FontHolder font, smallFont;
        TextureHolder texture;

        // RNG
        std::mt19937 engine;

        // Map of players, vector containing the placements of said players
        std::unordered_map<uint32_t, std::shared_ptr<Player>> players;
        std::vector<uint32_t> placements;

        // Whether or not the game has started
        bool gameStarted = false;

        /**
         * Used to keep track of attempting to connect
         */
        bool lastReturnPress;

        // Lines cleared by this player
        std::vector<unsigned int> lines;
    };
} // Tetris

#endif //MPTETRIS_JOINGAMEMENUSTATE_H
