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

    struct NetworkMessageString {
        uint32_t size;
        const char * data;
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

        // String specific overloads
        friend NetworkMessage& operator<<(NetworkMessage& message, const std::string& str);
        friend NetworkMessage& operator>>(NetworkMessage& message, std::string& str);

        // Normal POD overloads
        template <typename POD>
        friend NetworkMessage & operator<<(NetworkMessage &message, const POD & pod);

        template <typename POD>
        friend NetworkMessage & operator >>(NetworkMessage &message, POD & pod);

        // Helper methods to append/read from the data vector for const char pointer.
        void append(const char *string, uint32_t i);
        void read(const char *string, uint32_t i);
    };

    inline NetworkMessage& operator<<(NetworkMessage& message, const std::string& str) {
        auto len = static_cast<uint32_t>(str.length());
        message.append(str.data(), len);
        message << len;
        return message;
    }

    inline NetworkMessage& operator>>(NetworkMessage& message, std::string& str) {
        uint32_t len = 0;
        message >> len;
        str.resize(len);
        message.read(str.data(), len);
        return message;
    }

    template<typename POD>
    NetworkMessage &operator>>(NetworkMessage &message, POD &pod) {
        // std::is_standard_layout will determine if the data type of the POD template is a Plain Old Data Type (POD)
        static_assert(std::is_standard_layout<POD>::value, "Please use a POD. No complex types allowed :>");

        // Copy the data from the vector to the POD
        memcpy(&pod, message.data.data() + message.data.size() - sizeof(POD), sizeof(POD));

        // Decrease the message size by the size of the POD
        message.messageSize -= sizeof(POD);

        // Resize the vector to fit our new data
        message.data.resize(message.data.size() - sizeof(POD));

        return message;
    }

    template<typename POD>
    NetworkMessage &operator<<(NetworkMessage &message, const POD &pod) {
        // std::is_standard_layout will determine if the data type of the POD template is a Plain Old Data Type (POD)
        static_assert(std::is_standard_layout<POD>::value, "Please use a POD. No complex types allowed :>");

        // Resize the vector to fit our new data
        message.data.resize(message.data.size() + sizeof(POD));

        // Copy the data from the POD to the vector
        memcpy(message.data.data() + message.data.size() - sizeof(POD), &pod, sizeof(POD));

        // Increase the message size by the size of the POD
        message.messageSize += sizeof(POD);

        return message;
    }

    inline std::ostream &operator<<(std::ostream &os, const NetworkMessage &message) {
        os << "type: " << static_cast<int>(message.type) << " messageSize: " << message.messageSize;
        return os;
    }

    inline void NetworkMessage::append(const char *string, uint32_t i) {
        data.resize(data.size() + i);
        memcpy(data.data() + data.size() - i, string, i);
        messageSize += i;
    }

    inline void NetworkMessage::read(const char *string, uint32_t i) {
        memcpy((void*)string, data.data() + data.size() - i, i);
        messageSize -= i;
        data.resize(data.size() - i);
    }
}

#endif //MPTETRIS_NETWORKMESSAGE_H
