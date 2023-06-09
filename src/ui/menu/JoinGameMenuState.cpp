//
// Created by pauli on 4/30/2023.
//

#include <chrono>
#include "../../TetrisApp.h"
#include "JoinGameMenuState.h"

namespace Tetris {

    JoinGameMenuState::JoinGameMenuState(App *app) : MenuState(app), connectionStatus(NetworkClient::WAITING), ping(0) {

    }

    void JoinGameMenuState::loadComponents() {
        MenuState::loadComponents();

        texture = app->getTexture("tetronimo");
        font = app->getFont("opensans_large");
        smallFont = app->getFont("opensans");

        std::random_device dev;
        engine.seed(dev());

        SDL_StartTextInput();
    }

    void JoinGameMenuState::update(float ts) {
        MenuState::update(ts);

        // Check and determine which button is currently being selected
        bool isRetPressed = app->isKeyPressed(SDL_SCANCODE_RETURN);
        bool wasRetPressed = this->lastReturnPress;
        this->lastReturnPress = isRetPressed;

        // if the client ends up disconnecting, go back to the main menu
        if((!client || !client->isSocketOpen()) && connectionStatus != NetworkClient::WAITING) {
            std::shared_ptr<MainMenuState> mainMenu = std::make_shared<MainMenuState>(app);
            mainMenu->loadComponents();
            app->changeState(mainMenu);
            return;
        }

        // once the client has input the host ip and pressed return, we can create a new connection
        if(connectionStatus == NetworkClient::WAITING && isRetPressed && !wasRetPressed) {
            SDL_StopTextInput();
            attemptConnect();
            return;
        }

        // normal game logic, once it has started
        if (gameStarted) {
            auto player = players[client->getId()];

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
                    }

                    // Send update message
                    NetworkMessage gameStateMessage;
                    gameStateMessage.header.type = MessageType::PLAYER_UPDATE;
                    gameStateMessage << *player;
                    client->send(gameStateMessage);

                    return;
                }

                // Input handling
                int16_t x = player->pos.x;
                int16_t y = player->pos.y;
                checkInputs(inputs, app, player->piece, player->pos, player->speed, player->field, engine);

                // only send an update packet if the player has moved
                if(x != player->pos.x || y != player->pos.y) {
                    NetworkMessage gameStateMessage;
                    gameStateMessage.header.type = MessageType::PLAYER_UPDATE;
                    gameStateMessage << *player;
                    client->send(gameStateMessage);
                }

                player->speed.timer += ts;
                if (player->speed.timer >= player->speed.current) {
                    player->speed.timer = 0.0f;

                    // Check if can move down
                    if (Tetromino::canFit(player->piece.current, player->pos.x, player->pos.y + 1, player->pos.rotation,
                                          player->field)) {
                        player->pos.y++;

                        //send update packet
                        NetworkMessage gameStateMessage;
                        gameStateMessage.header.type = MessageType::PLAYER_UPDATE;
                        gameStateMessage << *player;
                        client->send(gameStateMessage);
                    } else {
                        // Place the piece
                        Tetromino::place(player->piece.current, player->pos.x, player->pos.y, player->pos.rotation,
                                         player->field);

                        // Check for lines
                        checkLines(player->field, lines, player->pos, player->speed);

                        // Generate new piece
                        resetPiece(player->piece, player->pos, player->speed, inputs, engine);

                        // Send update packet because the field has been updated as well as some other vars
                        NetworkMessage gameStateMessage;
                        gameStateMessage.header.type = MessageType::PLAYER_UPDATE;
                        gameStateMessage << *player;
                        client->send(gameStateMessage);

                        if (!Tetromino::canFit(player->piece.current, player->pos.x, player->pos.y,
                                               player->pos.rotation,
                                               player->field)) {

                            player->gameOver = true;
                            placements.push_back(player->id);

                            // Send a network message letting everyone know they have lost
                            NetworkMessage gameOverMessage;
                            gameOverMessage.header.type = MessageType::PLAYER_LOST;
                            gameOverMessage << player->id;
                            client->send(gameOverMessage);
                            return;
                        }
                    }
                }
            }
        }
    }

    void JoinGameMenuState::render(SDL_Renderer *renderer, float ts) {
        MenuState::render(renderer, ts);

        SDL_Rect dst = {0, 100, 150, 50};

        if (connectionStatus == NetworkClient::WAITING) {

            int width = 300, height = 100;
            SDL_Rect center = {(App::WINDOW_WIDTH / 2) - (width / 2), (App::WINDOW_HEIGHT / 2) - (height / 2), width, height};

            renderText(renderer, font.font, "Enter Host IP:", {255, 255, 255, 255}, &center);
            center.y += center.h;

            size_t length = app->getInputText().length();

            // Scale the width/height of the text box based on the length of the text
            center.w = length * 20;
            center.h = 50;

            // Shift so it remains center with the text above
            center.x += (width - center.w) / 2;

            renderText(renderer, smallFont.font, app->getInputText(), {100,100,100,255}, &center);
            return; // make sure nothing is being rendered if we haven't created a client yet
        } else if (connectionStatus == NetworkClient::CONNECTED && client->isSocketOpen()) {
            renderText(renderer, smallFont.font, "Status: Connected", {0, 255, 0, 255}, &dst);
        } else if(connectionStatus == NetworkClient::CONNECTING){
            renderText(renderer, smallFont.font, "Status: Waiting", {100, 100, 100, 255}, &dst);
            return;
        } else {
            renderText(renderer, smallFont.font, "Status: Failed", {255, 0, 0, 255},
                       &dst);
            return;// make sure nothing is being rendered if we're not connected
        }

        // Render all the players in
        if (!gameStarted)
            renderPlayers(renderer, players, font, client->getId());
        else {
            /*
             * First, we need to render the server's actual game state. Afterwards, we can render all the other players that are in the lobby (at a much scaled down rate).
             * We also want to display the server's field much smaller relative to how we do it in singleplayer mode.
             */
            int startX = 50;
            int startY = 150;
            int blockSize = 25;
            auto player = players[client->getId()];
            renderField(renderer, texture, startX, startY, player->field, lines, player->gameOver, blockSize);

            if(!player->gameOver) {
                // Then render the client's piece
                renderPiece(renderer, texture, player->pos.x, player->pos.y, player->piece.current, player->pos.rotation,
                            startX, startY, 255, blockSize);

                // Render "ghost" piece (where it will land)
                int ghostY = player->pos.y;
                while (Tetromino::canFit(player->piece.current, player->pos.x, ghostY + 1,
                                         player->pos.rotation,
                                         player->field)) {
                    ghostY++;
                }
                renderPiece(renderer, texture, player->pos.x, ghostY, player->piece.current,
                            player->pos.rotation, startX,
                            startY, 50, blockSize);
            } else {
                // Draw Game Over
                int place = getPlace(placements, client->getId(), players.size());
                std::string placeStr = formatPlacement(place);

                SDL_Rect textRec = {startX + (FIELD_WIDTH * blockSize) / 2 - 100,
                                    startY + (FIELD_HEIGHT * blockSize) / 2 - 50, 200, 100};
                renderText(renderer, font.font, place == 1 ? "Won" : "Lost", {255, 255, 255, 255}, &textRec);

                // Draw placement directly under
                textRec.y += textRec.h;
                textRec.w /= 2;
                textRec.h /= 2;
                textRec.x += textRec.w / 2;

                renderText(renderer, font.font, placeStr, {255, 255, 255, 255}, &textRec);
            }

            startX = startX + (FIELD_WIDTH * blockSize) + 20;

            // Render the other players in the server
            std::vector<unsigned int> oLines;
            for (auto &it: players) {
                if (it.first == client->getId()) continue;

                // Render other player fields and pieces
                renderField(renderer, texture, startX, startY, it.second->field, oLines, it.second->gameOver,
                            blockSize / 2);

                if(!it.second->gameOver) {
                    renderPiece(renderer, texture, it.second->pos.x, it.second->pos.y, it.second->piece.current,
                                it.second->pos.rotation, startX, startY, 255, blockSize / 2);
                } else {
                    // Draw Game Over
                    int place = getPlace(placements, it.second->id, players.size());
                    std::string placeStr = formatPlacement(place);

                    SDL_Rect textRec = {startX + (FIELD_WIDTH * (blockSize / 2)) / 2 - 50,
                                        startY + (FIELD_HEIGHT * (blockSize / 2)) / 2 - 25, 100, 50};
                    renderText(renderer, font.font, place == 1 ? "Won" : "Lost", {255, 255, 255, 255}, &textRec);

                    // Draw placement directly under
                    textRec.y += textRec.h;
                    textRec.w /= 2;
                    textRec.h /= 2;
                    textRec.x += textRec.w / 2;

                    renderText(renderer, font.font, placeStr, {255, 255, 255, 255}, &textRec);
                }

                startX += (FIELD_WIDTH * blockSize / 2) + 20;
            }
        }
    }

    JoinGameMenuState::~JoinGameMenuState() {
    }


    // Bounce the message back to the server, so the server can calculate the ping
    void JoinGameMenuState::onPingReceive(NetworkMessage &message) {
        client->send(message);
    }

    void JoinGameMenuState::onMessage(NetworkMessage &msg) {
        switch (msg.header.type) {
            case MessageType::KEEP_ALIVE: {
                onPingReceive(msg);
                break;
            }
            case MessageType::ASSIGN_ID: {
                uint32_t id;
                msg >> id;

                client->setId(id);
                std::shared_ptr<Player> player = std::make_shared<Player>();
                initField(player->field);
                resetPiece(player->piece, player->pos, player->speed, inputs, engine);
                player->id = id;
                player->speed.timer = 0.0f;
                player->speed.current = 0.7f;

                players.insert_or_assign(id, player);

                std::cout << "[JGMS] Assigned ID: " << id << std::endl;
                break;
            }
            case MessageType::CONNECT: {
                uint32_t id;
                msg >> id;

                std::shared_ptr<Player> player = std::make_shared<Player>();
                initField(player->field);
                resetPiece(player->piece, player->pos, player->speed, inputs, engine);
                player->id = id;

                players.insert_or_assign(id, player);

                std::cout << "[JGMS] Client connected to server with ID: " << id << std::endl;
                break;
            }

            case MessageType::DISCONNECT: {
                uint32_t id;
                msg >> id;

                // erase from containers
                auto it = std::find(placements.begin(), placements.end(), id);
                if(it != placements.end())
                    placements.erase(it);
                players.erase(id);

                std::cout << "[JGMS] Client disconnected from server with ID: " << id << std::endl;
                break;
            }
            case MessageType::PING: {
                uint16_t pingData;
                uint32_t clientId;

                msg >> pingData >> clientId;
                if(players[clientId])
                    players[clientId]->ping = pingData;

                break;
            }
            case MessageType::GAME_START: {
                gameStarted = true;
                placements.clear();

                break;
            }
            case MessageType::PLAYER_UPDATE: {
                Player player{};
                msg >> player;

                // grab shared ptr, convert it to a reference, then call memcpy
                std::shared_ptr<Player> playerPtr = players[player.id];
                Player & pl = *playerPtr;

                memcpy(&pl, &player, sizeof(Player));

                break;
            }
            case MessageType::PLAYER_LOST: {
                uint32_t id;
                msg >> id;
                players[id]->gameOver = true;
                placements.push_back(id);

                break;
            }
            case MessageType::PLAYER_WON: {
                uint32_t winner;
                msg >> winner;

                players[winner]->gameOver = true;
                placements.push_back(winner);

                break;
            }
            case MessageType::GAME_END: {
                gameStarted = false;

                std::shared_ptr<Player> pl = players[client->getId()];
                resetPiece(pl->piece, pl->pos, pl->speed, inputs, engine);
                initField(pl->field);
                pl->gameOver = false;

                break;
            }
            default:
                std::cout << "[JGMS] Received unknown message: " << msg << std::endl;
        }
    }

    void JoinGameMenuState::onConnect(std::shared_ptr<NetworkClient> &who) {

    }

    void JoinGameMenuState::onDisconnect(std::shared_ptr<NetworkClient> &who) {

    }

    void JoinGameMenuState::attemptConnect() {
        try {
            // Connect details
            const std::string &host = app->getInputText();
            const uint16_t port = 25000;

            std::cout << "Host: " << host << " Port: " << port << std::endl;

            // Resolve hostname/ip-address into tangiable physical address
            asio::ip::tcp::resolver resolver(io_context);
            asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

            // Connect to server
            connectionStatus = NetworkClient::CONNECTING;
            client = std::make_shared<NetworkClient>(io_context, asio::ip::tcp::socket(io_context), incomingMessages);
            client->onConnect([this](bool connected) {
                connectionStatus = connected ? NetworkClient::CONNECTED : NetworkClient::DISCONNECTED;
            });
            client->onMessageReceive([this](NetworkMessage &message) { onMessage(message); });
            client->connect(endpoints);
        } catch (std::exception &what) {
            std::cerr << "[JoinGameMenuState] Failed to connect to server: " << what.what() << std::endl;
            connectionStatus = NetworkClient::DISCONNECTED;
        }
    }

} // Tetris