//
// Created by pauli on 4/25/2023.
//

#ifndef MPTETRIS_NETWORKMESSAGE_H
#define MPTETRIS_NETWORKMESSAGE_H

#include <iostream>
#include <vector>

namespace Tetris {
    enum class MessageType {
        CONNECT,
        DISCONNECT
    };

    /**
    * Represents a message that will be sent between the client and the server.
    * Backed by a std::vector that contains the amount of the bytes for the message.
    */
    struct NetworkMessage {
        MessageType type;
        uint32_t messageSize = 0;
        std::vector<uint8_t> data;

        friend std::ostream &operator<<(std::ostream &os, const NetworkMessage &message);

        template <typename POD>
        friend NetworkMessage & operator<<(NetworkMessage &message, POD & pod);

        template <typename POD>
        friend NetworkMessage & operator >>(NetworkMessage &message, POD & pod);

    };
}

#endif //MPTETRIS_NETWORKMESSAGE_H
