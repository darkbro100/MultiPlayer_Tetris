//
// Created by pauli on 4/30/2023.
//

#include <chrono>
#include <utility>
#include "../../net/NetworkClient.h"
#include "../../net/NetworkMessage.h"
#include "../../TetrisApp.h"
#include "HostGameMenuState.h"

namespace Tetris {

    HostGameMenuState::HostGameMenuState(App *app) : server(25000), MenuState(app) {
    }

    void HostGameMenuState::loadComponents() {
        MenuState::loadComponents();

        texture = app->getTexture("tetronimo");
        holder = app->getFont("opensans_large");

        std::random_device dev;
        engine.seed(dev());

        Player serverPlayer{};
        serverPlayer.currentPiece = 5;
        serverPlayer.nextPiece = engine() % 7;
        serverPlayer.currentX = FIELD_WIDTH / 2;
        serverPlayer.currentY = 0;
//        serverPlayer.currentRotation = 0;
//        serverPlayer.storedRotation = 0;
        serverPlayer.currentSpeed = 0.7f;
//        serverPlayer.currentSpeedTimer = 0.0f;
//        serverPlayer.linesCreated = 0;
//        serverPlayer.score = 0;
        initField(serverPlayer.field);
        serverPlayer.id = 1;

        players[1] = serverPlayer;

        // Ensure this is updated in case the user is already holding return
        lastReturnPress = app->isKeyPressed(SDL_SCANCODE_RETURN);

        // Start the server
        server.onClientDisconnect([this](std::shared_ptr<NetworkClient> client) { onDisconnect(client); });
        server.onClientPreConnect([this](std::shared_ptr<NetworkClient> client) { return onPreConnect(client); });
        server.onClientConnect([this](std::shared_ptr<NetworkClient> client) { onConnect(client); });
        server.onMessageReceive(
                [this](std::shared_ptr<NetworkClient> client, NetworkMessage &message) { onMessage(client, message); });
        server.start();
    }

    void HostGameMenuState::update(float ts) {
        MenuState::update(ts);
        server.update(true);

        // Check and determine which button is currently being selected
        bool isRetPressed = app->isKeyPressed(SDL_SCANCODE_RETURN);
        bool wasRetPressed = this->lastReturnPress;
        this->lastReturnPress = isRetPressed;

        bool isPressed = app->isKeyPressed(SDL_SCANCODE_DOWN) || app->isKeyPressed(SDL_SCANCODE_UP);
        bool wasPressed = this->lastDownPress;
        this->lastDownPress = isPressed;

        // Check if we have enough players to start
        if (players.size() <= 1) {
            return;
        }

        if(!gameStarted && isRetPressed && !wasRetPressed) {
            gameStarted = true;

            // Send the start game message to all the clients
            NetworkMessage startGameMessage;
            startGameMessage.header.type = MessageType::GAME_START;
            server.sendMessageToAll(startGameMessage);

            return;
        }

        if(gameStarted) {

            // Send our game state
            NetworkMessage gameStateMessage;
            gameStateMessage.header.type = MessageType::PLAYER_UPDATE;
            gameStateMessage << players[1];
            server.sendMessageToAll(gameStateMessage);
        }
    }

    void HostGameMenuState::render(SDL_Renderer *renderer, float ts) {
        MenuState::render(renderer, ts);

        // Render all the players in
        if(!gameStarted)
            renderPlayers(renderer, players, holder);
        else {
            /*
             * First, we need to render the server's actual game state. Afterwards, we can render all the other players that are in the lobby (at a much scaled down rate).
             * We also want to display the server's field much smaller relative to how we do it in singleplayer mode.
             */
            int startX = 50;
            int startY = 150;
            int blockSize = 25;
            std::vector<unsigned int> lines;
            renderField(renderer, texture, startX, startY, players[1].field, lines, players[1].gameOver, blockSize);

            startX = startX + (FIELD_WIDTH * blockSize) + 20;

            // Render the other players in the server
            for(auto & player : players) {
                if(player.first == 1) continue;

                renderField(renderer, texture, startX, startY, player.second.field, lines, player.second.gameOver, blockSize / 2);
                startX += (FIELD_WIDTH * blockSize / 2) + 20;
            }
        }
    }

    void HostGameMenuState::onPingReceive(std::shared_ptr<NetworkClient> &client, NetworkMessage &message) {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        std::chrono::high_resolution_clock::time_point then;
        message >> then;

        // calc ping
        uint16_t pingData = std::chrono::duration_cast<std::chrono::milliseconds>(now - then).count();

        // update ping for the server
        players[client->getId()].ping = pingData;

        // network message to send to all clients
        NetworkMessage toSend;
        toSend.header.type = MessageType::KEEP_ALIVE;
        toSend << now;
        NetworkMessage pingDataMessage;
        pingDataMessage.header.type = MessageType::PING;
        pingDataMessage << client->getId();
        pingDataMessage << pingData;
        server.sendMessageTo(toSend, client);
        server.sendMessageToAll(pingDataMessage);
    }

    bool HostGameMenuState::onPreConnect(std::shared_ptr<NetworkClient> &client) {
        return true;
    }

    void HostGameMenuState::onConnect(std::shared_ptr<NetworkClient> &client) {
        std::cout << "[HGMS] Client connected: " << client->getId() << std::endl;

        // Add to player map
        Player player{};
        player.id = client->getId();
        initField(player.field);

        players[client->getId()] = player;

        NetworkMessage message;
        message.header.type = MessageType::CONNECT;
        message << client->getId();

        NetworkMessage assignId;
        assignId.header.type = MessageType::ASSIGN_ID;
        assignId << client->getId();

        // Send a test ping to the client, will maintain the RTT for the client
        std::chrono::high_resolution_clock::time_point timestamp = std::chrono::high_resolution_clock::now();
        NetworkMessage pingMessage;
        pingMessage.header.type = MessageType::KEEP_ALIVE;
        pingMessage << timestamp;

        NetworkMessage serverMsg;
        serverMsg.header.type = MessageType::CONNECT;
        serverMsg << 1;

        server.sendMessageToAll(message);
        server.sendMessageTo(assignId, client);
        server.sendMessageTo(pingMessage, client);
        server.sendMessageTo(serverMsg, client);
    }

    void HostGameMenuState::onDisconnect(std::shared_ptr<NetworkClient> &client) {
        std::cout << "[HGMS] Client disconnected: " << client->getId() << std::endl;

        // Remove from players map
        players.erase(client->getId());

        NetworkMessage message;
        message.header.type = MessageType::DISCONNECT;
        message << client->getId();

        server.sendMessageToAll(message, client->getId());
    }

    void HostGameMenuState::onMessage(std::shared_ptr<NetworkClient> &client, NetworkMessage &msg) {
        switch (msg.header.type) {
            case MessageType::KEEP_ALIVE: {
                onPingReceive(client, msg);
                break;
            }
            case MessageType::PLAYER_UPDATE: {
                server.sendMessageToAll(msg, client->getId());
                break;
            }
            default:
                std::cout << "[HGMS] Received unknown message: " << msg << std::endl;
        }
    }

} // Tetris