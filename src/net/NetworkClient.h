//
// Created by pauli on 4/25/2023.
//

#ifndef MPTETRIS_NETWORKCLIENT_H
#define MPTETRIS_NETWORKCLIENT_H

#include "NetworkMessage.h"
#include "ConcurrentQueue.h"
#include "asio.hpp"

namespace Tetris {

    enum class ClientType {
        CLIENT,
        SERVER
    };

    class NetworkClient : public std::enable_shared_from_this<NetworkClient> {
    public:
        NetworkClient(asio::io_context& asioContext, asio::ip::tcp::socket _socket, ConcurrentQueue<OwnedNetworkMessage> & _incoming) : ioContext(asioContext), socket(std::move(_socket)), incoming(_incoming) {};
        void connect(const asio::ip::tcp::resolver::results_type & endpoint);
        void connect(uint32_t id);

        void send(const NetworkMessage & message);
        void onMessageReceive(const std::function<void(NetworkMessage)> & handler);
        uint32_t getId() const;
        bool isSocketOpen();
    private:
        void beginRead();
        void finishRead();

        void addToIncomingQueue();
        void beginMessageSend();
        void finishMessageSend();

        ConcurrentQueue<OwnedNetworkMessage> & incoming;
        ConcurrentQueue<NetworkMessage> outgoing{};

        asio::io_context & ioContext;
        asio::ip::tcp::socket socket;

        NetworkMessage modifyingMessage;

        std::thread clientThread;

        // Function for handling messages
        std::function<void(NetworkMessage)> messageHandler = [](const NetworkMessage&) {};

        uint32_t clientId = 0;
        ClientType clientType;
    };

} // Tetris

#endif //MPTETRIS_NETWORKCLIENT_H
