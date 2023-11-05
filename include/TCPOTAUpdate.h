#pragma once

#include <WiFi.h>
#include <WiFiServer.h>

#include <vector>

class TCPOTAUpdate {
    private:
        static const uint32_t IDLE_TIMEOUT_MS = 5000;

        struct ClientHandle {
            WiFiClient hnd;
            uint32_t lastMsgTime;

            ClientHandle(WiFiClient hnd, uint32_t currentTime) :
                hnd(hnd),
                lastMsgTime(currentTime) {}
        };

        WiFiServer tcpServer;
        std::vector<ClientHandle> clients;

        void acceptNewClients(uint32_t currentTime);

        bool updateClient(ClientHandle& client, uint32_t currentTime);

        void updateClients(uint32_t currentTime);

    public:
        TCPOTAUpdate(uint16_t port);

        void update();
        void update(uint32_t currentTime);
};
