//
// Created by pauli on 4/30/2023.
//

#include "../../TetrisApp.h"
#include "JoinGameMenuState.h"

namespace Tetris {

    JoinGameMenuState::JoinGameMenuState(App *app) : MenuState(app) {

    }

    void JoinGameMenuState::loadComponents() {
        MenuState::loadComponents();

        // Ensure this is updated in case the user is already holding return
        lastReturnPress = app->isKeyPressed(SDL_SCANCODE_RETURN);

        // Connect details
        const std::string &host = "127.0.0.1";
        const uint16_t port = 25000;

        // Resolve hostname/ip-address into tangiable physical address
        asio::ip::tcp::resolver resolver(io_context);
        asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

        // Connect to server
        client = std::make_shared<NetworkClient>(io_context, asio::ip::tcp::socket(io_context), incomingMessages);
        client->connect(endpoints);
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
    }

    JoinGameMenuState::~JoinGameMenuState() {
        std::cout << "JoinGameMenuState destroyed\n";
    }

} // Tetris