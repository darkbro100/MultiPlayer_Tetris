//
// Created by pauli on 4/27/2023.
//

#include "NetworkMessage.h"

namespace Tetris {
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
    NetworkMessage &operator<<(NetworkMessage &message, POD &pod) {
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

    std::ostream &operator<<(std::ostream &os, const NetworkMessage &message) {
        os << "type: " << static_cast<int>(message.type) << " messageSize: " << message.messageSize;
        return os;
    }
}