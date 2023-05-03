//
// Created by pauli on 4/27/2023.
//

#include "NetworkClient.h"
#include "NetworkServer.h"
#include <algorithm>
#include <utility>

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
        // Close ASIO context
        ioContext.stop();

        // Join thread
        if(serverThread.joinable()) serverThread.join();

        // Log server stop
        std::cout << "Stopping server..." << std::endl;
    }

    void NetworkServer::listenForClient() {
        acceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket) {
            if(!ec) {
                auto remote_endpoint = socket.remote_endpoint();

                // Create new client, add it to the list of existing clients, ensure the client is listening for messages
                std::shared_ptr<NetworkClient> client = std::make_shared<NetworkClient>(ioContext, std::move(socket), incoming);

                if(preConnectHandler(client)) {
                    clients.push_back(client);
                    client->connect(clientIdCounter++);

                    // Call the connect handler
                    connectHandler(client);
                    std::cout << "[Server] Client Added: " << remote_endpoint << std::endl;
                    std::cout << "[Server] Total Clients: " << clients.size() << std::endl;
                }
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

    void NetworkServer::update(bool block) {
        if(!block)
            incoming.wait();

        while(!incoming.empty()) {
            auto latest = incoming.pop();
            messageHandler(latest.client, latest.message);
        }
    }

    void NetworkServer::onMessageReceive(std::function<void(std::shared_ptr<NetworkClient>, NetworkMessage &)> handler) {
        messageHandler = std::move(handler);
    }

    void NetworkServer::sendMessageToAll(const NetworkMessage &message, uint32_t id) {
        bool invalid = false;
        for(auto & client : clients) {
            if(!client || !client->isSocketOpen()) {
                std::cout << "Disconnected client: " << client->getId() << std::endl;
                client.reset();
                invalid = true;
                continue;
            }

            if(client->getId() != id) {
                client->send(message);
            }
        }

        if(invalid) {
            clients.erase(std::remove(clients.begin(), clients.end(), nullptr), clients.end());
        }
    }

    void NetworkServer::sendMessageTo(const NetworkMessage &message, const std::shared_ptr<NetworkClient> &client) {
        if(client && client->isSocketOpen()) {
            client->send(message);
        } else {
            std::cout << "Disconnected client: " << client->getId() << std::endl;
            clients.erase(std::remove(clients.begin(), clients.end(), nullptr), clients.end());
        }
    }

    void NetworkServer::onClientPreConnect(std::function<bool(std::shared_ptr<NetworkClient>)> handler) {
        preConnectHandler = std::move(handler);
    }

    void NetworkServer::onClientConnect(std::function<void(std::shared_ptr<NetworkClient>)> handler) {
        connectHandler = std::move(handler);
    }
}