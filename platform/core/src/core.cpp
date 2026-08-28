#include <openiot/core.hpp>
#include <cstring>

namespace openiot::core {

foundation::ErrorCode EventBus::begin() { initialized_ = true; return foundation::ErrorCode::Ok; }
void EventBus::end() { initialized_ = false; for (auto& s : subscriptions_) s = {}; }
foundation::ErrorCode EventBus::subscribe(EventType type, EventHandler handler) {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
    if (!handler) return foundation::ErrorCode::InvalidArgument;
    for (auto& s : subscriptions_) if (s.used && s.type == type && s.handler == handler) return foundation::ErrorCode::Busy;
    for (auto& s : subscriptions_) if (!s.used) { s = {type, handler, true}; return foundation::ErrorCode::Ok; }
    return foundation::ErrorCode::CapacityExceeded;
}
foundation::ErrorCode EventBus::unsubscribe(EventType type, EventHandler handler) {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
    for (auto& s : subscriptions_) if (s.used && s.type == type && s.handler == handler) { s = {}; return foundation::ErrorCode::Ok; }
    return foundation::ErrorCode::NotFound;
}
foundation::ErrorCode EventBus::publish(const Event& event) {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
    for (const auto& s : subscriptions_) if (s.used && s.type == event.type && s.handler) s.handler(event);
    return foundation::ErrorCode::Ok;
}

foundation::ErrorCode Logger::begin() { initialized_ = true; return foundation::ErrorCode::Ok; }
void Logger::info(const char* message) { (void)message; }
void Logger::error(const char* message) { (void)message; }

foundation::ErrorCode Scheduler::begin() { initialized_ = true; return foundation::ErrorCode::Ok; }
void Scheduler::loop() {
    if (!initialized_) return;
    for (auto& t : tasks_) if (t.used && t.task && t.period == 0) t.task(t.context);
}
void Scheduler::end() { initialized_ = false; for (auto& t : tasks_) t = {}; }
foundation::Result<std::uint8_t> Scheduler::registerTask(Task task, void* context, std::uint32_t period_ms) {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
    if (!task) return foundation::ErrorCode::InvalidArgument;
    for (std::uint8_t i = 0; i < tasks_.size(); ++i) if (!tasks_[i].used) { tasks_[i] = {task, context, period_ms, 0, true}; return i; }
    return foundation::ErrorCode::CapacityExceeded;
}
foundation::ErrorCode Scheduler::removeTask(std::uint8_t id) {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
    if (id >= tasks_.size() || !tasks_[id].used) return foundation::ErrorCode::NotFound;
    tasks_[id] = {}; return foundation::ErrorCode::Ok;
}

foundation::ErrorCode Config::begin() { initialized_ = true; return foundation::ErrorCode::Ok; }
void Config::end() { initialized_ = false; for (auto& e : entries_) e = {}; }
foundation::ErrorCode Config::set(const char* key, const char* value) {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
    if (!key || !value || std::strlen(key) >= sizeof(Entry::key) || std::strlen(value) >= sizeof(Entry::value)) return foundation::ErrorCode::InvalidArgument;
    for (auto& e : entries_) if (e.used && std::strcmp(e.key, key) == 0) { std::strncpy(e.value, value, sizeof(e.value)-1); return foundation::ErrorCode::Ok; }
    for (auto& e : entries_) if (!e.used) { std::strncpy(e.key, key, sizeof(e.key)-1); std::strncpy(e.value, value, sizeof(e.value)-1); e.used = true; return foundation::ErrorCode::Ok; }
    return foundation::ErrorCode::CapacityExceeded;
}
foundation::Result<const char*> Config::get(const char* key) const {
    if (!initialized_) return foundation::ErrorCode::NotInitialized;
    if (!key) return foundation::ErrorCode::InvalidArgument;
    for (const auto& e : entries_) if (e.used && std::strcmp(e.key, key) == 0) return e.value;
    return foundation::ErrorCode::NotFound;
}

foundation::ErrorCode DeviceManager::begin() { initialized_ = true; return foundation::ErrorCode::Ok; }
foundation::ErrorCode BootManager::begin() {
    if (initialized_) return foundation::ErrorCode::AlreadyInitialized;
    if (logger_.begin() != foundation::ErrorCode::Ok) return foundation::ErrorCode::InternalError;
    if (bus_.begin() != foundation::ErrorCode::Ok) return foundation::ErrorCode::InternalError;
    if (scheduler_.begin() != foundation::ErrorCode::Ok) return foundation::ErrorCode::InternalError;
    if (config_.begin() != foundation::ErrorCode::Ok) return foundation::ErrorCode::InternalError;
    if (device_.begin() != foundation::ErrorCode::Ok) return foundation::ErrorCode::InternalError;
    initialized_ = true;
    bus_.publish({EventType::BootCompleted, 0});
    return foundation::ErrorCode::Ok;
}
void BootManager::loop() { if (initialized_) scheduler_.loop(); }

} // namespace openiot::core
