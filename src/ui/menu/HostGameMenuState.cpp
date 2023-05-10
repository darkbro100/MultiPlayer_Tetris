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

        std::shared_ptr<Player> serverPlayer = std::make_shared<Player>();
        resetPiece(serverPlayer->piece, serverPlayer->pos, serverPlayer->speed, inputs, engine);
        initField(serverPlayer->field);
        serverPlayer->speed.timer = 0.0f;
        serverPlayer->speed.current = 0.7f;
        serverPlayer->id = 1;

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

        // Check if we have enough players to start
        if (players.size() <= 1) {
            return;
        }

        if (!gameStarted && isRetPressed && !wasRetPressed) {
            gameStarted = true;

            // Send the start game message to all the clients
            NetworkMessage startGameMessage;
            startGameMessage.header.type = MessageType::GAME_START;
            server.sendMessageToAll(startGameMessage);
            return;
        }

        if(gameOverTimer > 0) {
            gameOverTimer -= ts;

            if(gameOverTimer <= 0) {
                // Send the start game message to all the clients
                NetworkMessage endGameMsg;
                endGameMsg.header.type = MessageType::GAME_END;
                server.sendMessageToAll(endGameMsg);

                resetPiece(players[1]->piece, players[1]->pos, players[1]->speed, inputs, engine);
                initField(players[1]->field);
                players[1]->gameOver = false;

                placements.clear();
                gameStarted = false;
                gameOverTimer = 0.0f;
            }

            return;
        }

        if (gameStarted && gameOverTimer <= 0) {

            size_t numPlayers = players.size();
            size_t numPlacements = placements.size();

            if(numPlayers - numPlacements <= 1) {
                // Determine the winner
                uint32_t winner = 0;
                for (auto &it : players) {
                    if (!it.second->gameOver) {
                        winner = it.second->id;
                        break;
                    }
                }

                // add to list
                std::shared_ptr<Player> player = players[winner];
                player->gameOver = true;
                placements.push_back(player->id);

                // Send the winner message
                NetworkMessage winnerMsg;
                winnerMsg.header.type = MessageType::PLAYER_WON;
                winnerMsg << winner;
                server.sendMessageToAll(winnerMsg);
                gameOverTimer = 5.0f;

                return;
            }

            std::shared_ptr<Player> player = players[1];
            if (!player->gameOver) {

                // Check if any lines are happening. If there are, don't let anything else happen, shift the lines down
                if (!lines.empty()) {
                    player->speed.timer += ts;
                    if (player->speed.timer >= player->speed.current) {
                        clearLines(player->field, lines);

                        // Update lines cleared & clear the vector
                        player->lines += lines.size();
                        player->score += (1 << lines.size()) * 100;
                        lines.clear();

                        // Update timings to be the normal game timings TODO: store previous timings
                        player->speed.timer = 0.0f;
                        player->speed.current = 0.7f;

                        // Send update message
                        NetworkMessage gameStateMessage;
                        gameStateMessage.header.type = MessageType::PLAYER_UPDATE;
                        gameStateMessage << *player;
                        server.sendMessageToAll(gameStateMessage);
                    }

                    return;
                }

                // Input handling
                int16_t x = player->pos.x, y = player->pos.y;
                checkInputs(inputs, app, player->piece, player->pos, player->speed, player->field, engine);
                if(x != player->pos.x || y != player->pos.y) {
                    // Send update message
                    NetworkMessage gameStateMessage;
                    gameStateMessage.header.type = MessageType::PLAYER_UPDATE;
                    gameStateMessage << *player;
                    server.sendMessageToAll(gameStateMessage);
                }

                player->speed.timer += ts;
                if (player->speed.timer >= player->speed.current) {
                    player->speed.timer = 0.0f;

                    // Check if can move down
                    if (Tetromino::canFit(player->piece.current, player->pos.x, player->pos.y + 1, player->pos.rotation,
                                          player->field)) {
                        player->pos.y++;

                        // Send our game state
                        NetworkMessage gameStateMessage;
                        gameStateMessage.header.type = MessageType::PLAYER_UPDATE;
                        gameStateMessage << *player;
                        server.sendMessageToAll(gameStateMessage);
                    } else {
                        // Place the piece
                        Tetromino::place(player->piece.current, player->pos.x, player->pos.y, player->pos.rotation,
                                         player->field);

                        // Check for lines
                        checkLines(player->field, lines, player->pos, player->speed);

                        // Generate new piece
                        resetPiece(player->piece, player->pos, player->speed, inputs, engine);

                        // Send our game state
                        NetworkMessage gameStateMessage;
                        gameStateMessage.header.type = MessageType::PLAYER_UPDATE;
                        gameStateMessage << *player;
                        server.sendMessageToAll(gameStateMessage);

                        if (!Tetromino::canFit(player->piece.current, player->pos.x, player->pos.y,
                                               player->pos.rotation,
                                               player->field)) {
                            // Set gameover flag, push to placements vector, send network message to update connected clients
                            player->gameOver = true;
                            placements.push_back(player->id);

                            NetworkMessage gameOverMessage;
                            gameOverMessage.header.type = MessageType::PLAYER_LOST;
                            gameOverMessage << player->id;
                            server.sendMessageToAll(gameOverMessage);
                            return;
                        }
                    }
                }
            }
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
            std::shared_ptr<Player> serverPlayer = players[1];

            renderField(renderer, texture, startX, startY, serverPlayer->field, lines, serverPlayer->gameOver,
                        blockSize);

            if (!serverPlayer->gameOver) {
                // Then render the server's current piece and other values
                // Render current piece in
                renderPiece(renderer, texture, serverPlayer->pos.x, serverPlayer->pos.y, serverPlayer->piece.current,
                            serverPlayer->pos.rotation,
                            startX, startY, 255, blockSize);

                // Render "ghost" piece (where it will land)
                int ghostY = serverPlayer->pos.y;
                while (Tetromino::canFit(serverPlayer->piece.current, serverPlayer->pos.x, ghostY + 1,
                                         serverPlayer->pos.rotation,
                                         serverPlayer->field)) {
                    ghostY++;
                }
                renderPiece(renderer, texture, serverPlayer->pos.x, ghostY, serverPlayer->piece.current,
                            serverPlayer->pos.rotation, startX,
                            startY, 50, blockSize);
                // Render the next piece under the score
//            SDL_Rect textRect = {startX + (FIELD_WIDTH * CELL_SIZE) + 10, startY + 200, 150, 75};
//            renderText(renderer, holder.font, "Next", {255, 255, 255, 255}, &textRect);
//            renderPiece(renderer, texture, 0, 0, serverPlayer->piece.next, 0, startX + (FIELD_WIDTH * CELL_SIZE) + 10,
//                        startY + 225, 100, blockSize);
//
//            // Render the stored piece directly under the next piece
//            if (serverPlayer->storedPiece != INVALID_SHAPE) {
//                textRect = {startX + (FIELD_WIDTH * CELL_SIZE) + 10, startY + 400, 150, 75};
//                renderText(renderer, fontHolder.font, "Stored", {255, 255, 255, 255}, &textRect);
//                renderPiece(renderer, holder, 0, 0, serverPlayer->storedPiece, 0, startX + (FIELD_WIDTH * CELL_SIZE) + 10,
//                            startY + 425, 100);
//            }
            } else {
                // Draw Game Over
                int place = getPlace(placements, serverPlayer->id, players.size());
                std::string placeStr = formatPlacement(place);

                SDL_Rect textRec = {startX + (FIELD_WIDTH * blockSize) / 2 - 100,
                                    startY + (FIELD_HEIGHT * blockSize) / 2 - 50, 200, 100};
                renderText(renderer, holder.font, place == 1 ? "Won" : "Lost", {255, 255, 255, 255}, &textRec);

                // Draw placement directly under
                textRec.y += textRec.h;
                textRec.w /= 2;
                textRec.h /= 2;
                textRec.x += textRec.w / 2;

                renderText(renderer, holder.font, placeStr, {255, 255, 255, 255}, &textRec);
            }

            startX = startX + (FIELD_WIDTH * blockSize) + 20;
            std::vector<unsigned int> oLines;

            // Render the other players in the server
            for (auto &it: players) {
                // ignore if it's the server (we rendered it above)
                if (it.first == 1) continue;
                std::shared_ptr<Player> player = it.second;

                // Render the field
                renderField(renderer, texture, startX, startY, player->field, oLines, player->gameOver, blockSize / 2);

                if (!player->gameOver)
                    // Render current piece in
                    renderPiece(renderer, texture, player->pos.x, player->pos.y, player->piece.current,
                                player->pos.rotation, startX, startY, 255, blockSize / 2);
                else {
                    // Draw Game Over
                    int place = getPlace(placements, player->id, players.size());
                    std::string placeStr = formatPlacement(place);

                    SDL_Rect textRec = {startX + (FIELD_WIDTH * (blockSize / 2)) / 2 - 50,
                                startY + (FIELD_HEIGHT * (blockSize / 2)) / 2 - 25, 100, 50};
                    renderText(renderer, holder.font, place == 1 ? "Won" : "Lost", {255, 255, 255, 255}, &textRec);

                    // Draw placement directly under
                    textRec.y += textRec.h;
                    textRec.w /= 2;
                    textRec.h /= 2;
                    textRec.x += textRec.w / 2;

                    renderText(renderer, holder.font, placeStr, {255, 255, 255, 255}, &textRec);
                }

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
        players[client->getId()]->ping = pingData;

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
        std::shared_ptr<Player> player = std::make_shared<Player>();
        resetPiece(player->piece, player->pos, player->speed, inputs, engine);
        initField(player->field);
        player->id = client->getId();
        players.insert_or_assign(player->id, player);

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

        // Send a connect message with all the other online members
        for (auto it = players.begin(); it != players.end(); it++) {
            if (it->first == client->getId()) continue;

            NetworkMessage playerMsg;
            playerMsg.header.type = MessageType::CONNECT;
            playerMsg << it->second->id;
            server.sendMessageTo(playerMsg, client);
        }

        server.sendMessageTo(assignId, client);
        server.sendMessageTo(pingMessage, client);
        server.sendMessageToAll(message, client->getId());
    }

    void HostGameMenuState::onDisconnect(std::shared_ptr<NetworkClient> &client) {
        std::cout << "[HGMS] Client disconnected: " << client->getId() << std::endl;

        // Remove from players map
        players.erase(client->getId());
        auto it = std::find(placements.begin(), placements.end(), client->getId());
        if(it != placements.end())
            placements.erase(it);

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

                Player player{};
                msg >> player;

                std::shared_ptr<Player> playerPtr = players[player.id];
                Player &playerRef = *playerPtr;
                memcpy(&playerRef, &player, sizeof(Player));

                break;
            }
            case MessageType::PLAYER_LOST: {
                server.sendMessageToAll(msg, client->getId());

                uint32_t id;
                msg >> id;
                players[id]->gameOver = true;
                placements.push_back(id);

                break;
            }
            default:
                std::cout << "[HGMS] Received unknown message: " << msg << std::endl;
        }
    }

} // Tetris