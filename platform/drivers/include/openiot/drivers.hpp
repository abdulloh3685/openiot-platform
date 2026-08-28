#pragma once
#include <cstdint>
#include <openiot/foundation.hpp>
namespace openiot::drivers {
struct Dht22Reading { float temperature_c; float humidity_pct; };
class Dht22 { public: foundation::ErrorCode begin(std::uint8_t pin); foundation::Result<Dht22Reading> read(); private: std::uint8_t pin_{0}; bool ready_{false}; };
class Ds18b20 { public: foundation::ErrorCode begin(std::uint8_t pin); foundation::Result<float> readCelsius(); private: std::uint8_t pin_{0}; bool ready_{false}; };
class SoilMoisture { public: foundation::ErrorCode begin(std::uint8_t channel); foundation::Result<std::uint16_t> readRaw(); private: std::uint8_t channel_{0}; bool ready_{false}; };
class Relay { public: foundation::ErrorCode begin(std::uint8_t pin); foundation::ErrorCode set(bool on); bool state() const { return on_; } private: std::uint8_t pin_{0}; bool ready_{false}; bool on_{false}; };
} // namespace openiot::drivers
