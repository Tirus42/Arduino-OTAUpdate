#include "TCPOTAUpdate.h"

#include "StreamOTAUpdate.h"

TCPOTAUpdate::TCPOTAUpdate(uint16_t port) :
    tcpServer(port),
    clients() {

    tcpServer.begin();
}

void TCPOTAUpdate::update() {
    update(millis());
}

void TCPOTAUpdate::update(uint32_t currentTime) {
    acceptNewClients(currentTime);
    updateClients(currentTime);
}

void TCPOTAUpdate::acceptNewClients(uint32_t currentTime) {
    while(WiFiClient newClient = tcpServer.available()) {
        Serial.println("New OTA update client\n");
        clients.push_back(ClientHandle(newClient, currentTime));
    }
}

bool TCPOTAUpdate::updateClient(ClientHandle& client, uint32_t currentTime) {
    if (!client.hnd.connected()) {
        Serial.println("OTA client disconnect");
        return false;
    }

    int bytesAvailable = client.hnd.available();

    if (bytesAvailable == 0) {
        if (client.lastMsgTime + IDLE_TIMEOUT_MS < currentTime) {
            Serial.println("OTA client disconnect due to transfer timeout");
            return false;
        }

        return true;
    } else {
        client.lastMsgTime = currentTime;
    }

    if (bytesAvailable < 4)
        return true;

    uint8_t headerBytes[4];
    client.hnd.read(headerBytes, sizeof(headerBytes));

    uint32_t updateSize;
    memcpy(&updateSize, headerBytes, sizeof(updateSize));

    Serial.printf("Received OTA header, update has %u bytes\n", updateSize);

    client.hnd.setTimeout(10000);	// Set large timeout to avoid read() problems over weak WiFi

    StreamOTAUpdate updater(client.hnd, updateSize, &client.hnd);

    client.hnd.printf("Update successful: %s\n", updater.isSuccessful() ? "true" : "false");
    client.hnd.flush();

    if (!updater.isSuccessful()) {
        return false;
    }

    ESP.restart();
    return false;
}

void TCPOTAUpdate::updateClients(uint32_t currentTime) {
    for (size_t i = 0; i < clients.size(); ++i) {
        ClientHandle& client = clients[i];

        if (!updateClient(client, currentTime)) {
            clients.erase(clients.begin() + i);
            break;
        }
    }
}
