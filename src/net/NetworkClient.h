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

        void connect(const std::string &host, const std::string & port);

    private:
        ConcurrentQueue<NetworkMessage> incoming;
        ConcurrentQueue<NetworkMessage> outgoing;

        asio::io_context ioContext;
        asio::ip::tcp::socket socket;
    };

} // Tetris

#endif //MPTETRIS_NETWORKCLIENT_H
