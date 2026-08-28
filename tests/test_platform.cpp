#include <cassert>
#include <openiot/core.hpp>
#include <openiot/drivers.hpp>
#include <openiot/hal.hpp>
#include <openiot/smart_farming.hpp>
int main() {
    using namespace openiot;
    core::EventBus bus; core::Logger logger; core::Scheduler scheduler; core::Config config; core::DeviceManager device;
    core::BootManager boot(logger, bus, scheduler, config, device);
    assert(boot.begin() == foundation::ErrorCode::Ok);
    assert(config.set("device.name", "ESP32-001") == foundation::ErrorCode::Ok);
    assert(config.get("device.name").ok());
    drivers::SoilMoisture soil; drivers::Relay relay;
    assert(soil.begin(0) == foundation::ErrorCode::Ok);
    assert(relay.begin(2) == foundation::ErrorCode::Ok);
    products::SmartFarming farming(bus, soil, relay);
    assert(farming.begin() == foundation::ErrorCode::Ok);
    farming.loop();
    hal::Gpio gpio; hal::Adc adc; hal::Pwm pwm;
    assert(gpio.begin() == foundation::ErrorCode::Ok);
    assert(adc.begin() == foundation::ErrorCode::Ok);
    assert(pwm.begin() == foundation::ErrorCode::Ok);
    return 0;
}
