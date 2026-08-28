#include <openiot/network.hpp>
namespace openiot::network {
foundation::ErrorCode WiFi::begin(const char* ssid, const char* password) { if (!ssid || !password) return foundation::ErrorCode::InvalidArgument; connected_ = false; return foundation::ErrorCode::Ok; }
foundation::ErrorCode Mqtt::begin(const char* host, std::uint16_t port) { if (!host || port == 0) return foundation::ErrorCode::InvalidArgument; connected_ = false; return foundation::ErrorCode::Ok; }
foundation::ErrorCode Mqtt::publish(const char* topic, const char* payload) { if (!connected_) return foundation::ErrorCode::NotInitialized; if (!topic || !payload) return foundation::ErrorCode::InvalidArgument; return foundation::ErrorCode::Ok; }
} // namespace openiot::network
