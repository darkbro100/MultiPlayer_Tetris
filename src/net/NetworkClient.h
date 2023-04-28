//
// Created by pauli on 4/25/2023.
//

#ifndef MPTETRIS_NETWORKCLIENT_H
#define MPTETRIS_NETWORKCLIENT_H

#include "NetworkMessage.h"
#include "ConcurrentQueue.h"
#include "asio.hpp"

namespace Tetris {

    class NetworkClient {
    public:
        NetworkClient(asio::io_context& asioContext, asio::ip::tcp::socket _socket) : ioContext(asioContext), socket(std::move(_socket)) {};
        void connect(const asio::ip::tcp::resolver::results_type & endpoint);

        void beginRead();
        void finishRead();

        void send(const NetworkMessage & message);
    private:
        void beginMessageSend();
        void finishMessageSend();

        ConcurrentQueue<NetworkMessage> incoming{};
        ConcurrentQueue<NetworkMessage> outgoing{};

        asio::io_context & ioContext;
        asio::ip::tcp::socket socket;

        NetworkMessage modifyingMessage;

        std::thread clientThread;
    };

} // Tetris

#endif //MPTETRIS_NETWORKCLIENT_H
