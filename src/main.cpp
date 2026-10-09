#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <openiot/core.hpp>
#include <openiot/drivers.hpp>
#include <openiot/hal.hpp>
#include <openiot/network.hpp>
#include <openiot/smart_farming.hpp>

#if defined(ARDUINO) && __has_include(<openiot/network_secrets.hpp>)
#include <openiot/network_secrets.hpp>
#define OPENIOT_WIFI_SECRETS_AVAILABLE 1
#else
#define OPENIOT_WIFI_SECRETS_AVAILABLE 0
#endif

#ifndef OPENIOT_BUILD_NUMBER
#define OPENIOT_BUILD_NUMBER 0
#endif

#ifndef OPENIOT_GIT_COMMIT
#define OPENIOT_GIT_COMMIT "unknown"
#endif

#ifndef OPENIOT_GIT_STATE
#define OPENIOT_GIT_STATE "unknown"
#endif

#ifndef OPENIOT_BUILD_ENV
#define OPENIOT_BUILD_ENV "unknown"
#endif


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
static network::WiFi wifi;
static drivers::Dht22 dht22;
static drivers::SoilMoisture soil_moisture;
static drivers::Relay relay_channels[4];

namespace {
constexpr std::uint8_t kDht22Pin = 4;
constexpr std::uint8_t kSoilMoisturePin = 34;
constexpr std::uint32_t kSensorSampleIntervalMs = 2500;
constexpr std::uint8_t kRelayPins[] = {26, 32, 33, 25};
constexpr std::size_t kSmartFarmingPumpRelayIndex = 0;

void onSmartFarmingSensorEvent(const core::Event& event) {
#ifdef ARDUINO
    Serial.print("[SMART-FARMING-EVENT] Sensor raw_adc=");
    Serial.println(event.value);
#endif
}
}

static products::SmartFarming smart_farming(
    event_bus, soil_moisture, relay_channels[kSmartFarmingPumpRelayIndex]);

void setup()
{
#ifdef ARDUINO
    Serial.begin(115200);
    delay(500);

    Serial.println();
    Serial.println("============================================================");
    Serial.println(" OpenIoT Platform - Firmware Identity");
    Serial.println("------------------------------------------------------------");
    Serial.print(" Product      : ");
    Serial.println("OpenIoT Smart Farming");
    Serial.print(" Version      : ");
    Serial.print(foundation::kFrameworkVersion.major);
    Serial.print(".");
    Serial.print(foundation::kFrameworkVersion.minor);
    Serial.print(".");
    Serial.println(foundation::kFrameworkVersion.patch);
    Serial.print(" Build        : #");
    Serial.println(OPENIOT_BUILD_NUMBER);
    Serial.print(" Git Commit   : ");
    Serial.println(OPENIOT_GIT_COMMIT);
    Serial.print(" Git State    : ");
    Serial.println(OPENIOT_GIT_STATE);
    Serial.print(" Build Date   : ");
    Serial.println(__DATE__);
    Serial.print(" Build Time   : ");
    Serial.println(__TIME__);
    Serial.print(" Environment  : ");
    Serial.println(OPENIOT_BUILD_ENV);
    Serial.print(" Board        : ");
    Serial.println("ESP32 DevKitC V4 / ESP32 Dev Module");
    Serial.println("============================================================");
    Serial.println("[BOOT-01] setup entered");
    Serial.println("[BOOT-02] before boot.begin()");
#endif

#if OPENIOT_WIFI_SECRETS_AVAILABLE
    const foundation::ErrorCode wifi_config_result =
        wifi.begin(OPENIOT_WIFI_SSID, OPENIOT_WIFI_PASSWORD);
    Serial.print("[NETWORK-WIFI] config ErrorCode=");
    Serial.println(static_cast<unsigned int>(wifi_config_result));
    if (wifi_config_result == foundation::ErrorCode::Ok) {
        const foundation::ErrorCode wifi_connect_result = wifi.connect();
        Serial.print("[NETWORK-WIFI] initial connect result ErrorCode=");
        Serial.println(static_cast<unsigned int>(wifi_connect_result));
        Serial.println("[NETWORK-WIFI] waiting for Wi-Fi; no MQTT broker configured");
    }
#else
    Serial.println("[NETWORK-WIFI] DISABLED: local network_secrets.hpp not found");
    Serial.println("[NETWORK-WIFI] copy network_secrets.example.hpp to network_secrets.hpp and set local Wi-Fi credentials");
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
        const foundation::ErrorCode event_subscribe_result =
            event_bus.subscribe(core::EventType::Sensor, onSmartFarmingSensorEvent);
#ifdef ARDUINO
        Serial.print("[EVENTBUS-SENSOR] subscribe ErrorCode=");
        Serial.println(static_cast<unsigned int>(event_subscribe_result));
#endif
        if (event_subscribe_result != foundation::ErrorCode::Ok) {
            logger.error("Smart Farming EventBus subscription FAILED");
        }

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

    if (relays_safe && soil_result == foundation::ErrorCode::Ok) {
        const foundation::ErrorCode farming_result = smart_farming.begin();
#ifdef ARDUINO
        Serial.print("[SMART-FARMING] begin ErrorCode=");
        Serial.println(static_cast<unsigned int>(farming_result));
#endif
        if (farming_result != foundation::ErrorCode::Ok) {
            logger.error("Smart Farming initialization FAILED");
        }
    } else {
        logger.error("Smart Farming initialization skipped because prerequisites failed");
    }

#ifdef ARDUINO
    Serial.println("[BOOT-LOGGER-01] before logger.info()");
#endif

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

#if OPENIOT_WIFI_SECRETS_AVAILABLE
    wifi.loop();
    static network::ConnectionState last_wifi_state = network::ConnectionState::Error;
    if (wifi.state() != last_wifi_state) {
        last_wifi_state = wifi.state();
        Serial.print("[NETWORK-WIFI] state=");
        switch (last_wifi_state) {
            case network::ConnectionState::Down: Serial.println("DOWN"); break;
            case network::ConnectionState::Connecting: Serial.println("CONNECTING"); break;
            case network::ConnectionState::Connected:
                Serial.print("CONNECTED ip=");
                Serial.println(WiFi.localIP());
                break;
            case network::ConnectionState::Error: Serial.println("ERROR"); break;
        }
    }
#endif

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

    }

    static std::uint32_t last_farming_sample_ms = 0;
    const std::uint32_t farming_now = static_cast<std::uint32_t>(millis());
    if (farming_now - last_farming_sample_ms >= kSensorSampleIntervalMs) {
        last_farming_sample_ms = farming_now;
        const std::uint32_t sample_count_before = smart_farming.sampleCount();
        smart_farming.loop();
        if (smart_farming.sampleCount() == sample_count_before) {
            Serial.println("[SMART-FARMING-FAIL] Soil sample was not accepted");
        }
        Serial.print("[SOIL] raw_adc=");
        Serial.print(smart_farming.lastSoilRaw());
        Serial.print(" threshold=");
        Serial.print(smart_farming.threshold());
        Serial.print(" pump=");
        Serial.println(smart_farming.pumpOn() ? "ON" : "OFF");
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
