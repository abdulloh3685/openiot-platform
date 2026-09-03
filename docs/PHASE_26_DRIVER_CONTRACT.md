# Phase 26 — Driver Engineering Contract

## Status

**Phase:** 26 — Driver Engineering Completion  
**Source identity:** RE-IMPLEMENTED / NEW SOURCE  
**Scope:** DHT22, DS18B20, Soil Moisture, Relay

## Objective

Establish a deterministic, testable driver foundation that preserves the frozen architecture and does not introduce dynamic allocation into the critical path.

## Contract

- Drivers remain below Services/Core and above HAL/platform-specific access.
- Public APIs use fixed-width integer types and `foundation::ErrorCode` / `foundation::Result`.
- Lifecycle must be explicit: a read/set operation before `begin()` returns `NotInitialized`.
- `begin()` establishes the driver instance as ready and must reset driver-owned state to a deterministic baseline.
- No heap allocation is permitted in the driver critical path.
- Native tests must not require physical hardware.
- Hardware-specific sensor acquisition is isolated behind compile-time platform paths; native builds use deterministic simulation values until physical integration is validated.
- Drivers must not silently claim physical sensor accuracy from native simulation tests.

## Driver contracts

### DHT22

`begin(pin)` configures the sensor endpoint. `read()` returns `Dht22Reading`. Native validation uses a deterministic zero-value reading.

### DS18B20

`begin(pin)` configures the sensor endpoint. `readCelsius()` returns Celsius. Native validation uses a deterministic zero value.

### Soil Moisture

`begin(channel)` configures the ADC channel. `readRaw()` returns the raw ADC value. Native validation uses zero.

### Relay

`begin(pin)` initializes the output in the OFF state. `set(on)` changes the logical relay state. `state()` reports the last commanded state.

## Acceptance boundary

Phase 26 can close the software driver foundation only after source, deterministic native tests, and CI are green. Physical DHT22/DS18B20/soil-sensor/relay behavior remains a separate hardware evidence gate.
