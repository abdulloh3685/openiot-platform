#include <openiot/smart_farming.hpp>
namespace openiot::products {
foundation::ErrorCode SmartFarming::begin() { initialized_ = true; return foundation::ErrorCode::Ok; }
void SmartFarming::loop() {
    if (!initialized_) return;
    auto reading = soil_.readRaw();
    if (!reading.ok()) return;
    relay_.set(reading.value() < threshold_);
    bus_.publish({core::EventType::Sensor, reading.value()});
}
} // namespace openiot::products
