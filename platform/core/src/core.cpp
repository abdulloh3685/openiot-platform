#include <openiot/core.hpp>
#include <chrono>
#include <cstring>

#ifdef ARDUINO
#include <Arduino.h>
#endif

namespace openiot::core {

foundation::ErrorCode EventBus::begin() {
    if (initialized_) return foundation::ErrorCode::AlreadyInitialized;
    initialized_ = true;
    return foundation::ErrorCode::Ok;
}

void EventBus::end() {
    initialized_ = false;
    for (auto& s : subscriptions_) {
        s.type = EventType::BootCompleted;
        s.handler = nullptr;
        s.used = false;
    }
}

foundation::ErrorCode EventBus::subscribe(EventType type, EventHandler handler) {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
    if (!handler) return foundation::ErrorCode::InvalidArgument;
    for (const auto& s : subscriptions_) {
        if (s.used && s.type == type && s.handler == handler) return foundation::ErrorCode::Busy;
    }
    for (auto& s : subscriptions_) {
        if (!s.used) {
            s.type = type;
            s.handler = handler;
            s.used = true;
            return foundation::ErrorCode::Ok;
        }
    }
    return foundation::ErrorCode::CapacityExceeded;
}

foundation::ErrorCode EventBus::unsubscribe(EventType type, EventHandler handler) {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
    if (!handler) return foundation::ErrorCode::InvalidArgument;
    for (auto& s : subscriptions_) {
        if (s.used && s.type == type && s.handler == handler) {
            s.type = EventType::BootCompleted;
            s.handler = nullptr;
            s.used = false;
            return foundation::ErrorCode::Ok;
        }
    }
    return foundation::ErrorCode::NotFound;
}

foundation::ErrorCode EventBus::publish(const Event& event) {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;

    // Snapshot handlers so callbacks may safely unsubscribe/subscribe without
    // changing the current publication set.
    std::array<EventHandler, kMaxSubscribers> handlers{};
    std::size_t count = 0;
    for (const auto& s : subscriptions_) {
        if (s.used && s.type == event.type && s.handler) handlers[count++] = s.handler;
    }
    for (std::size_t i = 0; i < count; ++i) handlers[i](event);
    return foundation::ErrorCode::Ok;
}

foundation::ErrorCode Logger::begin() {
    if (initialized_) return foundation::ErrorCode::AlreadyInitialized;
    initialized_ = true;
    return foundation::ErrorCode::Ok;
}

void Logger::info(const char* message) { (void)message; }
void Logger::error(const char* message) { (void)message; }

foundation::ErrorCode Scheduler::begin() {
    if (initialized_) return foundation::ErrorCode::AlreadyInitialized;
    initialized_ = true;
    clock_started_ = false;
    last_tick_ms_ = 0;
    return foundation::ErrorCode::Ok;
}

void Scheduler::loop() {
    if (!initialized_) return;

#ifdef ARDUINO
    const std::uint32_t now = static_cast<std::uint32_t>(millis());
#else
    const auto now_time = std::chrono::steady_clock::now().time_since_epoch();
    const auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now_time).count();
    const std::uint32_t now = static_cast<std::uint32_t>(now_ms);
#endif

    if (!clock_started_) {
        clock_started_ = true;
        last_tick_ms_ = now;
        return;
    }

    const std::uint32_t elapsed = now - last_tick_ms_;
    last_tick_ms_ = now;
    tick(elapsed);
}

void Scheduler::tick(std::uint32_t elapsed_ms) {
    if (!initialized_) return;

    for (auto& t : tasks_) {
        if (!t.used || !t.task) continue;

        if (t.period == 0) {
            const Task task = t.task;
            const auto context = t.context;
            task(context);
            continue;
        }

        const std::uint32_t accumulated = t.elapsed + elapsed_ms;
        t.elapsed = accumulated;
        if (t.elapsed < t.period) continue;

        // Coalesce overdue time rather than executing a burst of callbacks.
        t.elapsed %= t.period;
        const Task task = t.task;
        const auto context = t.context;
        task(context);
    }
}

void Scheduler::end() {
    initialized_ = false;
    clock_started_ = false;
    last_tick_ms_ = 0;
    for (auto& t : tasks_) {
        t.task = nullptr;
        t.context = nullptr;
        t.period = 0;
        t.elapsed = 0;
        t.used = false;
    }
}

foundation::Result<std::uint8_t> Scheduler::registerTask(Task task, void* context, std::uint32_t period_ms) {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
    if (!task) return foundation::ErrorCode::InvalidArgument;
    for (std::uint8_t i = 0; i < tasks_.size(); ++i) {
        if (!tasks_[i].used) {
            tasks_[i].task = task;
            tasks_[i].context = context;
            tasks_[i].period = period_ms;
            tasks_[i].elapsed = 0;
            tasks_[i].used = true;
            return i;
        }
    }
    return foundation::ErrorCode::CapacityExceeded;
}

foundation::ErrorCode Scheduler::removeTask(std::uint8_t id) {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
    if (id >= tasks_.size() || !tasks_[id].used) return foundation::ErrorCode::NotFound;
    tasks_[id].task = nullptr;
    tasks_[id].context = nullptr;
    tasks_[id].period = 0;
    tasks_[id].elapsed = 0;
    tasks_[id].used = false;
    return foundation::ErrorCode::Ok;
}

foundation::ErrorCode Config::begin() {
    if (initialized_) return foundation::ErrorCode::AlreadyInitialized;
    initialized_ = true;
    return foundation::ErrorCode::Ok;
}

void Config::end() {
    initialized_ = false;
    for (auto& e : entries_) {
        e.key[0] = '\0';
        e.value[0] = '\0';
        e.used = false;
    }
}

foundation::ErrorCode Config::set(const char* key, const char* value) {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
    if (!key || !value || std::strlen(key) >= sizeof(Entry::key) || std::strlen(value) >= sizeof(Entry::value)) return foundation::ErrorCode::InvalidArgument;
    for (auto& e : entries_) if (e.used && std::strcmp(e.key, key) == 0) {
        std::strncpy(e.value, value, sizeof(e.value) - 1);
        e.value[sizeof(e.value) - 1] = '\0';
        return foundation::ErrorCode::Ok;
    }
    for (auto& e : entries_) if (!e.used) {
        std::strncpy(e.key, key, sizeof(e.key) - 1);
        std::strncpy(e.value, value, sizeof(e.value) - 1);
        e.key[sizeof(e.key) - 1] = '\0';
        e.value[sizeof(e.value) - 1] = '\0';
        e.used = true;
        return foundation::ErrorCode::Ok;
    }
    return foundation::ErrorCode::CapacityExceeded;
}

foundation::Result<const char*> Config::get(const char* key) const {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
    if (!key) return foundation::ErrorCode::InvalidArgument;
    for (const auto& e : entries_) if (e.used && std::strcmp(e.key, key) == 0) return e.value;
    return foundation::ErrorCode::NotFound;
}

foundation::ErrorCode DeviceManager::begin() {
    if (initialized_) return foundation::ErrorCode::AlreadyInitialized;
    initialized_ = true;
    return foundation::ErrorCode::Ok;
}

foundation::ErrorCode BootManager::begin() {
    if (initialized_) return foundation::ErrorCode::AlreadyInitialized;

    if (logger_.begin() != foundation::ErrorCode::Ok) return foundation::ErrorCode::InternalError;
    if (bus_.begin() != foundation::ErrorCode::Ok) { logger_.end(); return foundation::ErrorCode::InternalError; }
    if (scheduler_.begin() != foundation::ErrorCode::Ok) { bus_.end(); logger_.end(); return foundation::ErrorCode::InternalError; }
    if (config_.begin() != foundation::ErrorCode::Ok) { scheduler_.end(); bus_.end(); logger_.end(); return foundation::ErrorCode::InternalError; }
    if (device_.begin() != foundation::ErrorCode::Ok) { config_.end(); scheduler_.end(); bus_.end(); logger_.end(); return foundation::ErrorCode::InternalError; }

    initialized_ = true;
    Event boot_event{};
    boot_event.type = EventType::BootCompleted;
    boot_event.value = 0;
    if (bus_.publish(boot_event) != foundation::ErrorCode::Ok) {
        end();
        return foundation::ErrorCode::InternalError;
    }
    return foundation::ErrorCode::Ok;
}

void BootManager::loop() { if (initialized_) scheduler_.loop(); }

void BootManager::end() {
    if (!initialized_) return;
    device_.end();
    config_.end();
    scheduler_.end();
    bus_.end();
    logger_.end();
    initialized_ = false;
}

} // namespace openiot::core
