#include "StreamOTAUpdate.h"

#include <Update.h>

StreamOTAUpdate::StreamOTAUpdate(Stream& stream, uint32_t imageSize, Print* optErrorOutputStream) :
    optErrorOutputStream(optErrorOutputStream) {

    if (!Update.begin(imageSize)) {
        printUpdateError();
        return;
    }

    uint32_t bytesWritten = Update.writeStream(stream);

    if (bytesWritten != imageSize) {
        Serial.printf("OTA update error, stream ended after %u bytes but %u expected\n", bytesWritten, imageSize);

        if (optErrorOutputStream) {
            optErrorOutputStream->printf("OTA update error, stream ended after %u bytes but %u expected\n", bytesWritten, imageSize);
        }

        Update.abort();
    }

    Update.end(false);
}

StreamOTAUpdate::~StreamOTAUpdate() {
    Update.clearError();
}

void StreamOTAUpdate::printUpdateError() {
    Update.printError(Serial);

    if (!optErrorOutputStream)
        return;

    Update.printError(*optErrorOutputStream);
}

bool StreamOTAUpdate::isSuccessful() const {
    return !Update.hasError();
}
