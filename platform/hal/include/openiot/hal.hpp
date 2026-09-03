#pragma once

#include <array>
#include <cstdint>
#include <openiot/foundation.hpp>

namespace openiot::hal {

class Gpio final : public foundation::IModule {
public:
    static constexpr std::size_t kMaxPins = 40;

    foundation::ErrorCode begin() override;
    void loop() override {}
    void end() override;

    foundation::ErrorCode setOutput(std::uint8_t pin, bool high);
    foundation::Result<bool> read(std::uint8_t pin) const;

private:
    std::array<bool, kMaxPins> simulated_state_{};
    bool initialized_{false};
};

class Adc final : public foundation::IModule {
public:
    foundation::ErrorCode begin() override;
    void loop() override {}
    void end() override { initialized_ = false; }

    foundation::Result<std::uint16_t> read(std::uint8_t channel) const;

private:
    bool initialized_{false};
};

class Pwm final : public foundation::IModule {
public:
    static constexpr std::size_t kMaxPins = 40;
    static constexpr std::uint16_t kMaxDuty = 255;

    foundation::ErrorCode begin() override;
    void loop() override {}
    void end() override;

    // Existing API is preserved. The channel identifier is the GPIO used for PWM.
    foundation::ErrorCode set(std::uint8_t channel, std::uint16_t duty);

private:
    std::array<std::uint16_t, kMaxPins> simulated_duty_{};
    bool initialized_{false};
};

} // namespace openiot::hal
