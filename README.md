# OpenIoT Platform

Modular, hardware-independent IoT framework with an ESP32-first implementation path.

## Current status

**Re-implementation v0.1.0 — Phase 30 CLOSED / Phase 31 Physical Validation PREPARATION**

This repository is the engineering source of truth for the new implementation line. Historical source recovery was formally closed before this implementation began.

## Architecture

```text
Application
    ↓
SDK
    ↓
Services / Network
    ↓
Core Runtime
    ↓
HAL
    ↓
Platform
    ↓
Hardware
```

The architecture is modular, event-driven, testable and designed to isolate hardware dependencies in HAL. No dynamic allocation is used by the core data structures on the critical path.

## Implemented in v0.1.0

- Foundation types, result/error model and module interface
- Version identity for the new implementation line
- Logger contract
- EventBus with static subscriber storage
- Scheduler with static task storage
- RAM-backed Config
- DeviceManager identity contract
- Portable GPIO / ADC / PWM HAL contract
- PlatformIO ESP32 + native environments
- Core, HAL, Driver, Network, Smart Farming, telemetry, integration and stress smoke validation
- ESP32 PlatformIO build gate

## Explicitly not claimed

The source in this repository is **RE-IMPLEMENTED / NEW SOURCE**. It is not represented as recovered historical source, original OF-106, or original OF-107.

## Roadmap

1. Foundation/Core validation
2. Complete HAL: UART, I2C, SPI, Watchdog
3. Drivers: DHT22, DS18B20, Soil Moisture, Relay
4. Network: Wi-Fi, MQTT, JSON
5. Smart Farming MVP
6. Integration, stress, field validation
7. Release hardening

### Current Gate

Phase 30 — Integration & Stress Validation: **CLOSED / PASS**

Phase 31 — Physical ESP32 / Smart Farming Field Validation: **PREPARATION / HARDWARE EVIDENCE GATE**

Physical validation is not inferred from CI. Real-board, sensor, actuator, Wi-Fi, MQTT, safety, and stability claims require evidence from the physical test setup.

See `docs/PHASE_30_FINAL_STATUS.md`, `docs/PHASE_31_FIELD_VALIDATION_CONTRACT.md`, `docs/PROVENANCE.md`, and `docs/ENGINEERING_DECISIONS.md`.
