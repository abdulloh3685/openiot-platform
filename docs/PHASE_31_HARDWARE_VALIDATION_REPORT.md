# Phase 31 — Smart Farming Hardware Validation Report

## Status

**HARDWARE WIRING VERIFIED / SENSOR SAMPLING VERIFIED / RELAY FUNCTION VERIFIED / CONTROL CALIBRATION BASELINE RECORDED**

This report records the physical Smart Farming prototype wiring, hardware operation, sensor calibration evidence, and the current Smart Farming control baseline verified or derived from the Phase 31 field tests.

> Overall Phase 31 remains OPEN. Network/MQTT, stability, and final end-to-end control evidence are not marked PASS until their corresponding physical measurements and logs are captured.

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
- [PASS] Observed DHT22 values were approximately 24.2–24.3 °C and 63.4–63.6 %RH in the referenced log.
- [PASS] No DHT22 failure record was observed in the referenced serial log.
- [PASS] Repeated soil ADC readings were captured on GPIO 34.
- [PASS] Soil ADC physical sampling is operational.

### Relay Operation

- [PASS] Relay module operates on the physical ESP32 setup.
- [PASS] Four-channel relay wiring is operational.
- [PASS] Active-Low polarity is confirmed.
- [PASS] Safe startup configuration is implemented.
- [PASS] Relay CH1 was confirmed by the user as the Smart Farming pump relay and physically actuates.

### Firmware Build

- [PASS] A firmware build completed successfully before the current control-logic change.
- Previously reported resource usage:
  - RAM: 7.6%
  - Flash: 21.7%
- The current control-logic revision requires a new build before it can be recorded as build-validated.

## 5. Soil Calibration Evidence

The field calibration was performed using real soil states rather than converting raw ADC directly into a universal moisture percentage.

### Previously recorded soil states

| Soil state | Observed raw ADC range | Approx. center |
|---|---:|---:|
| Very dry | 2924–2959 | ~2940 |
| Considered needs watering | 2671–2816 | ~2714 |
| Moist enough | 2384–2493 | ~2465 |
| Very wet | 2169–2208 | ~2188 |

These values establish the observed direction:

**Higher raw ADC = drier soil. Lower raw ADC = wetter soil.**

### Boundary calibration

| Calibration point | Samples | Min | Max | Mean | Median |
|---|---:|---:|---:|---:|---:|
| ~2500 | 23 | 2503 | 2529 | 2515.5 | 2515 |
| ~2550 | 23 | 2511 | 2668 | 2575.3 | 2581 |
| ~2600 | 23 | 2606 | 2635 | 2622.9 | 2624 |
| ~2650 | 23 | 2599 | 2765 | 2692.9 | 2690 |

The ~2500 and ~2600 groups were comparatively tight. The ~2550 and ~2650 groups contained outliers, so averages alone are not used to select the control boundary.

### Sensor-fault evidence

A separate sensor-unpowered test produced raw ADC values approximately **343–379**.

This must not be interpreted as wet soil. It is treated as an invalid/fault reading for the control path.

## 6. Smart Farming Control Baseline

The current implementation uses the following field-calibrated candidate baseline:

- Pump ON threshold: **2600 ADC**
- Pump OFF threshold: **2500 ADC**
- Hysteresis band: **2501–2599 ADC**
- Valid soil operating guard band: **1000–3200 ADC**
- Invalid/out-of-band sensor reading: **Pump OFF**

Control semantics:

```text
raw >= 2600       -> Pump ON
raw <= 2500       -> Pump OFF
2501..2599        -> Keep previous pump state
raw < 1000       -> Sensor invalid -> Pump OFF
raw > 3200       -> Sensor invalid -> Pump OFF
```

The 1000–3200 validity guard is a conservative engineering guard band derived from the observed field data: the recorded sensor-fault values were below 400 ADC, while recorded real-soil values were approximately 2169–2959 ADC. It is a prototype safety boundary, not a universal sensor specification.

The public `SmartFarming::setThreshold()` API is retained. Its default value is now the pump-ON boundary of 2600 ADC. The pump-OFF boundary remains 2500 ADC to provide hysteresis.

## 7. Safety Behavior

The control path is explicitly fail-safe:

```text
Sensor read failure
       |
       v
   Pump OFF

Invalid/out-of-band ADC
       |
       v
   Pump OFF

Normal valid reading
       |
       +--> ADC >= 2600 -> Pump ON
       |
       +--> ADC <= 2500 -> Pump OFF
       |
       +--> 2501..2599 -> Keep previous state
```

This corrects the previously observed unsafe behavior where an unpowered sensor reading around 363 ADC could cause Pump ON under the earlier low-ADC threshold logic.

## 8. EventBus Evidence

- [PASS] EventBus Sensor subscription was observed with ErrorCode=0.
- [PASS] Physical serial evidence showed `[SMART-FARMING-EVENT] Sensor raw_adc=...`.
- Sensor events remain observable for accepted raw readings, including invalid readings, while actuator control applies the fail-safe validity gate.

## 9. Validation Boundary

The existing Phase 31 validation contract still covers:

- DS18B20 validation.
- Final Smart Farming threshold/control physical validation using the current baseline.
- Relay safe-state verification during shutdown/restart.
- Wi-Fi association.
- MQTT connectivity and telemetry.
- Network-loss recovery.
- Repeated boot/restart and stability.

These items are **not marked PASS by this report unless their corresponding measurements, logs, or test records are captured separately**.

## 10. Safety

Relay testing is performed with the prototype hardware. Mains-voltage loads must not be connected unless an appropriate isolated and electrically safe test environment is used.

## 11. Architecture Compliance

No architecture change is introduced by this control revision.

The frozen architecture remains:

`Application → SDK → Services / Network → Core Runtime → HAL → Platform → Hardware`

The current hardware wiring remains the Smart Farming prototype wiring reference.

No new package, phase, or architecture layer is introduced.

## 12. Repository Reconciliation

The reconciled repository implementation records:

- DHT22 physical read through `DHT.h` on GPIO 4.
- Soil raw ADC read through `analogRead()` on GPIO 34.
- Active-low relay output with safe OFF startup behavior.
- Physical pin map matching the validated prototype.
- PlatformIO dependency declaration for the DHT sensor library.
- Smart Farming control using high-ADC=dry semantics.
- Hysteresis between pump ON and pump OFF boundaries.
- Fail-safe Pump OFF for sensor read failures and out-of-band readings.

## 13. Result

**Physical Wiring / Sensor Sampling / Relay Validation: PASS**

**Calibration Evidence: RECORDED**

**Control Baseline: IMPLEMENTED — PHYSICAL RE-VALIDATION REQUIRED**

**Overall Phase 31 Contract: OPEN**

The control revision must be built and then physically validated against real soil states before the Smart Farming control path can be marked PASS.

## 14. Next Action

1. Build the revised firmware.
2. Run native Smart Farming smoke test.
3. Flash ESP32.
4. Verify startup leaves CH1/Pump OFF.
5. Test dry soil: raw >= 2600 -> CH1/Pump ON.
6. Test moist soil: raw <= 2500 -> CH1/Pump OFF.
7. Test hysteresis region 2501–2599 -> state remains stable.
8. Test sensor-unpowered/fault condition -> Pump OFF.
9. Capture serial evidence.
10. Record the physical result and then continue Wi-Fi/MQTT/stability validation.

No new architecture, PKG, or Phase is created by this control revision.
