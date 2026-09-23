#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <openiot/core.hpp>
#include <openiot/drivers.hpp>
#include <openiot/hal.hpp>

using namespace openiot;

static core::Logger logger;
static core::EventBus event_bus;
static core::Scheduler scheduler;
static core::Config config;
static core::DeviceManager device;
static core::BootManager boot(logger, event_bus, scheduler, config, device);
static hal::Gpio gpio;
static hal::Adc adc;
static hal::Pwm pwm;
static drivers::Dht22 dht22;
static drivers::SoilMoisture soil_moisture;
static drivers::Relay relay_channels[4];

namespace {
constexpr std::uint8_t kDht22Pin = 4;
constexpr std::uint8_t kSoilMoisturePin = 34;
constexpr std::uint32_t kSensorSampleIntervalMs = 2500;
constexpr std::uint8_t kRelayPins[] = {26, 32, 33, 25};
}

void setup()
{
#ifdef ARDUINO
    Serial.begin(115200);
    delay(500);

    Serial.println();
    Serial.println("================================");
    Serial.println(" OpenIoT Framework");
    Serial.println(" Logger Validation");
    Serial.println("================================");
    Serial.println("[BOOT-01] setup entered");
    Serial.println("[BOOT-02] before boot.begin()");
#endif

    const foundation::ErrorCode boot_result = boot.begin();

#ifdef ARDUINO
    Serial.println("[BOOT-03] boot.begin() returned");
    if (boot_result == foundation::ErrorCode::Ok) {
        Serial.println("[BOOT-OK] BootManager initialization PASS");
    } else {
        Serial.print("[BOOT-FAIL] BootManager initialization failed; ErrorCode=");
        Serial.println(static_cast<unsigned int>(boot_result));
    }
#endif

    if (boot_result == foundation::ErrorCode::Ok) {
#ifdef ARDUINO
        Serial.println("[HAL-01] Starting safe HAL readiness validation");
#endif
        const foundation::ErrorCode gpio_result = gpio.begin();
        const foundation::ErrorCode adc_result = adc.begin();
        const foundation::ErrorCode pwm_result = pwm.begin();

#ifdef ARDUINO
        Serial.print("[HAL-GPIO] ErrorCode=");
        Serial.println(static_cast<unsigned int>(gpio_result));
        Serial.print("[HAL-ADC] ErrorCode=");
        Serial.println(static_cast<unsigned int>(adc_result));
        Serial.print("[HAL-PWM] ErrorCode=");
        Serial.println(static_cast<unsigned int>(pwm_result));
#endif

        if (gpio_result == foundation::ErrorCode::Ok &&
            adc_result == foundation::ErrorCode::Ok &&
            pwm_result == foundation::ErrorCode::Ok) {
            logger.info("HAL readiness validation PASS; no physical pins were driven");
        } else {
            logger.error("HAL readiness validation FAILED");
        }
    }

    const foundation::ErrorCode dht_result = dht22.begin(kDht22Pin);
    const foundation::ErrorCode soil_result = soil_moisture.begin(kSoilMoisturePin);

#ifdef ARDUINO
    Serial.print("[SENSOR-DHT22] ErrorCode=");
    Serial.println(static_cast<unsigned int>(dht_result));
    Serial.print("[SENSOR-SOIL] ErrorCode=");
    Serial.println(static_cast<unsigned int>(soil_result));
#endif

    if (dht_result == foundation::ErrorCode::Ok &&
        soil_result == foundation::ErrorCode::Ok) {
        logger.info("DHT22 and soil-moisture sampling initialized");
    } else {
        logger.error("Sensor sampling initialization FAILED");
    }

    bool relays_safe = true;
    for (std::size_t index = 0; index < 4; ++index) {
        const foundation::ErrorCode relay_result =
            relay_channels[index].begin(kRelayPins[index], true);
        if (relay_result != foundation::ErrorCode::Ok) {
            relays_safe = false;
        }
#ifdef ARDUINO
        Serial.print("[RELAY-CH");
        Serial.print(index + 1);
        Serial.print("] ErrorCode=");
        Serial.println(static_cast<unsigned int>(relay_result));
#endif
    }

    if (relays_safe) {
        logger.info("Relay channels initialized OFF; active-low configuration");
    } else {
        logger.error("Relay safe-off initialization FAILED");
    }

#ifdef ARDUINO
    Serial.println("[BOOT-LOGGER-01] before logger.info()");
#endif

    // This validates the OpenIoT Logger API.  The current Logger backend is
    logger.info("OpenIoT Logger setup validation");

#ifdef ARDUINO
    Serial.println("[BOOT-LOGGER-02] after logger.info()");
#endif
}

void loop()
{
#ifdef ARDUINO
    Serial.println("[BOOT-04] before boot.loop()");
#endif

    boot.loop();

#ifdef ARDUINO
    Serial.println("[BOOT-05] boot.loop() returned");
#endif

    logger.info("OpenIoT Logger loop validation");

#ifdef ARDUINO
    static std::uint32_t last_sensor_sample_ms = 0;
    const std::uint32_t now = static_cast<std::uint32_t>(millis());
    if (now - last_sensor_sample_ms >= kSensorSampleIntervalMs) {
        last_sensor_sample_ms = now;

        const auto dht_reading = dht22.read();
        if (dht_reading.ok()) {
            Serial.print("[DHT22] temperature_c=");
            Serial.print(dht_reading.value().temperature_c, 1);
            Serial.print(" humidity_pct=");
            Serial.println(dht_reading.value().humidity_pct, 1);
        } else {
            Serial.print("[DHT22-FAIL] ErrorCode=");
            Serial.println(static_cast<unsigned int>(dht_reading.error()));
        }

        const auto soil_reading = soil_moisture.readRaw();
        if (soil_reading.ok()) {
            Serial.print("[SOIL] raw_adc=");
            Serial.println(soil_reading.value());
        } else {
            Serial.print("[SOIL-FAIL] ErrorCode=");
            Serial.println(static_cast<unsigned int>(soil_reading.error()));
        }
    }

    Serial.println("[BOOT-LOGGER] Logger loop validation completed");
    delay(1000);
#endif
}

#ifndef ARDUINO
int main()
{
    setup();
    loop();
    return 0;
}
#endif
