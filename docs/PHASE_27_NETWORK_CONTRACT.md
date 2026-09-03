# Phase 27 — Network Engineering Contract

## Status

Engineering contract for the re-implemented OpenIoT Network layer.

## Scope

- WiFi lifecycle and connection state
- MQTT lifecycle, publish/subscribe and reconnect behavior
- bounded JSON encode/decode helpers
- deterministic native validation
- ESP32 PlatformIO build integration

## Architecture

`Application → SDK → Services → Core Runtime → HAL → Platform → Hardware`

The Network layer remains below Services/Core integration and does not change the frozen architecture.

## WiFi Contract

- `begin()` validates SSID/password and timing configuration.
- `connect()` starts an attempt and returns `Busy` when the connection is not yet established.
- `loop()` performs connection-state observation and reconnect scheduling without explicit delay loops.
- `connected()` is true only for `ConnectionState::Connected`.
- `end()` returns the module to `Down` and clears initialization state.

## MQTT Contract

- `begin()` validates broker host, port and reconnect interval.
- `connect()` requires an established WiFi link on ESP32.
- `publish()` and `subscribe()` require initialization and an active MQTT connection.
- `loop()` services MQTT and schedules reconnect attempts.
- Invalid arguments are rejected before transport calls.

The ESP32 implementation uses PubSubClient as the transport adapter. Transport-library internals are external dependencies and are not represented as OpenIoT-owned memory guarantees.

## JSON Contract

The first network JSON surface is intentionally bounded and dependency-free:

- caller supplies output buffer and size;
- encoding returns `CapacityExceeded` on truncation;
- numeric decoding returns `Result<float>`;
- missing keys return `NotFound`;
- malformed numeric values return `InvalidArgument`.

This is a foundation contract, not a complete JSON document model.

## Native Test Contract

Native tests cover:

- lifecycle guards;
- invalid configuration;
- connection state transitions through deterministic simulation;
- MQTT publish/subscribe guards;
- JSON encoding, capacity handling and numeric decoding.

## Hardware Evidence Boundary

CI proves compilation and deterministic native behavior. It does **not** prove:

- real WiFi association;
- broker reachability;
- TLS/certificate behavior;
- RF stability;
- physical ESP32 network recovery;
- long-duration field operation.

Those require hardware/environment evidence and remain separate acceptance gates.

## Provenance

Source identity: **RE-IMPLEMENTED / NEW SOURCE**.

No historical source is claimed by this implementation.

PKG-109: APPROVED / CLOSED.
PKG-110: NOT CREATED.
