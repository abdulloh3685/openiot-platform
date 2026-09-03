#include <cassert>
#include <cstring>
#include <openiot/network.hpp>

using openiot::foundation::ErrorCode;
using openiot::network::ConnectionState;
using openiot::network::Json;
using openiot::network::Mqtt;
using openiot::network::WiFi;

int main() {
    WiFi wifi;
    assert(wifi.connect() == ErrorCode::NotInitialized);
    assert(wifi.begin(nullptr, "password") == ErrorCode::InvalidArgument);
    assert(wifi.begin("test-ssid", "password") == ErrorCode::Ok);
    assert(wifi.initialized());
    assert(wifi.state() == ConnectionState::Down);
    wifi.simulateConnected(true);
    assert(wifi.connected());
    wifi.end();
    assert(!wifi.initialized());

    Mqtt mqtt;
    assert(mqtt.publish("topic", "payload") == ErrorCode::NotInitialized);
    assert(mqtt.begin(nullptr, 1883) == ErrorCode::InvalidArgument);
    assert(mqtt.begin("broker", 1883) == ErrorCode::Ok);
    assert(mqtt.publish("topic", "payload") == ErrorCode::InvalidState);
    mqtt.simulateConnected(true);
    assert(mqtt.publish("topic", "payload") == ErrorCode::Ok);
    assert(mqtt.subscribe("topic") == ErrorCode::Ok);
    assert(mqtt.publish(nullptr, "payload") == ErrorCode::InvalidArgument);
    mqtt.end();

    char buffer[64]{};
    assert(Json::encodeString("device", "esp32", buffer, sizeof(buffer)) == ErrorCode::Ok);
    assert(std::strcmp(buffer, "{\"device\":\"esp32\"}") == 0);
    assert(Json::encodeNumber("temperature", 25.5f, buffer, sizeof(buffer)) == ErrorCode::Ok);
    assert(Json::decodeNumber(buffer, "temperature").ok());
    assert(Json::decodeNumber(buffer, "temperature").value() > 25.4f);
    char tiny[8]{};
    assert(Json::encodeString("device", "esp32", tiny, sizeof(tiny)) == ErrorCode::CapacityExceeded);
    assert(Json::decodeNumber("{\"x\":1.0}", "missing").error() == ErrorCode::NotFound);

    return 0;
}
