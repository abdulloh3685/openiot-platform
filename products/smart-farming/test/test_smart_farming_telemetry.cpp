#include <cassert>
#include <cstring>

#include <openiot/core.hpp>
#include <openiot/drivers.hpp>
#include <openiot/network.hpp>
#include <openiot/smart_farming.hpp>
#include <openiot/smart_farming_telemetry.hpp>

int main() {
    openiot::core::EventBus bus;
    openiot::drivers::SoilMoisture soil(32);
    openiot::drivers::Relay relay(25);
    openiot::products::SmartFarming farming(bus, soil, relay);
    openiot::network::Mqtt mqtt;

    assert(bus.begin() == openiot::foundation::ErrorCode::Ok);
    assert(soil.begin() == openiot::foundation::ErrorCode::Ok);
    assert(relay.begin() == openiot::foundation::ErrorCode::Ok);
    assert(farming.begin() == openiot::foundation::ErrorCode::Ok);
    assert(mqtt.begin("broker.test", 1883) == openiot::foundation::ErrorCode::Ok);

    openiot::products::SmartFarmingTelemetry telemetry(farming, mqtt, "openiot/farm/node1/telemetry");
    assert(telemetry.begin() == openiot::foundation::ErrorCode::Ok);
    assert(telemetry.begin() == openiot::foundation::ErrorCode::AlreadyInitialized);

    telemetry.loop();
    assert(telemetry.publishCount() == 0);

    mqtt.simulateConnected(true);
    farming.loop();
    telemetry.loop();

    assert(telemetry.publishCount() == 1);
    assert(std::strstr(telemetry.lastPayload(), "\"soil_raw\":0") != nullptr);
    assert(std::strstr(telemetry.lastPayload(), "\"pump_on\":true") != nullptr);
    assert(std::strstr(telemetry.lastPayload(), "\"samples\":1") != nullptr);

    telemetry.end();
    assert(!telemetry.initialized());
    assert(telemetry.publishCount() == 0);
    assert(telemetry.lastPayload()[0] == '\0');

    mqtt.end();
    farming.end();
    relay.end();
    soil.end();
    bus.end();
    return 0;
}
