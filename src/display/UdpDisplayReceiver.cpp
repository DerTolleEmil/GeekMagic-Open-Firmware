// SPDX-License-Identifier: GPL-3.0-or-later
#include "display/UdpDisplayReceiver.h"

#include <Arduino.h>
#include <Logger.h>
#include "config/ConfigManager.h"
#include "display/DisplayManager.h"

auto UdpDisplayReceiver::begin() -> bool {
    listening = socket.begin(PORT) != 0;
    if (!listening) {
        Logger::error("Failed to bind UDP display port", "UdpDisplayReceiver");
    }
    return listening;
}

void UdpDisplayReceiver::update() {
    if (!listening) {
        return;
    }

    for (size_t packet = 0; packet < PACKETS_PER_UPDATE; ++packet) {
        const int packetBytes = socket.parsePacket();
        if (packetBytes == 0) {
            break;
        }
        if (packetBytes < static_cast<int>(HEADER_BYTES + sizeof(uint16_t)) ||
            packetBytes > static_cast<int>(MAX_PACKET_BYTES)) {
            // The next parsePacket() discards any unread bytes of this datagram.
            continue;
        }

        std::array<uint8_t, HEADER_BYTES> header{};
        if (socket.read(header.data(), header.size()) != static_cast<int>(header.size())) {
            continue;
        }

        const int16_t xPos = header[0];
        const int16_t yPos = header[1];
        const int16_t width = header[2];
        const int16_t height = header[3];
        const size_t pixelCount = static_cast<size_t>(width) * height;
        if (width == 0 || height == 0 || xPos + width > LCD_W || yPos + height > LCD_H ||
            pixelCount > MAX_PIXELS) {
            continue;
        }

        const size_t pixelBytes = pixelCount * sizeof(uint16_t);
        if (static_cast<size_t>(packetBytes) != HEADER_BYTES + pixelBytes) {
            continue;
        }
        if (socket.read(reinterpret_cast<uint8_t*>(pixels.data()), pixelBytes) != static_cast<int>(pixelBytes)) {
            continue;
        }

        DisplayManager::drawRgb565Rectangle(xPos, yPos, width, height, pixels.data());
        yield();
    }
}
