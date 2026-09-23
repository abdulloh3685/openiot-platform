# Phase 31 — Smart Farming Hardware Validation Report

## Status

**HARDWARE WIRING VERIFIED / SENSOR SAMPLING VERIFIED / RELAY FUNCTION VERIFIED**

This report records the physical Smart Farming prototype wiring and the hardware operation verified during the Phase 31 field test.

> This report records only evidence actually observed/reported during the current hardware test. Items requiring network tests, calibrated moisture percentage, functional threshold control, or extended stability evidence remain open unless separately recorded.

## 1. Platform Baseline

- Repository: `abdulloh3685/openiot-platform`
- Target board: ESP32 DevKitC V4 / ESP32 Dev Module
- Framework: Arduino
- Build system: PlatformIO
- Product: OpenIoT Smart Farming MVP
- Phase: 31 — Physical ESP32 / Smart Farming Field Validation

## 2. Hardware Wiring

### DHT22 — 3-pin module

| DHT22 pin | Connection |
|---|---|
| VCC | ESP32 3V3 |
| DATA | GPIO 4 |
| GND | ESP32 GND |

The tested DHT22 is a three-pin module. No external pull-up resistor is added.

### Soil-Moisture Sensor

| Sensor pin | Connection |
|---|---|
| VCC | ESP32 3V3 |
| GND | ESP32 GND |
| AO | GPIO 34 |
| DO | Not connected |

GPIO 34 is used as the analog input for the soil-moisture sensor.

### Four-Channel Relay Module

| Relay pin | Connection |
|---|---|
| VCC | Regulated 5 V relay supply |
| GND | Relay-supply GND and ESP32 GND |
| IN1 | GPIO 26 |
| IN2 | GPIO 32 |
| IN3 | GPIO 33 |
| IN4 | GPIO 25 |

## 3. Relay Polarity

The four relay channels were confirmed as **Active-Low**.

| GPIO state | Relay state |
|---|---|
| HIGH | OFF |
| LOW | ON |

The firmware uses safe startup behavior so that relay outputs are driven HIGH during initialization, keeping all relay channels OFF at startup.

## 4. Physical Validation Result

### Wiring

- [PASS] DHT22 DATA connected to GPIO 4.
- [PASS] Soil Moisture AO connected to GPIO 34.
- [PASS] Relay CH1 connected to GPIO 26.
- [PASS] Relay CH2 connected to GPIO 32.
- [PASS] Relay CH3 connected to GPIO 33.
- [PASS] Relay CH4 connected to GPIO 25.
- [PASS] Relay supply uses regulated 5 V.
- [PASS] Relay ground and ESP32 ground are common.

### Sensor Sampling

- [PASS] DHT22 initialized successfully with ErrorCode=0.
- [PASS] Soil-moisture ADC initialized successfully with ErrorCode=0.
- [PASS] Repeated physical DHT22 readings were captured in the uploaded serial log.
- [PASS] Observed DHT22 values were approximately 24.2–24.3 °C and 63.4–63.6 %RH.
- [PASS] No DHT22 failure record was observed in the uploaded serial log.
- [PASS] Repeated soil ADC readings were captured on GPIO 34.
- [PASS] Observed soil raw ADC values were approximately 2990–3170 in the uploaded serial log.

> The soil ADC result is recorded as raw ADC evidence only. A calibrated moisture percentage is not inferred from these values.

### Relay Operation

- [PASS] Relay module operates on the physical ESP32 setup.
- [PASS] Four-channel relay wiring is operational.
- [PASS] Active-Low polarity is confirmed.
- [PASS] Safe startup configuration is implemented.

### Firmware Build

- [PASS] Firmware build completed successfully.
- Reported build resource usage:
  - RAM: 7.6%
  - Flash: 21.7%

## 5. Evidence

The uploaded serial log records successful boot/HAL/sensor/relay initialization and repeated DHT22 and soil ADC samples. Physical hardware was connected and tested on the ESP32 prototype, and the user confirmed that the connected relay hardware operates correctly.

The working firmware source used for this validation has now been identified as the source of truth for the physical DHT22, soil ADC, and active-low relay behavior being migrated into the repository.

## 6. Validation Boundary

The existing Phase 31 validation contract covers additional physical tests including:

- DS18B20 validation.
- Soil-moisture calibration and moisture-percentage validation.
- Smart Farming threshold/control behavior.
- EventBus sensor path.
- Wi-Fi association.
- MQTT connectivity and telemetry.
- Network-loss recovery.
- Repeated boot/restart and stability testing.

Those items are **not marked PASS by this report unless their corresponding measurements, logs, or test records are captured separately**.

## 7. Safety

Relay testing is performed with the prototype hardware. Mains-voltage loads must not be connected unless an appropriate isolated and electrically safe test environment is used.

## 8. Architecture Compliance

No architecture change is introduced by this validation.

The frozen architecture remains:

`Application → SDK → Services / Network → Core Runtime → HAL → Platform → Hardware`

The current hardware wiring is treated as the Smart Farming prototype wiring reference.

## 9. Result

**Phase 31 Hardware Wiring / Sensor Sampling / Relay Validation: PASS**

**Overall Phase 31 Contract: OPEN until the remaining required physical evidence is collected and recorded.**

This distinction is intentional: verified hardware behavior is recorded as PASS without converting untested items into a false overall PASS.

## 10. Repository Reconciliation

The physical firmware source used during the validation exposed an important repository-baseline discrepancy: the previous `main` branch contained stub Driver implementations for DHT22, soil ADC, and relay behavior.

The reconciled repository implementation now records:

- DHT22 physical read through `DHT.h` on GPIO 4.
- Soil raw ADC read through `analogRead()` on GPIO 34.
- Active-low relay output with safe OFF startup behavior.
- Physical pin map matching the validated prototype.
- PlatformIO dependency declaration for the DHT sensor library required by the physical DHT22 implementation.

No new architecture, PKG, or Phase is created by this reconciliation.

## 11. Next Action

Continue physical validation from the remaining Phase 31 contract items, prioritizing:

1. Smart Farming control-path validation using the existing threshold contract.
2. Relay safe-state verification during boot/shutdown.
3. EventBus sensor-path evidence.
4. Network/MQTT physical validation when the network test setup is ready.
5. Stability/restart evidence.

No new architecture or PKG is created by this report.
