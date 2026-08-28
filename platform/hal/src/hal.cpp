#include <openiot/hal.hpp>
namespace openiot::hal {
foundation::ErrorCode Gpio::setOutput(std::uint8_t pin, bool high) { (void)pin; (void)high; return foundation::ErrorCode::Ok; }
foundation::Result<bool> Gpio::read(std::uint8_t pin) const { (void)pin; return false; }
foundation::Result<std::uint16_t> Adc::read(std::uint8_t channel) const { (void)channel; return static_cast<std::uint16_t>(0); }
foundation::ErrorCode Pwm::set(std::uint8_t channel, std::uint16_t duty) { (void)channel; (void)duty; return foundation::ErrorCode::Ok; }
} // namespace openiot::hal
