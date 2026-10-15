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
7. Leave the device running for at least 5 minutes. Record any DOWN / CONNECTING / ERROR / CONNECTED transitions.
8. For a recovery test, temporarily turn off the Wi-Fi access point for 30–60 seconds, turn it back on, and observe whether the log returns to CONNECTED. Do not disconnect the ESP32 hardware or change relay wiring for this test.

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

## Follow-up evidence — 2026-10-15 (captured Serial Monitor log)

- The user confirmed that the AP was physically switched off for the interruption interval and then switched back on.
- The captured log shows the Wi-Fi transition to `CONNECTING` at approximately 14:30:42, repeated `ERROR` / retry states, and successful `CONNECTED` at approximately 14:32:25. The 1 minute 43 second interval is the observed recovery window; it is **not** treated as the AP-off duration.
- The user separately confirmed that the AP-off duration was within the intended 30–60 second window.
- After reconnection, the log continues through approximately 14:43:53 without another Wi-Fi state transition. This provides more than five minutes of observed post-reconnect connected stability (approximately 11 minutes 28 seconds from the reconnection log entry to the end of capture).
- DHT22 and soil/pump logging continued through the disconnect/retry/reconnect sequence. At the end of the capture, the displayed soil reading remained below threshold 2600 and the pump was `OFF`.
- Evidence file: `device-monitor-261015-142926.log` (local test capture). Avoid publishing private network identifiers; never record passwords or tokens.

### N1 status

**FUNCTIONAL RECOVERY: PASS. AP-OFF DURATION: PASS (user-confirmed 30–60 seconds). POST-RECONNECT STABILITY: PASS (>5 minutes observed in the log).**

Based on the captured transitions, continued sensor logging, the user's confirmation of the AP-off duration, and the sustained post-reconnect log, the prescribed N1 physical validation checks are now documented as complete. Phase 31 overall remains **OPEN**; this result does not claim MQTT or end-to-end telemetry success.

## Evidence to report

- Build result and firmware identity (build number, commit, environment).
- Whether CONNECTED appeared.
- Stability duration.
- Result and duration of the access-point-off/on recovery test.
- Relevant log lines, with SSID/password redacted.

## Acceptance

N1 functional connection/recovery has been observed and is **PASS for functional behavior**. The prescribed recovery-duration and five-minute connected-stability checks are not yet documented as complete. Keep formal N1 acceptance **IN PROGRESS** until those remaining checks are completed. Phase 31 overall remains **OPEN**; this test does not claim MQTT or end-to-end telemetry success.
