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

        font = app->getFont("opensans_large");
        smallFont = app->getFont("opensans");

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
            client->onMessageReceive([this](NetworkMessage &message) {
                switch (message.header.type) {
                    case MessageType::PING: {
                        onPingReceive(message);
                        break;
                    }
                    case MessageType::ASSIGN_ID: {
                        uint32_t id;
                        message >> id;
                        client->setId(id);
                        std::cout << "[JGMS] Assigned ID: " << id << std::endl;

                        break;
                    }
                    case MessageType::CONNECT: {
                        uint32_t id;
                        message >> id;
                        std::cout << "[JGMS] Client connected to server with ID: " << id << std::endl;
                        break;
                    }

                    case MessageType::DISCONNECT: {
                        uint32_t id;
                        message >> id;
                        std::cout << "[JGMS] Client disconnected from server with ID: " << id << std::endl;
                        break;
                    }
                    default:
                        std::cout << "[JGMS] Received unknown message: " << message << std::endl;
                }
            });
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

        // cycle between menu buttons
//        if (app->isKeyPressed(SDL_SCANCODE_DOWN) && !wasPressed) {
//            selectedButton = (selectedButton + 1) % buttons.size();
//        } else if (app->isKeyPressed(SDL_SCANCODE_UP) && !wasPressed) {
//            selectedButton = (selectedButton - 1) % buttons.size();
//        }
    }

    void JoinGameMenuState::render(SDL_Renderer *renderer, float ts) {
        MenuState::render(renderer, ts);

        SDL_Rect dst = {0, 100, 300, 75};
        switch (connectionStatus) {
            case NetworkClient::WAITING: {
                renderText(renderer, smallFont.font, "Status: Waiting", {100, 100, 100, 255}, &dst);
                break;
            }

            case NetworkClient::CONNECTED: {
                renderText(renderer, smallFont.font, "Status: Connected", {0, 255, 0, 255}, &dst);
                dst = {0, dst.y + dst.h, 300, 75};
                renderText(renderer, smallFont.font, "Ping: " + std::to_string(ping) + "ms", {0, 255, 0, 255}, &dst);
                break;
            }

            case NetworkClient::DISCONNECTED: {
                renderText(renderer, smallFont.font, "Status: Failed", {255, 0, 0, 255}, &dst);
                break;
            }
        }
    }

    JoinGameMenuState::~JoinGameMenuState() {
    }

    void JoinGameMenuState::onPingReceive(NetworkMessage &message) {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        std::chrono::high_resolution_clock::time_point then;
        message >> then;

        // update ping var. TODO: display ping on screen and calculate an actual avg for the previous pings
        uint16_t pingData = std::chrono::duration_cast<std::chrono::milliseconds>(now - then).count();
        ping = pingData;

        NetworkMessage toSend;
        toSend.header.type = MessageType::PING;
        toSend << now;

        client->send(toSend);
    }

} // Tetris