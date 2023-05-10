//
// Created by pauli on 4/30/2023.
//

#ifndef MPTETRIS_HOSTGAMEMENUSTATE_H
#define MPTETRIS_HOSTGAMEMENUSTATE_H

#include "../../game/GameCommon.h"
#include <unordered_map>
#include "MenuState.h"
#include "../../net/NetworkServer.h"
#include <random>

namespace Tetris {

    /**
     * Represents a Menu where the user is hosting a game as well as playing, so we must ensure that we have a server running as well as having our own game instance running to send to the other clients
     * Initially the game will start in a waiting phase, where we are waiting for clients to connect. Once everyone has connected, the host can press enter/return to start the game.
     */
    class HostGameMenuState : public MenuState {
    public:
        explicit HostGameMenuState(App
                                   *app);

        void loadComponents() override;

        void update(float ts) override;

        void render(SDL_Renderer *renderer, float ts) override;
    private:
        /**
         * Called when a client attempts to connect to the server
         * @param client Client attempting to connect
         * @return True if they should connect, false otherwise
         */
        bool onPreConnect(std::shared_ptr<NetworkClient> & client);
        
        /**
         * Called when a message is received from a client
         * @param client Client who sent the message
         * @param msg Msg that the client had sent
         */
        void onMessage(std::shared_ptr<NetworkClient> & client, NetworkMessage & msg);
        
        /**
         * Called when a client disconnects from the server
         * @param client Handler to call
         */
        void onDisconnect(std::shared_ptr<NetworkClient> & client);
        
        /**
         * Called when a client connects to the server
         * @param client Handler to call
         */
        void onConnect(std::shared_ptr<NetworkClient> & client);

        /**
         * Called when a ping message is received from a client. We will simply calculate the ping, bounce the message back
         * @param client Client who sent it
         * @param message Ping msg
         */
        void onPingReceive(std::shared_ptr<NetworkClient>& client, NetworkMessage & message);

        /**
         * Holds the textures/fonts needed for this menu
         */
        FontHolder holder;
        TextureHolder texture;

        /**
         * Holds the server
         */
        NetworkServer server;

        /**
         * Holds the inputs being pressed
         */
        InputHolder inputs{};

        /**
         * Holds the PRNG state
         */
        std::mt19937 engine;

        /**
         * Map containing all the connected players
         */
        std::unordered_map<uint32_t, std::shared_ptr<Player>> players;

        /**
         * Whether or not the host has pressed enter/return to start the game
         */
        bool lastReturnPress;

        /**
         * Whether or not the Multiplayer game has started
         */
        bool gameStarted = false;

        /**
         * Timer for after the game has ended, how long till we go back to the lobby
         */
        float gameOverTimer = 0;

        /**
         * Vector containing all the players who have lost (back of the list contains who lost first)
         */
        std::vector<uint32_t> placements;

        /**
         * Vector containing all the lines that have been cleared by the host
         */
        std::vector<unsigned int> lines;
    };
} // Tetris

#endif //MPTETRIS_HOSTGAMEMENUSTATE_H
