#include <openiot/smart_farming_telemetry.hpp>

#include <cstdio>
#include <cstring>

namespace openiot::products {

foundation::ErrorCode SmartFarmingTelemetry::begin() {
    if (initialized_) return foundation::ErrorCode::AlreadyInitialized;
    if (topic_ == nullptr || topic_[0] == '\0') return foundation::ErrorCode::InvalidArgument;
    if (!farming_.initialized()) return foundation::ErrorCode::NotInitialized;
    if (!mqtt_.initialized()) return foundation::ErrorCode::NotInitialized;

    payload_[0] = '\0';
    publish_count_ = 0;
    initialized_ = true;
    return foundation::ErrorCode::Ok;
}

void SmartFarmingTelemetry::loop() {
    if (!initialized_ || !farming_.initialized() || !mqtt_.connected()) return;

    const int written = std::snprintf(
        payload_, sizeof(payload_),
        "{\"soil_raw\":%u,\"pump_on\":%s,\"samples\":%lu}",
        static_cast<unsigned>(farming_.lastSoilRaw()),
        farming_.pumpOn() ? "true" : "false",
        static_cast<unsigned long>(farming_.sampleCount()));

    if (written < 0 || static_cast<std::size_t>(written) >= sizeof(payload_)) {
        payload_[0] = '\0';
        return;
    }

    if (mqtt_.publish(topic_, payload_) == foundation::ErrorCode::Ok) {
        ++publish_count_;
    }
}

void SmartFarmingTelemetry::end() {
    initialized_ = false;
    payload_[0] = '\0';
    publish_count_ = 0;
}

} // namespace openiot::products
