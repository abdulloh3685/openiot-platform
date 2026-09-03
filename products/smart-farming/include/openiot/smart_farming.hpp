#pragma once

#include <cstdint>
#include <openiot/core.hpp>
#include <openiot/drivers.hpp>

namespace openiot::products {

class SmartFarming final : public foundation::IModule {
public:
    SmartFarming(core::EventBus& bus, drivers::SoilMoisture& soil, drivers::Relay& relay)
        : bus_(bus), soil_(soil), relay_(relay) {}

    foundation::ErrorCode begin() override;
    void loop() override;
    void end() override;

    void setThreshold(std::uint16_t threshold) { threshold_ = threshold; }
    std::uint16_t threshold() const { return threshold_; }
    std::uint16_t lastSoilRaw() const { return last_soil_raw_; }
    bool pumpOn() const { return pump_on_; }
    std::uint32_t sampleCount() const { return sample_count_; }
    bool initialized() const { return initialized_; }

private:
    core::EventBus& bus_;
    drivers::SoilMoisture& soil_;
    drivers::Relay& relay_;
    std::uint16_t threshold_{2000};
    std::uint16_t last_soil_raw_{0};
    std::uint32_t sample_count_{0};
    bool pump_on_{false};
    bool initialized_{false};
};

} // namespace openiot::products
