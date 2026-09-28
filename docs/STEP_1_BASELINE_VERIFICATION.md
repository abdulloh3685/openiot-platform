# Step 1 — Engineering Contract & Repository Baseline Verification

## Status

**COMPLETED — INSPECT / ANALYZE / REPORT**

This document records the Step 1 verification of the current `main` branch of `abdulloh3685/openiot-platform`.

## Verification Baseline

- Branch: `main`
- HEAD: `7871de278dd805d082f85ffc181e08b01a83d3ae`
- HEAD commit: `feat(smart-farming): connect physical soil control to relay CH1`
- Verification date: 2026-09-28
- Repository lineage: **RE-IMPLEMENTED / NEW SOURCE**
- PKG-109: APPROVED / CLOSED
- PKG-110: NOT CREATED

## Frozen Architecture

The repository remains aligned with the frozen layered direction:

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

No architecture change is introduced by Step 1.

## Repository Structure Observed

```text
.github/
docs/
platform/
  foundation/
  core/
  hal/
  drivers/
  network/
products/
  smart-farming/
src/
tests/
platformio.ini
README.md
```

The implementation remains organized around Foundation, Core, HAL, Drivers, Network, Product, Tests, and Documentation. This is consistent with the frozen repository direction.

## Source Verification

### Foundation

- Portable `ErrorCode`, `Result<T>`, `IModule`, and framework version identity are present.
- Core data structures use fixed-size storage.

### Core Runtime

- `EventBus` uses a fixed subscriber array.
- `Scheduler` uses fixed task storage and exposes deterministic `tick()`.
- `Config` uses fixed-size entries.
- `DeviceManager` owns device identity.
- `BootManager` composes the core modules through references.

### HAL

- GPIO, ADC, and PWM contracts are present.
- ESP32-specific behavior is isolated behind `#ifdef ARDUINO`.
- GPIO34 is treated as input-only by the ESP32 output capability check.
- ADC capability includes GPIO34.

### Drivers

- DHT22 physical read is implemented with the Arduino DHT library.
- Soil Moisture physical raw ADC read uses `analogRead()`.
- Relay supports active-low polarity and initializes to OFF.
- DS18B20 remains a stub implementation and is therefore not physically validated by the current source.

### Smart Farming

- Smart Farming reads the soil driver, compares against the existing threshold contract, controls the injected relay, and publishes a Sensor EventBus event.
- Pump control is bound to relay channel 1 in `src/main.cpp`.

### Network

- Wi-Fi, MQTT, and bounded JSON contracts are present.
- Arduino builds use `WiFi.h` and `PubSubClient`.
- Native simulation hooks remain available for connectivity tests.

## Physical Pin Reconciliation

The current `main` source matches the validated Phase 31 prototype wiring:

| Function | Pin |
|---|---:|
| DHT22 DATA | GPIO 4 |
| Soil Moisture AO | GPIO 34 |
| Relay CH1 | GPIO 26 |
| Relay CH2 | GPIO 32 |
| Relay CH3 | GPIO 33 |
| Relay CH4 | GPIO 25 |

The four-channel relay is configured as **Active-Low** and initialized OFF.

## Tests and Validation Evidence

Repository tests are present at Core, HAL, Drivers, Network, Smart Farming, integration, and top-level smoke-test levels.

Phase 30 is recorded as **CLOSED / PASS** for deterministic software integration and stress validation.

Phase 31 records **HARDWARE WIRING VERIFIED / SENSOR SAMPLING VERIFIED / RELAY FUNCTION VERIFIED**, while explicitly keeping remaining physical/network/stability items OPEN.

The current HEAD commit has no associated pull-request workflow run returned by the GitHub workflow association query. Therefore Step 1 does **not** convert the historical Phase 30 CI evidence into a new current-HEAD CI claim.

## Findings

### F1 — Baseline is internally coherent

The current repository structure, implementation layers, physical pin map, and Phase 31 reconciliation report are mutually consistent for the currently implemented Smart Farming path.

### F2 — Physical evidence is bounded correctly

The repository explicitly distinguishes physical evidence from native/CI evidence. This boundary must remain unchanged.

### F3 — Remaining implementation/validation scope is explicit

The following remain outside a completed Phase 31 gate:

- DS18B20 physical validation.
- Soil-moisture calibration / moisture percentage.
- Full threshold-control evidence on the physical setup.
- Safe-state verification during boot/shutdown as a recorded physical test.
- EventBus sensor-path evidence on hardware.
- Wi-Fi association.
- MQTT broker connectivity and telemetry.
- Network-loss recovery.
- Repeated boot/restart and extended stability evidence.

These are existing Phase 31 scope items, not new work packages.

## Step 1 Decision

**STEP 1 BASELINE VERIFICATION: COMPLETE**

No source refactor, API redesign, folder restructuring, architecture change, or PKG-110 creation is required by this verification.

The repository `main` branch is the working baseline for the next authorized engineering step.

## Governance

- Frozen Architecture: PRESERVED
- Frozen Engineering Direction: PRESERVED
- Frozen Coding Direction: PRESERVED
- Frozen Folder Structure: PRESERVED
- Frozen API Direction: PRESERVED
- Historical source recovery: CLOSED
- Provenance: RE-IMPLEMENTED / NEW SOURCE
- PKG-109: APPROVED / CLOSED
- PKG-110: MUST NOT BE CREATED
