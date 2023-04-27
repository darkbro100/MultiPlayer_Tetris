//
// Created by pauli on 4/25/2023.
//

#include "NetworkClient.h"

namespace Tetris {
    void NetworkClient::connect(const std::string &host, const std::string & port) {
        asio::ip::tcp::resolver resolver(ioContext);
        asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, port);

        asio::async_connect(socket, endpoints, [this](std::error_code ec, const asio::ip::tcp::endpoint& endpoint) {
            if(!ec) {
                std::cout << "Connected to " << endpoint << std::endl;
            } else {
                std::cout << "Failed to connect to " << endpoint << std::endl;
            }
        });
    }
} // Tetris