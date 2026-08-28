#include <cassert>
#include <openiot/core.hpp>
using namespace openiot::core;
static int calls = 0;
static void on_boot(const Event&) { ++calls; }
int main() {
    EventBus bus; Logger logger; Scheduler scheduler; Config config; DeviceManager device;
    assert(bus.begin() == openiot::foundation::ErrorCode::Ok);
    assert(bus.subscribe(EventType::BootCompleted, on_boot) == openiot::foundation::ErrorCode::Ok);
    assert(bus.publish({EventType::BootCompleted, 1}) == openiot::foundation::ErrorCode::Ok);
    assert(calls == 1);
    assert(config.begin() == openiot::foundation::ErrorCode::Ok);
    assert(config.set("device.name", "ESP32-001") == openiot::foundation::ErrorCode::Ok);
    assert(config.get("device.name").ok());
    assert(device.begin() == openiot::foundation::ErrorCode::Ok);
    return 0;
}
