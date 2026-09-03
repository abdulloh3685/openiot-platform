# OpenIoT Platform — Phase 25 HAL Contract

## Scope

Phase 25 completes the current RE-IMPLEMENTED / NEW SOURCE HAL baseline for ESP32 Arduino and native test environments.

## HAL modules

- `Gpio`
- `Adc`
- `Pwm`

## Contract

### GPIO

- `setOutput(pin, high)` configures the pin as output and writes the requested level.
- `read(pin)` reads the current digital level.
- ESP32 pin validation rejects GPIO numbers outside 0..39.
- Input-only GPIO 34..39 are rejected for output operations.

### ADC

- `read(channel)` treats `channel` as the Arduino/ESP32 ADC-capable pin identifier.
- Invalid ESP32 pin identifiers return `InvalidArgument`.
- Native builds provide deterministic zero-valued ADC simulation.

### PWM

- `set(channel, duty)` uses the existing API without changing the public signature.
- `channel` is interpreted as the PWM-capable GPIO identifier in the Arduino target.
- Duty range is 0..255, matching the Arduino ESP32 `analogWrite` baseline used by this project.
- Values above 255 are rejected rather than silently truncated.
- Native builds accept deterministic simulated duty state.

## Engineering constraints

- No dynamic allocation is introduced by HAL operations.
- Lifecycle remains compatible with `foundation::IModule`.
- Existing public API direction is preserved.
- Hardware calls are isolated behind `#ifdef ARDUINO`.
- Native CI remains deterministic and hardware-independent.

## Provenance

All Phase 25 implementation is **RE-IMPLEMENTED / NEW SOURCE**. No historical source is claimed as recovered.
