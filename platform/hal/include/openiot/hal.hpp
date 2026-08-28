#pragma once
#include <cstdint>
#include <openiot/foundation.hpp>
namespace openiot::hal {
class Gpio : public foundation::IModule { public: foundation::ErrorCode begin() override { return foundation::ErrorCode::Ok; } void loop() override {} void end() override {} foundation::ErrorCode setOutput(std::uint8_t pin, bool high); foundation::Result<bool> read(std::uint8_t pin) const; };
class Adc : public foundation::IModule { public: foundation::ErrorCode begin() override { return foundation::ErrorCode::Ok; } void loop() override {} void end() override {} foundation::Result<std::uint16_t> read(std::uint8_t channel) const; };
class Pwm : public foundation::IModule { public: foundation::ErrorCode begin() override { return foundation::ErrorCode::Ok; } void loop() override {} void end() override {} foundation::ErrorCode set(std::uint8_t channel, std::uint16_t duty); };
} // namespace openiot::hal
