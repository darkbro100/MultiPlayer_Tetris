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

        // Ensure this is updated in case the user is already holding return
        lastReturnPress = app->isKeyPressed(SDL_SCANCODE_RETURN);

        // Start the server
        server.onClientDisconnect([this](std::shared_ptr<NetworkClient> client) { onDisconnect(client); });
        server.onClientPreConnect([this](std::shared_ptr<NetworkClient> client) { return onPreConnect(client); });
        server.onClientConnect([this](std::shared_ptr<NetworkClient> client) { onConnect(client); });
        server.onMessageReceive([this](std::shared_ptr<NetworkClient> client, NetworkMessage &message) {
            switch (message.header.type) {
                case MessageType::KEEP_ALIVE: {
                    onPingReceive(client, message);
                    break;
                }
                default:
                    std::cout << "[HGMS] Received unknown message: " << message << std::endl;
            }
        });
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

        // cycle between menu buttons
//        if (app->isKeyPressed(SDL_SCANCODE_DOWN) && !wasPressed) {
//            selectedButton = (selectedButton + 1) % buttons.size();
//        } else if (app->isKeyPressed(SDL_SCANCODE_UP) && !wasPressed) {
//            selectedButton = (selectedButton - 1) % buttons.size();
//        }
    }

    void HostGameMenuState::render(SDL_Renderer *renderer, float ts) {
        MenuState::render(renderer, ts);
    }

    void HostGameMenuState::onPingReceive(std::shared_ptr<NetworkClient> &client, NetworkMessage &message) {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        std::chrono::high_resolution_clock::time_point then;
        message >> then;

        // calc ping
        uint16_t pingData = std::chrono::duration_cast<std::chrono::milliseconds>(now - then).count();

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

        server.sendMessageToAll(message);
        server.sendMessageTo(assignId, client);
        server.sendMessageTo(pingMessage, client);
    }

    void HostGameMenuState::onDisconnect(std::shared_ptr<NetworkClient> &client) {
        std::cout << "[HGMS] Client disconnected: " << client->getId() << std::endl;

        NetworkMessage message;
        message.header.type = MessageType::DISCONNECT;
        message << client->getId();

        server.sendMessageToAll(message, client->getId());
    }

} // Tetris