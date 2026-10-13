# Phase 31 — Wi-Fi-First Physical Validation

## Scope

This step validates Wi-Fi using the existing `platform/network` module and its `openiot::network::WiFi` API, and adds a lightweight board-uptime diagnostic to Serial Monitor. MQTT broker configuration, MQTT credentials, TLS, and telemetry are intentionally out of scope until Wi-Fi has passed.

## Security

- Never put Wi-Fi SSID/password in tracked source files or commit history.
- Never paste passwords into chat.
- The tracked `network_secrets.example.hpp` contains placeholders only.
- The real `network_secrets.hpp` is ignored by Git.

## Local setup (Windows / VS Code)

1. In the repository, copy:
   `platform/network/include/openiot/network_secrets.example.hpp`
   to:
   `platform/network/include/openiot/network_secrets.hpp`
2. Edit only the local `network_secrets.hpp` and set `OPENIOT_WIFI_SSID` and `OPENIOT_WIFI_PASSWORD`.
3. Use a 2.4 GHz Wi-Fi network and ensure its credentials are correct.
4. Build and upload the existing `esp32dev` environment, then open Serial Monitor at 115200 baud.
5. Confirm the log contains `[NETWORK-WIFI] state=CONNECTED`, followed by `connected_ssid=...` and `rssi_dbm=...`.
6. Confirm `[BOARD-STATUS] uptime=...` appears immediately after startup and then approximately once per minute. The uptime counts from the current boot and resets after a reboot or power cycle.

6. Leave the device running for at least 5 minutes. Record any DOWN / CONNECTING / ERROR / CONNECTED transitions.
7. For a recovery test, temporarily turn off the Wi-Fi access point for 30–60 seconds, turn it back on, and observe whether the log returns to CONNECTED. Do not disconnect the ESP32 hardware or change relay wiring for this test.

## Expected logs

- `[BOARD-STATUS] uptime=2d 5h 32m 18s`: time elapsed since this boot, reported approximately once per minute. The implementation extends the 32-bit `millis()` delta into a 64-bit counter so the displayed days can continue across the usual `millis()` wrap.
- `[NETWORK-WIFI] config ErrorCode=0`: local Wi-Fi configuration accepted.
- `[NETWORK-WIFI] initial connect result ErrorCode=...`: the initial connection attempt result; a nonzero Busy result may occur while joining.
- `[NETWORK-WIFI] state=CONNECTING`: association in progress.
- `[NETWORK-WIFI] state=CONNECTED`: Wi-Fi connected.
- `[NETWORK-WIFI] connected_ssid=...`: SSID returned by the ESP32 Wi-Fi interface after connection; it is diagnostic output, not a password.
- `[NETWORK-WIFI] rssi_dbm=...`: received signal strength in dBm at the moment the connection-state transition is logged.
- `[NETWORK-WIFI] state=DOWN` or `ERROR`: disconnected or connection attempt timed out; observe subsequent retry.

If the log says `DISABLED: local network_secrets.hpp not found`, create the local file from the example. Do not rename or commit the actual credentials file.

## Validation record — 2026-10-12

### Build

- PlatformIO environment: `esp32dev`.
- Result: PASS, as reported from the user's local PlatformIO build.
- RAM: 24,776 / 327,680 bytes (7.6%).
- Flash: 285,557 / 1,310,720 bytes (21.8%).

### N1 — Wi-Fi connection and recovery

- Initial connection to Wi-Fi was confirmed on the physical ESP32.
- The user confirmed that the Router / Access Point was deliberately switched off and then switched back on for this recovery test.
- Reported AP-off duration: less than 30 seconds. This is shorter than the guide's recommended 30–60 second test window; repeat for 30–60 seconds to complete the prescribed duration check.
- In the submitted Serial Monitor log, `ERROR` appears at lines 262 and 324, `CONNECTING` at lines 281 and 349, and `CONNECTED` again at line 386.
- Sensor and soil/pump status logging continued in the captured output; the shown soil readings were below the configured threshold of 2600 and the reported pump state remained `OFF`.
- Assessment: recovery after an actual AP interruption is observed. The shorter-than-prescribed interruption is recorded as a test deviation, not silently treated as a full-duration test.

### N1 status

**FUNCTIONAL RECOVERY: PASS (observed). DURATION CHECK: INCOMPLETE.**

Repeat the controlled AP-off test for 30–60 seconds and capture the log from before the AP is switched off until after `CONNECTED` returns. Also complete the five-minute connected stability observation. SSID is now printed locally for diagnosis; redact it from any logs shared publicly if it identifies a private network. Never share passwords, tokens, or other secrets.

## Evidence to report

- Build result and firmware identity (build number, commit, environment).
- Whether CONNECTED appeared.
- Stability duration.
- Result and duration of the access-point-off/on recovery test.
- Relevant log lines, with SSID/password redacted.

## Acceptance

N1 functional connection/recovery has been observed, but the prescribed recovery-duration and five-minute stability checks are not yet documented as complete. Keep the overall N1 acceptance **IN PROGRESS** until those remaining checks are completed. Phase 31 overall remains **OPEN**; this test does not claim MQTT or end-to-end telemetry success.
