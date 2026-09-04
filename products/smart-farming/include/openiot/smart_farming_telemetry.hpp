#pragma once

#include <cstddef>
#include <cstdint>
#include <openiot/network.hpp>
#include <openiot/smart_farming.hpp>

namespace openiot::products {

class SmartFarmingTelemetry final : public foundation::IModule {
public:
    static constexpr std::size_t kPayloadCapacity = 96;

    SmartFarmingTelemetry(SmartFarming& farming, network::Mqtt& mqtt, const char* topic)
        : farming_(farming), mqtt_(mqtt), topic_(topic) {}

    foundation::ErrorCode begin() override;
    void loop() override;
    void end() override;

    const char* lastPayload() const { return payload_; }
    std::uint32_t publishCount() const { return publish_count_; }
    bool initialized() const { return initialized_; }

private:
    SmartFarming& farming_;
    network::Mqtt& mqtt_;
    const char* topic_{nullptr};
    char payload_[kPayloadCapacity]{};
    std::uint32_t publish_count_{0};
    bool initialized_{false};
};

} // namespace openiot::products
