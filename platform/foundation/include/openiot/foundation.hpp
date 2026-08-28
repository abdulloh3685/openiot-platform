#pragma once

#include <cstddef>
#include <cstdint>

namespace openiot::foundation {

enum class ErrorCode : std::uint8_t {
    Ok = 0,
    InvalidArgument,
    NotInitialized,
    AlreadyInitialized,
    Busy,
    CapacityExceeded,
    NotFound,
    InvalidState,
    Timeout,
    HardwareError,
    Unsupported,
    InternalError
};

template <typename T>
class Result {
public:
    constexpr Result(T value) : value_(value), error_(ErrorCode::Ok) {}
    constexpr Result(ErrorCode error) : value_{}, error_(error) {}
    constexpr bool ok() const { return error_ == ErrorCode::Ok; }
    constexpr ErrorCode error() const { return error_; }
    constexpr const T& value() const { return value_; }
private:
    T value_{};
    ErrorCode error_{ErrorCode::InternalError};
};

class IModule {
public:
    virtual ~IModule() = default;
    virtual ErrorCode begin() = 0;
    virtual void loop() = 0;
    virtual void end() = 0;
};

struct Version {
    std::uint16_t major;
    std::uint16_t minor;
    std::uint16_t patch;
};

constexpr Version kFrameworkVersion{0, 1, 0};
constexpr std::size_t kModuleNameMax = 24;

} // namespace openiot::foundation
