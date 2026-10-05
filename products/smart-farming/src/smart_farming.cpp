#include <openiot/smart_farming.hpp>

namespace openiot::products {
namespace {
constexpr std::uint16_t kPumpOffThreshold = 2500;
constexpr std::uint16_t kMinimumValidRaw = 1000;
constexpr std::uint16_t kMaximumValidRaw = 3200;

bool isValidSoilReading(const std::uint16_t raw) {
    return raw >= kMinimumValidRaw && raw <= kMaximumValidRaw;
}
}

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
    if (!reading.ok()) {
        (void)relay_.set(false);
        pump_on_ = false;
        return;
    }

    last_soil_raw_ = reading.value();
    ++sample_count_;

    const std::uint16_t raw = reading.value();
    core::Event sensor_event{};
    sensor_event.type = core::EventType::Sensor;
    sensor_event.value = raw;
    (void)bus_.publish(sensor_event);

    if (!isValidSoilReading(raw)) {
        (void)relay_.set(false);
        pump_on_ = false;
        return;
    }

    bool requested_pump_state = pump_on_;
    if (raw >= threshold_) {
        requested_pump_state = true;
    } else if (raw <= kPumpOffThreshold) {
        requested_pump_state = false;
    }

    if (relay_.set(requested_pump_state) != foundation::ErrorCode::Ok) {
        return;
    }

    pump_on_ = requested_pump_state;
}

void SmartFarming::end() {
    if (!initialized_) return;

    (void)relay_.set(false);
    pump_on_ = false;
    initialized_ = false;
}

} // namespace openiot::products
