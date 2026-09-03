#include <openiot/smart_farming.hpp>

namespace openiot::products {

foundation::ErrorCode SmartFarming::begin() {
    if (initialized_) return foundation::ErrorCode::AlreadyInitialized;
    initialized_ = true;
    last_soil_raw_ = 0;
    sample_count_ = 0;
    pump_on_ = false;
    return foundation::ErrorCode::Ok;
}

void SmartFarming::loop() {
    if (!initialized_) return;

    const auto reading = soil_.readRaw();
    if (!reading.ok()) return;

    last_soil_raw_ = reading.value();
    ++sample_count_;

    const bool requested_pump_state = reading.value() < threshold_;
    if (relay_.set(requested_pump_state) != foundation::ErrorCode::Ok) return;

    pump_on_ = requested_pump_state;
    (void)bus_.publish({core::EventType::Sensor, reading.value()});
}

void SmartFarming::end() {
    if (!initialized_) return;
    (void)relay_.set(false);
    pump_on_ = false;
    initialized_ = false;
}

} // namespace openiot::products
