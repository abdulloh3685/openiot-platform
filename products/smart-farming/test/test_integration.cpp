#include <cassert>
#include <cstdint>
#include <cstring>

#include <openiot/core.hpp>
#include <openiot/drivers.hpp>
#include <openiot/network.hpp>
#include <openiot/smart_farming.hpp>
#include <openiot/smart_farming_telemetry.hpp>

namespace {
std::uint32_t sensor_events = 0;
std::uint32_t scheduler_ticks = 0;

void onSensor(const openiot::core::Event& event) {
    if (event.type == openiot::core::EventType::Sensor) ++sensor_events;
}

void scheduledTask(void*) { ++scheduler_ticks; }
}

int main() {
    using namespace openiot;

    core::EventBus bus;
    core::Scheduler scheduler;
    core::Logger logger;
    core::Config config;
    core::DeviceManager device;
    core::BootManager boot(logger, bus, scheduler, config, device);

    assert(boot.begin() == foundation::ErrorCode::Ok);
    assert(boot.begin() == foundation::ErrorCode::AlreadyInitialized);
    assert(bus.subscribe(core::EventType::Sensor, onSensor) == foundation::ErrorCode::Ok);

    auto task = scheduler.registerTask(scheduledTask, nullptr, 100);
    assert(task.ok());

    drivers::SoilMoisture soil;
    drivers::Relay relay;
    assert(soil.begin(32) == foundation::ErrorCode::Ok);
    assert(relay.begin(25) == foundation::ErrorCode::Ok);

    products::SmartFarming farming(bus, soil, relay);
    network::Mqtt mqtt;
    products::SmartFarmingTelemetry telemetry(farming, mqtt, "farm/node/telemetry");

    assert(mqtt.begin("127.0.0.1", 1883) == foundation::ErrorCode::Ok);
    assert(farming.begin() == foundation::ErrorCode::Ok);
    assert(telemetry.begin() == foundation::ErrorCode::Ok);
    mqtt.simulateConnected(true);

    for (std::uint32_t i = 0; i < 1000; ++i) {
        scheduler.tick(1);
        farming.loop();
        telemetry.loop();
    }

    assert(scheduler_ticks == 10);
    assert(farming.sampleCount() == 1000);
    assert(sensor_events == 1000);
    assert(telemetry.publishCount() == 1000);
    assert(std::strstr(telemetry.lastPayload(), "\"soil_raw\":0") != nullptr);
    assert(std::strstr(telemetry.lastPayload(), "\"pump_on\":true") != nullptr);

    mqtt.simulateConnected(false);
    const auto published_before_loss = telemetry.publishCount();
    farming.loop();
    telemetry.loop();
    assert(farming.sampleCount() == 1001);
    assert(sensor_events == 1001);
    assert(telemetry.publishCount() == published_before_loss);

    for (int cycle = 0; cycle < 10; ++cycle) {
        telemetry.end();
        farming.end();
        mqtt.end();
        assert(!telemetry.initialized());
        assert(!farming.initialized());
        assert(!mqtt.initialized());

        assert(mqtt.begin("127.0.0.1", 1883) == foundation::ErrorCode::Ok);
        assert(farming.begin() == foundation::ErrorCode::Ok);
        assert(telemetry.begin() == foundation::ErrorCode::Ok);
        mqtt.simulateConnected(true);
        farming.loop();
        telemetry.loop();
        assert(telemetry.publishCount() == 1);
    }

    telemetry.end();
    farming.end();
    mqtt.end();
    assert(bus.unsubscribe(core::EventType::Sensor, onSensor) == foundation::ErrorCode::Ok);
    boot.end();
    assert(boot.begin() == foundation::ErrorCode::Ok);
    boot.end();
    return 0;
}
