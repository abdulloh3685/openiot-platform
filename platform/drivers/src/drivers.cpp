#include <openiot/drivers.hpp>
#ifdef ARDUINO
#include <Arduino.h>
#include <DHT.h>
#include <cmath>
#endif

namespace openiot::drivers {
foundation::ErrorCode Dht22::begin(std::uint8_t pin) {
    pin_ = pin;
    ready_ = true;
    return foundation::ErrorCode::Ok;
}

foundation::Result<Dht22Reading> Dht22::read() {
    if (!ready_) return foundation::ErrorCode::NotInitialized;

#ifdef ARDUINO
    DHT sensor(pin_, DHT22);
    sensor.begin();
    const float humidity = sensor.readHumidity();
    const float temperature = sensor.readTemperature();
    if (std::isnan(humidity) || std::isnan(temperature)) {
        return foundation::ErrorCode::HardwareError;
    }
    return Dht22Reading{temperature, humidity};
#else
    return Dht22Reading{0.0F, 0.0F};
#endif
}
foundation::ErrorCode Ds18b20::begin(std::uint8_t pin) { pin_ = pin; ready_ = true; return foundation::ErrorCode::Ok; }
foundation::Result<float> Ds18b20::readCelsius() { if (!ready_) return foundation::ErrorCode::NotInitialized; return 0.0F; }
foundation::ErrorCode SoilMoisture::begin(std::uint8_t channel) { channel_ = channel; ready_ = true; return foundation::ErrorCode::Ok; }
foundation::Result<std::uint16_t> SoilMoisture::readRaw() {
    if (!ready_) return foundation::ErrorCode::NotInitialized;
#ifdef ARDUINO
    return static_cast<std::uint16_t>(analogRead(channel_));
#else
    return static_cast<std::uint16_t>(0);
#endif
}
foundation::ErrorCode Relay::begin(std::uint8_t pin) {
    return begin(pin, false);
}

foundation::ErrorCode Relay::begin(std::uint8_t pin, bool active_low) {
    pin_ = pin;
    active_low_ = active_low;
    on_ = false;

#ifdef ARDUINO
    pinMode(pin_, OUTPUT);
    digitalWrite(pin_, active_low_ ? HIGH : LOW);
#endif

    ready_ = true;
    return foundation::ErrorCode::Ok;
}

foundation::ErrorCode Relay::set(bool on) {
    if (!ready_) return foundation::ErrorCode::NotInitialized;
    on_ = on;

#ifdef ARDUINO
    const bool level_high = active_low_ ? !on_ : on_;
    digitalWrite(pin_, level_high ? HIGH : LOW);
#endif

    return foundation::ErrorCode::Ok;
}
} // namespace openiot::drivers
