# Phase 30 — Integration & Stress Validation Contract

Status: IMPLEMENTATION

## Objective

Validate the completed Core, Drivers, Network, Smart Farming, and telemetry path as one deterministic native integration surface before physical field validation.

## Scope

- End-to-end Core -> Driver -> Smart Farming -> MQTT telemetry smoke path.
- Scheduler timing under repeated deterministic ticks.
- EventBus delivery under repeated sensor events.
- Network-loss behavior: product sampling continues while telemetry publication stops.
- Repeated module shutdown/startup lifecycle recovery.
- Preserve the frozen architecture and existing public APIs.
- Add the integration/stress smoke test to CI.
- Preserve the ESP32 PlatformIO build gate.

## Acceptance Targets

1. 1,000 cooperative iterations complete without assertion failure.
2. A 100 ms scheduler task fires exactly 10 times over 1,000 one-millisecond ticks.
3. 1,000 Smart Farming sensor events are delivered.
4. 1,000 telemetry publications succeed while MQTT is simulated connected.
5. A simulated network loss does not stop Smart Farming sampling and does not increment telemetry publication count.
6. Ten telemetry/farming/MQTT lifecycle recovery cycles succeed.
7. BootManager can be restarted after clean shutdown.
8. Existing native smoke tests and ESP32 build remain green.

## Evidence Boundary

This phase proves deterministic software integration only. It does not prove:

- physical GPIO/ADC/PWM behavior,
- real WiFi association,
- real MQTT broker reachability,
- TLS/certificate behavior,
- RF stability,
- sensor accuracy,
- pump/relay electrical safety,
- long-duration field operation.

Those require physical hardware evidence supplied separately.

## Architecture

The implementation remains within:

`Application → SDK → Services → Core Runtime → HAL → Platform → Hardware`

No new layer or protocol is introduced.

## Source Identity

`RE-IMPLEMENTED / NEW SOURCE`.

Historical source recovery remains closed/bounded; this phase does not alter provenance.

PKG-109 remains APPROVED/CLOSED. PKG-110 must not be created.
