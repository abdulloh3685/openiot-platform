#pragma once
#include <cstdint>
#include <openiot/foundation.hpp>
namespace openiot::network {
class WiFi { public: foundation::ErrorCode begin(const char* ssid, const char* password); bool connected() const { return connected_; } void loop() {} private: bool connected_{false}; };
class Mqtt { public: foundation::ErrorCode begin(const char* host, std::uint16_t port); foundation::ErrorCode publish(const char* topic, const char* payload); bool connected() const { return connected_; } private: bool connected_{false}; };
} // namespace openiot::network
