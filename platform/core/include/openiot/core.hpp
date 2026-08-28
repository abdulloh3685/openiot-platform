#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <openiot/foundation.hpp>

namespace openiot::core {

enum class EventType : std::uint8_t { BootCompleted, Tick, Sensor, Command, Error, Custom };
struct Event { EventType type; std::uint32_t value{0}; };
using EventHandler = void (*)(const Event&);

class EventBus final : public foundation::IModule {
public:
    static constexpr std::size_t kMaxSubscribers = 16;
    foundation::ErrorCode begin() override;
    void loop() override {}
    void end() override;
    foundation::ErrorCode subscribe(EventType type, EventHandler handler);
    foundation::ErrorCode unsubscribe(EventType type, EventHandler handler);
    foundation::ErrorCode publish(const Event& event);
private:
    struct Subscription { EventType type{}; EventHandler handler{nullptr}; bool used{false}; };
    std::array<Subscription, kMaxSubscribers> subscriptions_{};
    bool initialized_{false};
};

class Logger final : public foundation::IModule {
public:
    foundation::ErrorCode begin() override;
    void loop() override {}
    void end() override { initialized_ = false; }
    void info(const char* message);
    void error(const char* message);
private:
    bool initialized_{false};
};

class Scheduler final : public foundation::IModule {
public:
    static constexpr std::size_t kMaxTasks = 16;
    using Task = void (*)(void*);
    foundation::ErrorCode begin() override;
    void loop() override;
    void end() override;
    foundation::Result<std::uint8_t> registerTask(Task task, void* context, std::uint32_t period_ms);
    foundation::ErrorCode removeTask(std::uint8_t id);
private:
    struct Slot { Task task{nullptr}; void* context{nullptr}; std::uint32_t period{0}; std::uint32_t elapsed{0}; bool used{false}; };
    std::array<Slot, kMaxTasks> tasks_{};
    bool initialized_{false};
};

class Config final : public foundation::IModule {
public:
    static constexpr std::size_t kMaxEntries = 24;
    foundation::ErrorCode begin() override;
    void loop() override {}
    void end() override;
    foundation::ErrorCode set(const char* key, const char* value);
    foundation::Result<const char*> get(const char* key) const;
private:
    struct Entry { char key[32]{}; char value[64]{}; bool used{false}; };
    std::array<Entry, kMaxEntries> entries_{};
    bool initialized_{false};
};

class DeviceManager final : public foundation::IModule {
public:
    foundation::ErrorCode begin() override;
    void loop() override {}
    void end() override { initialized_ = false; }
    const char* id() const { return id_; }
private:
    char id_[32]{"ESP32-001"};
    bool initialized_{false};
};

class BootManager final : public foundation::IModule {
public:
    BootManager(Logger& logger, EventBus& bus, Scheduler& scheduler, Config& config, DeviceManager& device)
        : logger_(logger), bus_(bus), scheduler_(scheduler), config_(config), device_(device) {}
    foundation::ErrorCode begin() override;
    void loop() override;
    void end() override { initialized_ = false; }
private:
    Logger& logger_; EventBus& bus_; Scheduler& scheduler_; Config& config_; DeviceManager& device_; bool initialized_{false};
};

} // namespace openiot::core
