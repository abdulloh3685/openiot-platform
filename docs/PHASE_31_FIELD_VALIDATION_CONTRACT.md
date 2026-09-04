# Phase 31 — Physical ESP32 / Smart Farming Field Validation Contract

## Status

**PREPARATION / HARDWARE EVIDENCE GATE**

## Objective

Validate the re-implemented OpenIoT Platform on real ESP32 hardware and establish the first physical Smart Farming evidence set after successful deterministic software integration.

## Baseline

- Repository: `abdulloh3685/openiot-platform`
- Software baseline: Phase 30 `main` merge `a9ab214b0b3d5a402d95d18d8e602ff4d7b884ee`
- Target: ESP32 DevKitC V4 / ESP32 Dev Module
- Framework: Arduino
- Build system: PlatformIO
- Product: Smart Farming MVP

## Validation Scope

### 31.1 Hardware Bring-up

- Confirm board identity and firmware boot.
- Confirm serial output at 115200 baud.
- Record firmware/version identity.
- Record free heap at boot and during operation.

### 31.2 HAL Physical Validation

- GPIO output/input behavior.
- ADC reading behavior on the selected soil-sensor input.
- PWM behavior where used by the physical test setup.
- Record pin mapping and observed results.

### 31.3 Driver Physical Validation

- DHT22 read validation.
- DS18B20 read validation.
- Soil Moisture read validation.
- Relay output validation.
- Record wiring, supply voltage, sensor readings, and observed errors.

### 31.4 Smart Farming Functional Validation

- Soil sampling on real hardware.
- Threshold-based pump/relay control.
- Sensor EventBus path observable through firmware logs/telemetry.
- Safe shutdown behavior: relay/pump output must be OFF when the product is stopped or enters the defined safe state.

### 31.5 Network Physical Validation

- Wi-Fi association.
- MQTT broker reachability.
- Publish telemetry from the ESP32 to the configured broker.
- Simulated/real network-loss recovery where the test setup permits.
- TLS/certificate validation only if a TLS-enabled broker is explicitly configured.

### 31.6 Stability Validation

- Repeated boot/restart cycles.
- Observe heap trend and runtime stability.
- Observe sensor/relay/network recovery behavior.
- Record test duration and failures.

## Required Evidence

Every physical test result must include, where applicable:

- date/time,
- board and firmware identity,
- PlatformIO environment,
- wiring/pin map,
- power source,
- sensor/actuator model,
- serial log or measurement record,
- expected result,
- observed result,
- PASS/FAIL,
- anomaly or corrective action.

Photos or screenshots may be attached as supporting evidence, but a photo alone is not sufficient for a functional claim when a measurement or serial log is required.

## Acceptance Boundary

CI evidence proves software/build behavior. Physical evidence is required for claims about real hardware, sensors, actuators, Wi-Fi, MQTT broker connectivity, RF behavior, electrical safety, and field stability.

Phase 31 must not convert a simulated/native result into a physical PASS.

## Safety Boundary

Relay/pump testing must use an appropriate isolated and electrically safe test setup. Do not connect mains-voltage equipment to the prototype unless the test environment is designed and qualified for that purpose.

## Architecture

The frozen architecture remains:

`Application → SDK → Services / Network → Core Runtime → HAL → Platform → Hardware`

No architecture change is authorized by this phase.

## Provenance

`RE-IMPLEMENTED / NEW SOURCE`

No historical-source recovery claim is introduced.

## Governance

- PKG-109: APPROVED / CLOSED
- PKG-110: MUST NOT BE CREATED
- AiPASS: not used

## Gate

Phase 31 is **not PASS by preparation alone**. It becomes CLOSED only after the required physical evidence is collected, reviewed, and recorded. Any unavailable hardware test remains explicitly OPEN rather than being inferred from CI.
