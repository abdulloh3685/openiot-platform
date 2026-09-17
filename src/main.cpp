#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <openiot/core.hpp>
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

