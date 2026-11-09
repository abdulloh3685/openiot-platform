# OpenIoT Platform — Project Progress

**Last updated:** 2026-11-09  
**Repository:** `abdulloh3685/openiot-platform`  
**Status:** Active development — MQTT over TLS integration and physical verification in progress.

This document records verified implementation and validation evidence. It does not authorize architecture changes, merge decisions, or claims beyond the evidence listed here.

## 1. Overall status

| Area | Status | Evidence / boundary |
|---|---|---|
| Frozen platform architecture and engineering direction | PRESERVED | No redesign or architecture change is included in the current MQTT TLS work. |
| Phase 30 — Integration & Stress Validation | CLOSED / PASS | Native integration and stress checks were included in the successful CI pipeline. |
| Phase 31 — Physical ESP32 / Smart Farming validation | OPEN | Wi-Fi N1 physical validation is accepted; MQTT/TLS and end-to-end telemetry acceptance remain outstanding. |
| MQTT TLS source integration | IMPLEMENTED ON PR BRANCH | PR #17; source path and CA trust-anchor wiring reviewed. |
| Native CI + ESP32 build | PASS | GitHub Actions Run #127, Run ID `42194610263`; both `native` and `esp32` jobs succeeded. |
| Physical HiveMQ Cloud TLS connection | NOT YET VERIFIED | Requires local credentials and real ESP32 test evidence. |
| End-to-end Smart Farming telemetry to broker | NOT YET VERIFIED | Requires successful physical TLS connection and message observation at the broker. |

## 2. Verified completed work

### Phase 31 — Wi-Fi N1 physical validation

The recorded N1 evidence covers:
- Initial Wi-Fi connection.
- Access-point interruption and automatic recovery.
- Reconnection while sensor logging continues.
- Post-reconnect stability observation of approximately 11 minutes 28 seconds, exceeding the five-minute criterion.
- Native and ESP32 CI validation.

The Wi-Fi N1 evidence does **not** establish MQTT broker connectivity, TLS certificate verification, or end-to-end telemetry delivery.

Reference: [PR #11 — Phase 31 Wi-Fi-first physical validation](https://github.com/abdulloh3685/openiot-platform/pull/11).

### MQTT TLS implementation

The following source flow was reviewed on the MQTT TLS work:
1. `platform/network/include/openiot/isrgrootx1.hpp` embeds the ISRG Root X1 PEM trust anchor.
2. `platform/network/include/openiot/network_secrets.example.hpp` includes that header and defines `OPENIOT_MQTT_CA_CERTIFICATE_PEM`.
3. `src/main.cpp` uses the configured macro to populate the MQTT configuration, including TLS enablement and port 8883.
4. `platform/network/src/network.cpp` configures `WiFiClientSecure` with `setCACert(config_.ca_certificate_pem)` when TLS is enabled.
5. The non-TLS transport path remains available for backward compatibility; no `setInsecure()` fallback is used.

Reference: [PR #17 — MQTT TLS configuration for ESP32](https://github.com/abdulloh3685/openiot-platform/pull/17).

**Evidence boundary:** source inspection and CI build success do not prove that the certificate chain, broker credentials, DNS, network path, or TLS handshake work on the physical device.

## 3. CI evidence

Latest recorded successful run:
- Workflow run: **#127**
- Run ID: `42194610263`
- Native job: **SUCCESS** — build and Core, HAL, Driver, Network, Smart Farming, Telemetry, Integration & Stress smoke tests.
- ESP32 job: **SUCCESS** — PlatformIO ESP32 build.
- Runner: Ubuntu 24.04.
- Node.js 20 runtime deprecation notices were warnings; they did not fail the jobs.

[View GitHub Actions run #127](https://github.com/abdulloh3685/openiot-platform/actions/runs/42194610263).

## 4. Current work in progress

- Prepare a local `network_secrets.hpp` by copying `network_secrets.example.hpp`.
- Enter the real Wi-Fi and HiveMQ Cloud settings locally.
- Build and upload firmware to the ESP32 DevKitC V4.
- Observe serial output for Wi-Fi and MQTT connection state.
- Verify broker message delivery on topic `openiot/smart-farming/node1/telemetry`.

The local secrets file is private configuration and must remain untracked/ignored. **Never commit `network_secrets.hpp`, real Wi-Fi credentials, MQTT username/password, or other secrets.** Do not paste credentials into issues, PR comments, logs, or screenshots.

## 5. Physical verification checklist

- [ ] Local `network_secrets.hpp` created from the example file.
- [ ] Local secrets file confirmed ignored by Git.
- [ ] ESP32 firmware build succeeds with local configuration.
- [ ] Firmware uploaded to the physical ESP32 DevKitC V4.
- [ ] Wi-Fi connects on the physical board.
- [ ] MQTT TLS connection to HiveMQ Cloud succeeds on port 8883.
- [ ] Broker accepts the configured credentials.
- [ ] A telemetry message is observed on `openiot/smart-farming/node1/telemetry`.
- [ ] Reconnect behavior is observed after a broker/network interruption.
- [ ] Serial logs and broker-side evidence are recorded without exposing credentials.

Do not mark these items complete without user-provided or otherwise directly observed physical evidence.

## 6. Governance and next gate

- Keep the established architecture, folder structure, API direction, and engineering contracts frozen.
- Do not create unapproved package IDs or invent new work packages; PKG-109 remains closed and PKG-110 must not be created.
- Keep PR #17 unmerged until review and physical verification requirements are satisfied.
- Do not merge any PR or change the frozen architecture without explicit authorization.
- The next acceptance gate is physical MQTT/TLS connection and verified telemetry delivery, followed by review of the evidence.

---
*This status is an evidence-based snapshot as of 2026-11-09. Update it only when new results are actually verified.*
