#include <cassert>
#include <openiot/hal.hpp>

using openiot::foundation::ErrorCode;
using openiot::hal::Adc;
using openiot::hal::Gpio;
using openiot::hal::Pwm;

int main() {
    Gpio gpio;
    assert(gpio.setOutput(2, true) == ErrorCode::NotInitialized);
    assert(gpio.begin() == ErrorCode::Ok);
    assert(gpio.begin() == ErrorCode::AlreadyInitialized);
    assert(gpio.setOutput(2, true) == ErrorCode::Ok);
    assert(gpio.read(2).ok());
    assert(gpio.read(2).value());
    assert(gpio.setOutput(2, false) == ErrorCode::Ok);
    assert(!gpio.read(2).value());
    assert(gpio.setOutput(34, true) == ErrorCode::InvalidArgument);
    assert(gpio.setOutput(6, true) == ErrorCode::InvalidArgument);
    assert(gpio.read(40).error() == ErrorCode::InvalidArgument);
    gpio.end();
    assert(gpio.read(2).error() == ErrorCode::NotInitialized);

    Adc adc;
    assert(adc.read(32).error() == ErrorCode::NotInitialized);
    assert(adc.begin() == ErrorCode::Ok);
    assert(adc.begin() == ErrorCode::AlreadyInitialized);
    auto adc_value = adc.read(32);
    assert(adc_value.ok());
#ifndef ARDUINO
    assert(adc_value.value() == 0);
#endif
    assert(adc.read(5).error() == ErrorCode::InvalidArgument);
    assert(adc.read(40).error() == ErrorCode::InvalidArgument);
    adc.end();
    assert(adc.read(32).error() == ErrorCode::NotInitialized);

    Pwm pwm;
    assert(pwm.set(2, 100) == ErrorCode::NotInitialized);
    assert(pwm.begin() == ErrorCode::Ok);
    assert(pwm.begin() == ErrorCode::AlreadyInitialized);
    assert(pwm.set(2, 0) == ErrorCode::Ok);
    assert(pwm.set(2, 255) == ErrorCode::Ok);
    assert(pwm.set(2, 256) == ErrorCode::InvalidArgument);
    assert(pwm.set(34, 100) == ErrorCode::InvalidArgument);
    assert(pwm.set(6, 100) == ErrorCode::InvalidArgument);
    assert(pwm.set(40, 100) == ErrorCode::InvalidArgument);
    pwm.end();
    assert(pwm.set(2, 100) == ErrorCode::NotInitialized);

    return 0;
}
