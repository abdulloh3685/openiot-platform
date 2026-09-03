#include <openiot/hal.hpp>

#ifdef ARDUINO
#include <Arduino.h>
#endif

namespace openiot::hal {
namespace {
constexpr bool validPin(std::uint8_t pin) { return pin < 40U; }
constexpr bool outputCapable(std::uint8_t pin) {
    // ESP32 GPIO34..39 are input-only; GPIO6..11 are reserved for SPI flash.
    return validPin(pin) && pin < 34U && !(pin >= 6U && pin <= 11U);
}
constexpr bool adcCapable(std::uint8_t pin) {
    return pin == 0U || pin == 2U || pin == 4U ||
           (pin >= 12U && pin <= 15U) ||
           (pin >= 25U && pin <= 27U) ||
           (pin >= 32U && pin <= 39U);
}
} // namespace

foundation::ErrorCode Gpio::begin() {
    if (initialized_) return foundation::ErrorCode::AlreadyInitialized;
    simulated_state_.fill(false);
    initialized_ = true;
    return foundation::ErrorCode::Ok;
}

void Gpio::end() {
    simulated_state_.fill(false);
    initialized_ = false;
}

foundation::ErrorCode Gpio::setOutput(std::uint8_t pin, bool high) {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
    if (!outputCapable(pin)) return foundation::ErrorCode::InvalidArgument;

#ifdef ARDUINO
    pinMode(pin, OUTPUT);
    digitalWrite(pin, high ? HIGH : LOW);
#else
    simulated_state_[pin] = high;
#endif
    return foundation::ErrorCode::Ok;
}

foundation::Result<bool> Gpio::read(std::uint8_t pin) const {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
    if (!validPin(pin)) return foundation::ErrorCode::InvalidArgument;

#ifdef ARDUINO
    return digitalRead(pin) == HIGH;
#else
    return simulated_state_[pin];
#endif
}

foundation::ErrorCode Adc::begin() {
    if (initialized_) return foundation::ErrorCode::AlreadyInitialized;
    initialized_ = true;
    return foundation::ErrorCode::Ok;
}

foundation::Result<std::uint16_t> Adc::read(std::uint8_t channel) const {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
    if (!adcCapable(channel)) return foundation::ErrorCode::InvalidArgument;

#ifdef ARDUINO
    return static_cast<std::uint16_t>(analogRead(channel));
#else
    // Deterministic hardware-independent native simulation.
    return static_cast<std::uint16_t>(0);
#endif
}

foundation::ErrorCode Pwm::begin() {
    if (initialized_) return foundation::ErrorCode::AlreadyInitialized;
    simulated_duty_.fill(0);
    initialized_ = true;
    return foundation::ErrorCode::Ok;
}

void Pwm::end() {
    simulated_duty_.fill(0);
    initialized_ = false;
}

foundation::ErrorCode Pwm::set(std::uint8_t channel, std::uint16_t duty) {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
    if (!outputCapable(channel) || duty > kMaxDuty) return foundation::ErrorCode::InvalidArgument;

#ifdef ARDUINO
    analogWrite(channel, static_cast<int>(duty));
#else
    simulated_duty_[channel] = duty;
#endif
    return foundation::ErrorCode::Ok;
}

} // namespace openiot::hal
