//
// Created by pauli on 4/27/2023.
//
#ifndef MPTETRIS_NETWORKSERVER_H
#define MPTETRIS_NETWORKSERVER_H

#include "asio.hpp"
#include "NetworkMessage.h"
#include "ConcurrentQueue.h"
#include <thread>

namespace Tetris {

    class NetworkServer {
    public:
        explicit NetworkServer(uint16_t port) : acceptor(ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {};
        ~NetworkServer();

        void sendMessageToAll(const NetworkMessage & message, uint32_t id = 0);
        void sendMessageTo(const NetworkMessage & message, uint32_t id);

        void start();
        void update(bool block = false);
        void onMessageReceive(const std::function<void(std::shared_ptr<NetworkClient>, NetworkMessage)> & handler);
    private:
        void stop();
        void listenForClient();

        ConcurrentQueue<OwnedNetworkMessage> incoming;

        asio::io_context ioContext;
        asio::ip::tcp::acceptor acceptor;

        std::thread serverThread;

        std::vector<std::shared_ptr<NetworkClient>> clients;

        // Function for handling messages
        std::function<void(std::shared_ptr<NetworkClient>, NetworkMessage)> messageHandler = [](const std::shared_ptr<NetworkClient>&, const NetworkMessage&) {};

        uint32_t clientIdCounter = 10000;
    };
}

#endif //MPTETRIS_NETWORKSERVER_H
