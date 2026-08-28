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
    void end() override { initialized_ = false; }
    void setThreshold(std::uint16_t threshold) { threshold_ = threshold; }
private:
    core::EventBus& bus_; drivers::SoilMoisture& soil_; drivers::Relay& relay_;
    std::uint16_t threshold_{2000}; bool initialized_{false};
};
} // namespace openiot::products
