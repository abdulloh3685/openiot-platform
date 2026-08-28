#include <unity.h>
#include <openiot/core.hpp>
#include <openiot/hal.hpp>

using namespace openiot;

void test_event_bus_publish(void) {
    core::EventBus bus;
    TEST_ASSERT_EQUAL_UINT8(static_cast<std::uint8_t>(foundation::ErrorCode::Ok),
                            static_cast<std::uint8_t>(bus.begin()));

    static int calls = 0;
    auto handler = [](const core::Event&) { ++calls; };

    TEST_ASSERT_EQUAL_UINT8(static_cast<std::uint8_t>(foundation::ErrorCode::Ok),
                            static_cast<std::uint8_t>(bus.subscribe(core::EventType::BootCompleted, handler)));
    TEST_ASSERT_EQUAL_UINT8(static_cast<std::uint8_t>(foundation::ErrorCode::Ok),
                            static_cast<std::uint8_t>(bus.publish({core::EventType::BootCompleted, 1})));
    TEST_ASSERT_EQUAL_INT(1, calls);
}

void test_config_round_trip(void) {
    core::Config config;
    TEST_ASSERT_EQUAL_UINT8(static_cast<std::uint8_t>(foundation::ErrorCode::Ok),
                            static_cast<std::uint8_t>(config.begin()));
    TEST_ASSERT_EQUAL_UINT8(static_cast<std::uint8_t>(foundation::ErrorCode::Ok),
                            static_cast<std::uint8_t>(config.set("device.name", "ESP32-001")));
    const auto result = config.get("device.name");
    TEST_ASSERT_TRUE(result.ok());
    TEST_ASSERT_EQUAL_STRING("ESP32-001", result.value());
}

void test_device_manager(void) {
    core::DeviceManager device;
    TEST_ASSERT_EQUAL_UINT8(static_cast<std::uint8_t>(foundation::ErrorCode::Ok),
                            static_cast<std::uint8_t>(device.begin()));
    TEST_ASSERT_EQUAL_STRING("ESP32-001", device.id());
}

void test_hal_foundation(void) {
    hal::Gpio gpio;
    hal::Adc adc;
    hal::Pwm pwm;
    TEST_ASSERT_EQUAL_UINT8(static_cast<std::uint8_t>(foundation::ErrorCode::Ok),
                            static_cast<std::uint8_t>(gpio.begin()));
    TEST_ASSERT_EQUAL_UINT8(static_cast<std::uint8_t>(foundation::ErrorCode::Ok),
                            static_cast<std::uint8_t>(adc.begin()));
    TEST_ASSERT_EQUAL_UINT8(static_cast<std::uint8_t>(foundation::ErrorCode::Ok),
                            static_cast<std::uint8_t>(pwm.begin()));
}

void setup(void) {
    UNITY_BEGIN();
    RUN_TEST(test_event_bus_publish);
    RUN_TEST(test_config_round_trip);
    RUN_TEST(test_device_manager);
    RUN_TEST(test_hal_foundation);
    UNITY_END();
}

void loop(void) {}
