#include <openiot/network.hpp>

#include <cstdio>
#include <cstring>

#ifdef ARDUINO
#include <WiFi.h>
#include <PubSubClient.h>
#endif

namespace openiot::network {

namespace {
#ifndef ARDUINO
std::uint32_t nowMs() { return 0; }
#else
std::uint32_t nowMs() { return millis(); }
#endif

bool validString(const char* value) {
    return value != nullptr && value[0] != '\0';
}

} // namespace

foundation::ErrorCode WiFi::begin(const char* ssid, const char* password) {
    NetworkConfig config{};
    config.ssid = ssid;
    config.password = password;
    return begin(config);
}

foundation::ErrorCode WiFi::begin(const NetworkConfig& config) {
    if (!validString(config.ssid) || config.password == nullptr || config.connect_timeout_ms == 0 || config.reconnect_interval_ms == 0) {
        return foundation::ErrorCode::InvalidArgument;
    }
    config_ = config;
    initialized_ = true;
    state_ = ConnectionState::Down;
    last_attempt_ms_ = 0;
    return foundation::ErrorCode::Ok;
}

foundation::ErrorCode WiFi::connect() {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
#ifdef ARDUINO
    state_ = ConnectionState::Connecting;
    ::WiFi.begin(config_.ssid, config_.password);
    last_attempt_ms_ = nowMs();
    if (::WiFi.status() == WL_CONNECTED) {
        state_ = ConnectionState::Connected;
        return foundation::ErrorCode::Ok;
    }
    return foundation::ErrorCode::Busy;
#else
    state_ = ConnectionState::Connecting;
    last_attempt_ms_ = nowMs();
    return foundation::ErrorCode::Busy;
#endif
}

void WiFi::loop() {
    if (!initialized_) return;
#ifdef ARDUINO
    if (::WiFi.status() == WL_CONNECTED) {
        state_ = ConnectionState::Connected;
        return;
    }
    if (state_ == ConnectionState::Connected) state_ = ConnectionState::Down;
    const auto elapsed = nowMs() - last_attempt_ms_;
    if (state_ != ConnectionState::Connecting && elapsed >= config_.reconnect_interval_ms) {
        (void)connect();
    }
    if (state_ == ConnectionState::Connecting && elapsed >= config_.connect_timeout_ms) {
        state_ = ConnectionState::Error;
    }
#endif
}

void WiFi::end() {
#ifdef ARDUINO
    ::WiFi.disconnect(true);
#endif
    initialized_ = false;
    state_ = ConnectionState::Down;
    last_attempt_ms_ = 0;
}

#ifndef ARDUINO
void WiFi::simulateConnected(bool connected) {
    if (!initialized_) return;
    state_ = connected ? ConnectionState::Connected : ConnectionState::Down;
}
#endif

#ifdef ARDUINO
namespace {
WiFiClient mqtt_wifi_client;
PubSubClient mqtt_client(mqtt_wifi_client);
}
#endif

foundation::ErrorCode Mqtt::begin(const char* host, std::uint16_t port) {
    MqttConfig config{};
    config.host = host;
    config.port = port;
    return begin(config);
}

foundation::ErrorCode Mqtt::begin(const MqttConfig& config) {
    if (!validString(config.host) || config.port == 0 || config.reconnect_interval_ms == 0) {
        return foundation::ErrorCode::InvalidArgument;
    }
    config_ = config;
    initialized_ = true;
    state_ = ConnectionState::Down;
    last_attempt_ms_ = 0;
#ifdef ARDUINO
    mqtt_client.setServer(config_.host, config_.port);
#endif
    return foundation::ErrorCode::Ok;
}

foundation::ErrorCode Mqtt::connect() {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
#ifdef ARDUINO
    if (::WiFi.status() != WL_CONNECTED) {
        state_ = ConnectionState::Down;
        return foundation::ErrorCode::InvalidState;
    }
    state_ = ConnectionState::Connecting;
    last_attempt_ms_ = nowMs();
    if (mqtt_client.connect("openiot-node")) {
        state_ = ConnectionState::Connected;
        return foundation::ErrorCode::Ok;
    }
    state_ = ConnectionState::Error;
    return foundation::ErrorCode::HardwareError;
#else
    state_ = ConnectionState::Connecting;
    last_attempt_ms_ = nowMs();
    return foundation::ErrorCode::Busy;
#endif
}

foundation::ErrorCode Mqtt::publish(const char* topic, const char* payload) {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
    if (!validString(topic) || payload == nullptr || payload[0] == '\0') return foundation::ErrorCode::InvalidArgument;
    if (!connected()) return foundation::ErrorCode::InvalidState;
#ifdef ARDUINO
    return mqtt_client.publish(topic, payload) ? foundation::ErrorCode::Ok : foundation::ErrorCode::HardwareError;
#else
    return foundation::ErrorCode::Ok;
#endif
}

foundation::ErrorCode Mqtt::subscribe(const char* topic) {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
    if (!validString(topic)) return foundation::ErrorCode::InvalidArgument;
    if (!connected()) return foundation::ErrorCode::InvalidState;
#ifdef ARDUINO
    return mqtt_client.subscribe(topic) ? foundation::ErrorCode::Ok : foundation::ErrorCode::HardwareError;
#else
    return foundation::ErrorCode::Ok;
#endif
}

void Mqtt::loop() {
    if (!initialized_) return;
#ifdef ARDUINO
    if (!mqtt_client.connected()) {
        if (state_ == ConnectionState::Connected) state_ = ConnectionState::Down;
        const auto elapsed = nowMs() - last_attempt_ms_;
        if (elapsed >= config_.reconnect_interval_ms) (void)connect();
        return;
    }
    state_ = ConnectionState::Connected;
    mqtt_client.loop();
#endif
}

void Mqtt::end() {
#ifdef ARDUINO
    mqtt_client.disconnect();
#endif
    initialized_ = false;
    state_ = ConnectionState::Down;
    last_attempt_ms_ = 0;
}

#ifndef ARDUINO
void Mqtt::simulateConnected(bool connected) {
    if (!initialized_) return;
    state_ = connected ? ConnectionState::Connected : ConnectionState::Down;
}
#endif

foundation::ErrorCode Json::encodeString(const char* key, const char* value, char* out, std::size_t out_size) {
    if (!validString(key) || value == nullptr || out == nullptr || out_size == 0) return foundation::ErrorCode::InvalidArgument;
    const int written = std::snprintf(out, out_size, "{\"%s\":\"%s\"}", key, value);
    if (written < 0 || static_cast<std::size_t>(written) >= out_size) return foundation::ErrorCode::CapacityExceeded;
    return foundation::ErrorCode::Ok;
}

foundation::ErrorCode Json::encodeNumber(const char* key, float value, char* out, std::size_t out_size) {
    if (!validString(key) || out == nullptr || out_size == 0) return foundation::ErrorCode::InvalidArgument;
    const int written = std::snprintf(out, out_size, "{\"%s\":%.3f}", key, static_cast<double>(value));
    if (written < 0 || static_cast<std::size_t>(written) >= out_size) return foundation::ErrorCode::CapacityExceeded;
    return foundation::ErrorCode::Ok;
}

foundation::Result<float> Json::decodeNumber(const char* json, const char* key) {
    if (!validString(json) || !validString(key)) return foundation::ErrorCode::InvalidArgument;
    char pattern[64]{};
    const int written = std::snprintf(pattern, sizeof(pattern), "\"%s\":", key);
    if (written < 0 || static_cast<std::size_t>(written) >= sizeof(pattern)) return foundation::ErrorCode::CapacityExceeded;
    const char* value = std::strstr(json, pattern);
    if (!value) return foundation::ErrorCode::NotFound;
    value += std::strlen(pattern);
    char* end = nullptr;
    const float result = std::strtof(value, &end);
    if (end == value) return foundation::ErrorCode::InvalidArgument;
    return result;
}

} // namespace openiot::network
