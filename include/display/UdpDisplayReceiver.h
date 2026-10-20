// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <WiFiUdp.h>
#include <array>
#include <cstddef>
#include <cstdint>

class UdpDisplayReceiver {
   public:
    static constexpr uint16_t PORT = 7777;
    static constexpr size_t HEADER_BYTES = 4;
    static constexpr size_t MAX_PIXELS = 720;
    static constexpr size_t MAX_PACKET_BYTES = HEADER_BYTES + MAX_PIXELS * sizeof(uint16_t);
    static constexpr size_t PACKETS_PER_UPDATE = 4;

    bool begin();
    void update();

   private:
    WiFiUDP socket;
    // A uint16_t array keeps the incoming big-endian pixel bytes aligned for Arduino_GFX.
    std::array<uint16_t, MAX_PIXELS> pixels{};
    bool listening = false;
};
