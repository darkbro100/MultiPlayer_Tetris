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
        serverPlayer.piece.current = 5;
        serverPlayer.piece.next = engine() % 7;
        serverPlayer.pos.x = FIELD_WIDTH / 2;
        serverPlayer.pos.y = 0;
        serverPlayer.pos.rotation = 0;
        serverPlayer.pos.storedRotation = 0;
        serverPlayer.speed.current = 0.7f;
        serverPlayer.speed.timer = 0.0f;
        serverPlayer.lines = 0;
        serverPlayer.score = 0;
        initField(serverPlayer.field);
        serverPlayer.id = 1;
        players[1] = serverPlayer;

        // Ensure input holder is setup
        inputs.leftDelay = INPUT_DELAY;
        inputs.rightDelay = INPUT_DELAY;
        inputs.downDelay = INPUT_DELAY;

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
//        if (players.size() <= 1) {
//            return;
//        }

        if (!gameStarted && isRetPressed && !wasRetPressed) {
            gameStarted = true;

            // Send the start game message to all the clients
            NetworkMessage startGameMessage;
            startGameMessage.header.type = MessageType::GAME_START;
            server.sendMessageToAll(startGameMessage);
            return;
        }

        if (gameStarted) {
            Player &player = players[1];
            if (!player.gameOver) {

                // Check if any lines are happening. If there are, don't let anything else happen, shift the lines down
                if (!lines.empty()) {
                    player.speed.timer += ts;
                    if (player.speed.timer >= player.speed.current) {
                        for (auto &line: lines) {
                            for (int x = 1; x < FIELD_WIDTH - 1; x++) {
                                int index = line * FIELD_WIDTH + x;
                                player.field[index] = EMPTY_ID;
                            }

                            for (int y = line; y > 0; y--) {
                                for (int x = 1; x < FIELD_WIDTH - 1; x++) {
                                    int index = y * FIELD_WIDTH + x;
                                    player.field[index] = player.field[index - FIELD_WIDTH];
                                }
                            }
                        }

                        // Update lines cleared & clear the vector
                        player.lines += lines.size();
                        player.score += (1 << lines.size()) * 100;
                        lines.clear();

                        // Update timings to be the normal game timings TODO: store previous timings
                        player.speed.timer = 0.0f;
                        player.speed.current = 0.7f;
                    }

                    return;
                }

                // Input handling
                checkInputs(inputs, app, player.piece, player.pos, player.speed, player.field);

                player.speed.timer += ts;
                if (player.speed.timer >= player.speed.current) {
                    player.speed.timer = 0.0f;

                    // Check if can move down
                    if (Tetromino::canFit(player.piece.current, player.pos.x, player.pos.y + 1, player.pos.rotation,
                                          player.field)) {
                        player.pos.y++;
                    } else {
                        // Place the piece
                        Tetromino::place(player.piece.current, player.pos.x, player.pos.y, player.pos.rotation,
                                         player.field);

                        // Check for lines
                        for (int y = 0; y < Tetromino::TETROMINO_SIZE; y++) {
                            if (player.pos.y + y >= FIELD_HEIGHT - 1)
                                continue;

                            bool line = true;

                            // If a line has an empty slot
                            for (int x = 1; x < FIELD_WIDTH - 1; x++) {
                                int index = (y + player.pos.y) * FIELD_WIDTH + x;
                                if (player.field[index] == EMPTY_ID) {
                                    line = false;
                                }
                            }

                            if (line) {
                                lines.push_back(player.pos.y + y);
                                player.speed.timer = 0.0f;
                                player.speed.current = 0.25f;
                            }
                        }

                        // Generate new piece
                        player.piece.current = player.piece.next;
                        player.piece.next = engine() % 7;
                        player.pos.x = FIELD_WIDTH / 2;
                        player.pos.y = 0;
                        player.pos.rotation = 0;
                        player.pos.storedRotation = 0;
                        player.speed.timer = 0.0f;
                        player.piece.hasStored = false;
                        inputs.leftDelay = INPUT_DELAY;
                        inputs.rightDelay = INPUT_DELAY;
                        inputs.downDelay = INPUT_DELAY;

                        if (!Tetromino::canFit(player.piece.current, player.pos.x, player.pos.y, player.pos.rotation,
                                               player.field))
                            player.gameOver = true;
                    }
                }
            }

            // Send our game state
            NetworkMessage gameStateMessage;
            gameStateMessage.header.type = MessageType::PLAYER_UPDATE;
            gameStateMessage << player;
            server.sendMessageToAll(gameStateMessage);
        }
    }

    void HostGameMenuState::render(SDL_Renderer *renderer, float ts) {
        MenuState::render(renderer, ts);

        // Render all the players in
        if (!gameStarted)
            renderPlayers(renderer, players, holder);
        else {
            /*
             * First, we need to render the server's actual game state. Afterwards, we can render all the other players that are in the lobby (at a much scaled down rate).
             * We also want to display the server's field much smaller relative to how we do it in singleplayer mode.
             */
            int startX = 50;
            int startY = 150;
            int blockSize = 25;
            Player &serverPlayer = players[1];

            renderField(renderer, texture, startX, startY, serverPlayer.field, lines, serverPlayer.gameOver, blockSize);

            // Then render the server's current piece and other values
            // Render current piece in
            renderPiece(renderer, texture, serverPlayer.pos.x, serverPlayer.pos.y, serverPlayer.piece.current,
                        serverPlayer.pos.rotation,
                        startX, startY, 255, blockSize);

            // Render "ghost" piece (where it will land)
            int ghostY = serverPlayer.pos.y;
            while (Tetromino::canFit(serverPlayer.piece.current, serverPlayer.pos.x, ghostY + 1,
                                     serverPlayer.pos.rotation,
                                     serverPlayer.field)) {
                ghostY++;
            }
            renderPiece(renderer, texture, serverPlayer.pos.x, ghostY, serverPlayer.piece.current,
                        serverPlayer.pos.rotation, startX,
                        startY, 50, blockSize);

//            // Render the next piece under the score
//            SDL_Rect textRect = {startX + (FIELD_WIDTH * CELL_SIZE) + 10, startY + 200, 150, 75};
//            renderText(renderer, holder.font, "Next", {255, 255, 255, 255}, &textRect);
//            renderPiece(renderer, texture, 0, 0, serverPlayer.piece.next, 0, startX + (FIELD_WIDTH * CELL_SIZE) + 10,
//                        startY + 225, 100, blockSize);
//
//            // Render the stored piece directly under the next piece
//            if (serverPlayer.storedPiece != INVALID_SHAPE) {
//                textRect = {startX + (FIELD_WIDTH * CELL_SIZE) + 10, startY + 400, 150, 75};
//                renderText(renderer, fontHolder.font, "Stored", {255, 255, 255, 255}, &textRect);
//                renderPiece(renderer, holder, 0, 0, serverPlayer.storedPiece, 0, startX + (FIELD_WIDTH * CELL_SIZE) + 10,
//                            startY + 425, 100);
//            }

            startX = startX + (FIELD_WIDTH * blockSize) + 20;
            std::vector<unsigned int> oLines;

            // Render the other players in the server
            for (auto &it: players) {
                // ignore if it's the server (we rendered it above)
                if (it.first == 1) continue;
                Player player = it.second;

                // Render the field
                renderField(renderer, texture, startX, startY, player.field, oLines, player.gameOver, blockSize / 2);

                // Render current piece in
                renderPiece(renderer, texture, player.pos.x, player.pos.y, player.piece.current,
                            player.pos.rotation, startX, startY, 255, blockSize / 2);

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