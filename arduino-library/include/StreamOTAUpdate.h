#pragma once

#include <Stream.h>
#include <Print.h>

class StreamOTAUpdate {
    private:
        Print* optErrorOutputStream;

        void printUpdateError();

    public:
        StreamOTAUpdate(Stream& stream, uint32_t imageSize, Print* optErrorOutputStream = nullptr);
        ~StreamOTAUpdate();

        bool isSuccessful() const;
};
