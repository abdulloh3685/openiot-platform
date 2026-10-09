# Phase 31 — Wi-Fi-First Physical Validation

## Scope

This step validates Wi-Fi only, using the existing `platform/network` module and its `openiot::network::WiFi` API. MQTT broker configuration, MQTT credentials, TLS, and telemetry are intentionally out of scope until Wi-Fi has passed.

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
5. Confirm the log contains `[NETWORK-WIFI] state=CONNECTED`.
6. Leave the device running for at least 5 minutes. Record any DOWN / CONNECTING / ERROR / CONNECTED transitions.
7. For a recovery test, temporarily turn off the Wi-Fi access point for 30–60 seconds, turn it back on, and observe whether the log returns to CONNECTED. Do not disconnect the ESP32 hardware or change relay wiring for this test.

## Expected logs

- `[NETWORK-WIFI] config ErrorCode=0`: local Wi-Fi configuration accepted.
- `[NETWORK-WIFI] initial connect result ErrorCode=...`: the initial connection attempt result; a nonzero Busy result may occur while joining.
- `[NETWORK-WIFI] state=CONNECTING`: association in progress.
- `[NETWORK-WIFI] state=CONNECTED`: Wi-Fi connected.
- `[NETWORK-WIFI] state=DOWN` or `ERROR`: disconnected or connection attempt timed out; observe subsequent retry.

If the log says `DISABLED: local network_secrets.hpp not found`, create the local file from the example. Do not rename or commit the actual credentials file.

## Evidence to report

- Build result and firmware identity (build number, commit, environment).
- Whether CONNECTED appeared.
- Stability duration.
- Result of access-point-off/on recovery test.
- Any relevant log lines, with SSID/password redacted.

## Acceptance

Wi-Fi N1 is PASS only after the device connects and a controlled disconnection/recovery test is observed and documented. Until then, mark N1 IN PROGRESS. Phase 31 overall remains OPEN; this test does not claim MQTT or end-to-end telemetry success.
