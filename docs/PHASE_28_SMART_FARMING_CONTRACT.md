# Phase 28 — Smart Farming Product Integration Contract

## Status

Implementation contract for Phase 28.

## Objective

Complete the first product-layer integration of OpenIoT using the existing Core Runtime, Drivers, and deterministic CI smoke validation.

## Scope

1. Smart Farming lifecycle contract.
2. Soil-moisture sampling through the existing driver API.
3. Deterministic relay control using a configurable threshold.
4. Sensor event publication through EventBus.
5. Observable product state for testability and diagnostics.
6. Safe shutdown: relay is commanded OFF during `end()`.
7. Native smoke validation and ESP32 compile validation.

## Non-Goals

- No architecture changes.
- No new dynamic allocation in the product control path.
- No replacement of the existing Driver API.
- No claim of physical sensor/relay behavior from native CI.
- No claim of real WiFi/MQTT/cloud connectivity.
- No PKG-110.

## Runtime Contract

- `begin()` is idempotence-guarded and returns `AlreadyInitialized` when already active.
- `loop()` is a cooperative, non-blocking control step.
- A successful soil reading updates product state and increments the sample counter.
- Pump request is ON when `soil_raw < threshold`; otherwise OFF.
- Relay failure prevents the product from publishing the corresponding sensor event.
- Each successful sample publishes `EventType::Sensor` with the raw soil value.
- `end()` commands the relay OFF and clears the initialized state.

## Architecture Boundary

`Application → SDK → Services → Core Runtime → HAL → Platform → Hardware`

Smart Farming remains a product-layer consumer of Core Runtime and Driver contracts; it does not modify the frozen architecture.

## Provenance

All implementation is **RE-IMPLEMENTED / NEW SOURCE**. No historical source claim is made.

## Acceptance Evidence

Required before closure:

- Smart Farming native smoke test passes.
- Existing Core/HAL/Driver/Network smoke tests continue to pass.
- ESP32 PlatformIO build passes.
- PR validation passes.
- Post-merge `main` CI passes.
