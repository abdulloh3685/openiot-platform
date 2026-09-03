#pragma once

#include <cstddef>
#include <cstdint>
#include <openiot/foundation.hpp>

namespace openiot::network {

enum class ConnectionState : std::uint8_t {
    Down = 0,
    Connecting,
    Connected,
    Error
};

struct NetworkConfig {
    const char* ssid{nullptr};
    const char* password{nullptr};
    std::uint32_t connect_timeout_ms{10000};
    std::uint32_t reconnect_interval_ms{5000};
};

class WiFi {
public:
    foundation::ErrorCode begin(const char* ssid, const char* password);
    foundation::ErrorCode begin(const NetworkConfig& config);
    foundation::ErrorCode connect();
    void loop();
    void end();

    bool connected() const { return state_ == ConnectionState::Connected; }
    bool initialized() const { return initialized_; }
    ConnectionState state() const { return state_; }

#ifndef ARDUINO
    void simulateConnected(bool connected);
#endif

private:
    NetworkConfig config_{};
    ConnectionState state_{ConnectionState::Down};
    bool initialized_{false};
    std::uint32_t last_attempt_ms_{0};
};

struct MqttConfig {
    const char* host{nullptr};
    std::uint16_t port{0};
    std::uint32_t reconnect_interval_ms{5000};
};

class Mqtt {
public:
    foundation::ErrorCode begin(const char* host, std::uint16_t port);
    foundation::ErrorCode begin(const MqttConfig& config);
    foundation::ErrorCode connect();
    foundation::ErrorCode publish(const char* topic, const char* payload);
    foundation::ErrorCode subscribe(const char* topic);
    void loop();
    void end();

    bool connected() const { return state_ == ConnectionState::Connected; }
    bool initialized() const { return initialized_; }
    ConnectionState state() const { return state_; }

#ifndef ARDUINO
    void simulateConnected(bool connected);
#endif

private:
    MqttConfig config_{};
    ConnectionState state_{ConnectionState::Down};
    bool initialized_{false};
    std::uint32_t last_attempt_ms_{0};
};

class Json {
public:
    static foundation::ErrorCode encodeString(const char* key,
                                               const char* value,
                                               char* out,
                                               std::size_t out_size);
    static foundation::ErrorCode encodeNumber(const char* key,
                                               float value,
                                               char* out,
                                               std::size_t out_size);
    static foundation::Result<float> decodeNumber(const char* json,
                                                   const char* key);
};

} // namespace openiot::network
