//
// Created by pauli on 4/27/2023.
//

#include "NetworkClient.h"
#include "NetworkServer.h"

namespace Tetris {

    void NetworkServer::start() {
        try {
            listenForClient();
            serverThread = std::thread([this]() { ioContext.run(); });
        } catch(std::exception & ex) {
            std::cerr << "Exception: " << ex.what() << std::endl;
        }
    }

    void NetworkServer::stop() {
        ioContext.stop();

        if(serverThread.joinable()) serverThread.join();

        std::cout << "Stopping server..." << std::endl;
    }

    void NetworkServer::listenForClient() {
        acceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket) {
            if(!ec) {
                std::string message = "Hello from server!";
                NetworkMessage toSend;
                toSend << message;

                std::cout << "New client connected: " << socket.remote_endpoint() << " ... Sending test message" << std::endl;

                std::shared_ptr<NetworkClient> client = std::make_shared<NetworkClient>(ioContext, std::move(socket));
                clients.push_back(client);

                client->send(toSend);
            } else {
                std::cout << "Failed to connect to client: " << ec.message() << std::endl;
            }

            // Once again listen for a new client, since we are technically inside the lambda this isn't necessarily recursion but it may seem like it at first glance
            listenForClient();
        });
    }

    NetworkServer::~NetworkServer() {
        stop();
    }

    void NetworkServer::update() {

    }
}