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

        void start();
        void update();
    private:
        void stop();
        void listenForClient();

        ConcurrentQueue<NetworkMessage> incoming;

        asio::io_context ioContext;
        asio::ip::tcp::acceptor acceptor;

        std::thread serverThread;

        std::vector<std::shared_ptr<NetworkClient>> clients;
    };
}

#endif //MPTETRIS_NETWORKSERVER_H
