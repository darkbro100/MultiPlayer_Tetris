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

        // Ensure this is updated in case the user is already holding return
        lastReturnPress = app->isKeyPressed(SDL_SCANCODE_RETURN);

        try {
            // Connect details
            const std::string &host = "127.0.0.1";
            const uint16_t port = 25000;

            // Resolve hostname/ip-address into tangiable physical address
            asio::ip::tcp::resolver resolver(io_context);
            asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

            // Connect to server
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

    void JoinGameMenuState::update(float ts) {
        MenuState::update(ts);

        // Check and determine which button is currently being selected
        bool isRetPressed = app->isKeyPressed(SDL_SCANCODE_RETURN);
        bool wasRetPressed = this->lastReturnPress;
        this->lastReturnPress = isRetPressed;

        bool isPressed = app->isKeyPressed(SDL_SCANCODE_DOWN) || app->isKeyPressed(SDL_SCANCODE_UP);
        bool wasPressed = this->lastDownPress;
        this->lastDownPress = isPressed;

        if (connectionStatus == NetworkClient::DISCONNECTED || !client->isSocketOpen()) {
            return;
        }

        if(gameStarted) {
            // Send update msg every frame
            NetworkMessage updateMsg;
            updateMsg.header.type = MessageType::PLAYER_UPDATE;
            updateMsg << players[client->getId()];
            client->send(updateMsg);
        }

        // cycle between menu buttons
//        if (app->isKeyPressed(SDL_SCANCODE_DOWN) && !wasPressed) {
//            selectedButton = (selectedButton + 1) % buttons.size();
//        } else if (app->isKeyPressed(SDL_SCANCODE_UP) && !wasPressed) {
//            selectedButton = (selectedButton - 1) % buttons.size();
//        }
    }

    void JoinGameMenuState::render(SDL_Renderer *renderer, float ts) {
        MenuState::render(renderer, ts);

        SDL_Rect dst = {0, 100, 150, 50};
        if (connectionStatus == NetworkClient::WAITING) {
            renderText(renderer, smallFont.font, "Status: Waiting", {100, 100, 100, 255}, &dst);
        } else if (connectionStatus == NetworkClient::CONNECTED && client->isSocketOpen()) {
            renderText(renderer, smallFont.font, "Status: Connected", {0, 255, 0, 255}, &dst);
        } else {
            renderText(renderer, smallFont.font, "Status: Failed", {255, 0, 0, 255},
                       &dst);
            return;// make sure nothing is being rendered if we're not connected
        }

        // Render all the players in
        if(!gameStarted)
            renderPlayers(renderer, players, font);
        else {
            /*
             * First, we need to render the server's actual game state. Afterwards, we can render all the other players that are in the lobby (at a much scaled down rate).
             * We also want to display the server's field much smaller relative to how we do it in singleplayer mode.
             */
            int startX = 50;
            int startY = 150;
            int blockSize = 25;
            std::vector<unsigned int> lines;
            renderField(renderer, texture, startX, startY, players[client->getId()].field, lines, players[client->getId()].gameOver, blockSize);

            startX = startX + (FIELD_WIDTH * blockSize) + 20;

            // Render the other players in the server
            for(auto & player : players) {
                if(player.first == client->getId()) continue;

                renderField(renderer, texture, startX, startY, player.second.field, lines, player.second.gameOver, blockSize / 2);
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

                Player player{};
                player.id = id;
                player.currentPiece = 5;
                player.nextPiece = engine() % 7;
                player.currentX = FIELD_WIDTH / 2;
                player.currentY = 0;
//                player.currentRotation = 0;
//                player.storedRotation = 0;
                player.currentSpeed = 0.7f;
//                player.currentSpeedTimer = 0.0f;
//                player.linesCreated = 0;
//                player.score = 0;
                initField(player.field);
                players[id] = player;

                client->setId(id);
                std::cout << "[JGMS] Assigned ID: " << id << std::endl;
                break;
            }
            case MessageType::CONNECT: {
                uint32_t id;
                msg >> id;

                Player player{};
                player.id = id;
                players[id] = player;

                std::cout << "[JGMS] Client connected to server with ID: " << id << std::endl;
                break;
            }

            case MessageType::DISCONNECT: {
                uint32_t id;
                msg >> id;

                players.erase(id);
                std::cout << "[JGMS] Client disconnected from server with ID: " << id << std::endl;
                break;
            }
            case MessageType::PING: {
                uint16_t pingData;
                uint32_t clientId;

                msg >> pingData >> clientId;
                players[clientId].ping = pingData;

                break;
            }
            case MessageType::GAME_START: {
                gameStarted = true;
                break;
            }
            case MessageType::PLAYER_UPDATE: {
                Player player{};
                msg >> player;
                players.insert_or_assign(player.id, player);
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

} // Tetris