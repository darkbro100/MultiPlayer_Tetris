//
// Created by pauli on 4/25/2023.
//

#include "NetworkClient.h"

namespace Tetris {
    void NetworkClient::connect(
            const asio::ip::basic_resolver<asio::ip::tcp, asio::any_io_executor>::results_type &endpoint) {

        // Prime ASIO with some work to do, connect to the socket
        asio::async_connect(socket, endpoint, [this](std::error_code ec, const asio::ip::tcp::endpoint &endpoints) {
            if (!ec) {
                std::cout << "Connected to " << endpoints << std::endl;
                beginRead();
            } else {
                std::cout << "Failed to connect to " << endpoints << std::endl;
                if (clientThread.joinable()) clientThread.join();
            }
        });

        // Launch the thread to run the asio context
        clientThread = std::thread([this]() { ioContext.run(); });
        clientType = ClientType::CLIENT;
    }

    void NetworkClient::send(const NetworkMessage &message) {
        asio::post(ioContext, [this, message]() {
            bool isWriting = !outgoing.empty();
            outgoing.push(message);

            if (!isWriting) {
                beginMessageSend();
            }
        });
    }

    void NetworkClient::beginMessageSend() {
        asio::async_write(socket, asio::buffer(&outgoing.front().header, sizeof(outgoing.front().header)),
                          [this](std::error_code ec, std::size_t length) {
                              if (!ec) {
                                  std::cout << "Wrote " << length << " bytes" << std::endl;
                                  finishMessageSend();
                              } else {
                                  std::cout << "Failed to write " << length << " bytes" << std::endl;
                              }
                          });
    }

    void NetworkClient::finishMessageSend() {
        asio::async_write(socket, asio::buffer(outgoing.front().data.data(), outgoing.front().header.messageSize),
                          [this](std::error_code ec, std::size_t length) {
                              if (!ec) {
                                  std::cout << "Wrote " << length << " bytes" << std::endl;
                                  outgoing.pop();

                                  if (!outgoing.empty()) {
                                      beginMessageSend();
                                  }
                              } else {
                                  std::cout << "Failed to write " << length << " bytes" << std::endl;
                              }
                          });
    }

    void NetworkClient::beginRead() {
        asio::async_read(socket, asio::buffer(&modifyingMessage.header, sizeof(modifyingMessage.header)),
                         [this](std::error_code ec, std::size_t length) {
                             if (!ec) {
                                 std::cout << "Read " << length << " bytes" << std::endl;
                                 modifyingMessage.data.resize(modifyingMessage.header.messageSize);

                                 if(modifyingMessage.header.messageSize <= 0) {
                                     addToIncomingQueue();
                                     beginRead();
                                 } else {
                                     finishRead();
                                 }
                             } else {
                                 std::cout << "Failed to read " << length << " bytes" << std::endl;
                             }
                         });
    }

    void NetworkClient::finishRead() {
        asio::async_read(socket, asio::buffer(modifyingMessage.data.data(), modifyingMessage.header.messageSize),
                         [this](std::error_code ec, std::size_t length) {
                             if (!ec) {
                                 std::cout << "Read " << length << " bytes" << std::endl;
                                 addToIncomingQueue();
                                 beginRead();
                             } else {
                                 std::cout << "Failed to read " << length << " bytes" << std::endl;
                             }
                         });
    }

    void NetworkClient::addToIncomingQueue() {
        if(clientType == ClientType::SERVER) {
            OwnedNetworkMessage ownedMessage = {shared_from_this(), modifyingMessage};
            incoming.push(ownedMessage);

            messageHandler(modifyingMessage);
        } else {
            OwnedNetworkMessage ownedMessage = {nullptr, modifyingMessage};
            incoming.push(ownedMessage);

            messageHandler(modifyingMessage);
        }
    }

    void NetworkClient::onMessageReceive(const std::function<void(NetworkMessage)> & handler) {
        messageHandler = handler;
    }

    void NetworkClient::connect(uint32_t id) {
        if(socket.is_open()) {
            clientType = ClientType::SERVER;
            clientId = id;
            beginRead();
        }
    }

    uint32_t NetworkClient::getId() const {
        return clientId;
    }

    bool NetworkClient::isSocketOpen() {
        return socket.is_open();
    }

} // Tetris