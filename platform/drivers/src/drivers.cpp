#include <openiot/drivers.hpp>
namespace openiot::drivers {
foundation::ErrorCode Dht22::begin(std::uint8_t pin) { pin_ = pin; ready_ = true; return foundation::ErrorCode::Ok; }
foundation::Result<Dht22Reading> Dht22::read() { if (!ready_) return foundation::ErrorCode::NotInitialized; return Dht22Reading{0.0F, 0.0F}; }
foundation::ErrorCode Ds18b20::begin(std::uint8_t pin) { pin_ = pin; ready_ = true; return foundation::ErrorCode::Ok; }
foundation::Result<float> Ds18b20::readCelsius() { if (!ready_) return foundation::ErrorCode::NotInitialized; return 0.0F; }
foundation::ErrorCode SoilMoisture::begin(std::uint8_t channel) { channel_ = channel; ready_ = true; return foundation::ErrorCode::Ok; }
foundation::Result<std::uint16_t> SoilMoisture::readRaw() { if (!ready_) return foundation::ErrorCode::NotInitialized; return static_cast<std::uint16_t>(0); }
foundation::ErrorCode Relay::begin(std::uint8_t pin) { pin_ = pin; ready_ = true; on_ = false; return foundation::ErrorCode::Ok; }
foundation::ErrorCode Relay::set(bool on) { if (!ready_) return foundation::ErrorCode::NotInitialized; on_ = on; return foundation::ErrorCode::Ok; }
} // namespace openiot::drivers
