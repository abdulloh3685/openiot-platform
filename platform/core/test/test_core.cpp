#include <cassert>
#include <cstdint>
#include <string>
#include <openiot/core.hpp>

using openiot::core::Config;
using openiot::core::DeviceManager;
using openiot::core::Event;
using openiot::core::EventBus;
using openiot::core::EventType;
using openiot::core::Logger;
using openiot::core::Scheduler;
using openiot::foundation::ErrorCode;

namespace {
int boot_calls = 0;
int custom_calls = 0;
int periodic_calls = 0;
int immediate_calls = 0;

void on_boot(const Event&) { ++boot_calls; }
void on_custom(const Event&) { ++custom_calls; }
void scheduler_periodic(void*) { ++periodic_calls; }
void scheduler_immediate(void*) { ++immediate_calls; }
}

int main() {
    EventBus bus;
    assert(bus.subscribe(EventType::BootCompleted, on_boot) == ErrorCode::NotInitialized);
    assert(bus.begin() == ErrorCode::Ok);
    assert(bus.begin() == ErrorCode::AlreadyInitialized);
    assert(bus.subscribe(EventType::BootCompleted, nullptr) == ErrorCode::InvalidArgument);
    assert(bus.subscribe(EventType::BootCompleted, on_boot) == ErrorCode::Ok);
    assert(bus.subscribe(EventType::BootCompleted, on_boot) == ErrorCode::Busy);
    assert(bus.publish({EventType::BootCompleted, 1}) == ErrorCode::Ok);
    assert(boot_calls == 1);
    assert(bus.subscribe(EventType::Custom, on_custom) == ErrorCode::Ok);
    assert(bus.publish({EventType::Custom, 7}) == ErrorCode::Ok);
    assert(custom_calls == 1);
    assert(bus.unsubscribe(EventType::BootCompleted, on_boot) == ErrorCode::Ok);
    assert(bus.unsubscribe(EventType::BootCompleted, on_boot) == ErrorCode::NotFound);
    bus.end();
    assert(bus.publish({EventType::Custom, 8}) == ErrorCode::NotInitialized);

    Scheduler scheduler;
    assert(scheduler.registerTask(scheduler_periodic, nullptr, 100).error() == ErrorCode::NotInitialized);
    assert(scheduler.begin() == ErrorCode::Ok);
    assert(scheduler.begin() == ErrorCode::AlreadyInitialized);
    auto periodic_id = scheduler.registerTask(scheduler_periodic, nullptr, 100);
    assert(periodic_id.ok());
    auto immediate_id = scheduler.registerTask(scheduler_immediate, nullptr, 0);
    assert(immediate_id.ok());

    scheduler.tick(0);
    assert(periodic_calls == 0);
    assert(immediate_calls == 1);
    scheduler.tick(99);
    assert(periodic_calls == 0);
    assert(immediate_calls == 2);
    scheduler.tick(1);
    assert(periodic_calls == 1);
    assert(immediate_calls == 3);
    scheduler.tick(250);
    assert(periodic_calls == 2);
    assert(immediate_calls == 4);
    assert(scheduler.removeTask(periodic_id.value()) == ErrorCode::Ok);
    assert(scheduler.removeTask(periodic_id.value()) == ErrorCode::NotFound);
    assert(scheduler.removeTask(255) == ErrorCode::NotFound);
    scheduler.tick(1000);
    assert(periodic_calls == 2);
    assert(immediate_calls == 5);
    scheduler.end();
    assert(scheduler.removeTask(immediate_id.value()) == ErrorCode::NotInitialized);

    Config config;
    assert(config.set("device.name", "ESP32-001") == ErrorCode::NotInitialized);
    assert(config.begin() == ErrorCode::Ok);
    assert(config.begin() == ErrorCode::AlreadyInitialized);
    assert(config.set(nullptr, "x") == ErrorCode::InvalidArgument);
    assert(config.set("device.name", "ESP32-001") == ErrorCode::Ok);
    assert(config.set("device.name", "ESP32-002") == ErrorCode::Ok);
    auto name = config.get("device.name");
    assert(name.ok());
    assert(std::string(name.value()) == "ESP32-002");
    assert(config.get("missing").error() == ErrorCode::NotFound);
    config.end();
    assert(config.get("device.name").error() == ErrorCode::NotInitialized);

    DeviceManager device;
    assert(device.id() != nullptr);
    assert(device.begin() == ErrorCode::Ok);
    assert(device.begin() == ErrorCode::AlreadyInitialized);
    assert(std::string(device.id()) == "ESP32-001");
    device.end();

    Logger logger;
    EventBus boot_bus;
    Scheduler boot_scheduler;
    Config boot_config;
    DeviceManager boot_device;
    openiot::core::BootManager boot(logger, boot_bus, boot_scheduler, boot_config, boot_device);
    boot_calls = 0;
    assert(boot.begin() == ErrorCode::Ok);
    assert(boot.begin() == ErrorCode::AlreadyInitialized);
    assert(boot_calls == 0); // No subscriber was registered before boot.
    assert(boot_device.id() != nullptr);
    boot.end();
    assert(boot_bus.publish({EventType::BootCompleted, 2}) == ErrorCode::NotInitialized);
    assert(boot.begin() == ErrorCode::Ok);
    boot.end();

    return 0;
}
