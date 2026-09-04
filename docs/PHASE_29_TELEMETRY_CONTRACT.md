# Phase 29 — Smart Farming Telemetry Integration Contract

Status: IMPLEMENTED — awaiting CI / PR gate

## Objective

Connect the Smart Farming product state to the existing MQTT/JSON network layer without changing the frozen platform architecture.

## Scope

- Add `SmartFarmingTelemetry` as a product-layer module.
- Publish bounded JSON telemetry through the existing `network::Mqtt` API.
- Expose soil raw value, pump state, and sample count.
- Preserve lifecycle guards and deterministic native behavior.
- Add native telemetry smoke validation to CI.
- Preserve the existing ESP32 build gate.

## Payload Contract

Example:

```json
{"soil_raw":0,"pump_on":true,"samples":1}
```

The payload is stored in a fixed 96-byte buffer. No heap allocation is introduced by the telemetry module.

## Lifecycle Contract

- `begin()` rejects duplicate initialization.
- `begin()` requires Smart Farming and MQTT modules to be initialized.
- `loop()` publishes only when the telemetry module, Smart Farming module, and MQTT transport are ready.
- Failed MQTT publication does not increment the publish counter.
- `end()` clears observable telemetry state.

## Architecture Boundary

The implementation remains within:

`Application → SDK → Services → Core Runtime → HAL → Platform → Hardware`

This phase does not modify the frozen architecture, replace Driver APIs, or introduce a new network protocol.

## Non-Goals

- No claim of physical WiFi association.
- No claim of real MQTT broker reachability.
- No TLS/certificate validation claim.
- No long-duration field validation.
- No Smart Farming cloud/dashboard implementation.
- No dynamic allocation in the telemetry critical path.
- No PKG-110.

## Provenance

Source identity: `RE-IMPLEMENTED / NEW SOURCE`.

Historical source recovery remains unresolved; this implementation must not be labeled as recovered/original source.

## Acceptance Evidence

Required before Phase 29 closure:

1. Native build passes.
2. Existing Core/HAL/Driver/Network/Smart Farming smoke tests pass.
3. Smart Farming telemetry smoke test passes.
4. ESP32 PlatformIO build passes.
5. PR validation passes.
6. PR is merged to `main`.
7. Post-merge CI passes.

PKG-109 remains APPROVED/CLOSED. PKG-110 must not be created.
