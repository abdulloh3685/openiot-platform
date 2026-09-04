#include <cassert>
#include <cstdint>
#include <openiot/core.hpp>
#include <openiot/drivers.hpp>
#include <openiot/smart_farming.hpp>

namespace {
std::uint32_t sensor_events = 0;
std::uint32_t last_sensor_value = 0;

void on_sensor(const openiot::core::Event& event) {
    ++sensor_events;
    last_sensor_value = event.value;
}
}

int main() {
    openiot::core::EventBus bus;
    openiot::drivers::SoilMoisture soil;
    openiot::drivers::Relay relay;

    assert(bus.begin() == openiot::foundation::ErrorCode::Ok);
    assert(soil.begin(32) == openiot::foundation::ErrorCode::Ok);
    assert(relay.begin(25) == openiot::foundation::ErrorCode::Ok);

    assert(bus.subscribe(openiot::core::EventType::Sensor, on_sensor) == openiot::foundation::ErrorCode::Ok);

    openiot::products::SmartFarming farming(bus, soil, relay);
    assert(farming.begin() == openiot::foundation::ErrorCode::Ok);
    assert(farming.begin() == openiot::foundation::ErrorCode::AlreadyInitialized);
    assert(farming.initialized());
    assert(farming.threshold() == 2000);

    farming.loop();

    assert(farming.sampleCount() == 1);
    assert(farming.lastSoilRaw() == 0);
    assert(farming.pumpOn());
    assert(relay.state());
    assert(sensor_events == 1);
    assert(last_sensor_value == 0);

    farming.setThreshold(0);
    farming.loop();
    assert(farming.sampleCount() == 2);
    assert(!farming.pumpOn());
    assert(!relay.state());
    assert(sensor_events == 2);

    farming.end();
    assert(!farming.initialized());
    assert(!farming.pumpOn());
    assert(!relay.state());

    return 0;
}
