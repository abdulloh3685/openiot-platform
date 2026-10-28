# MQTT TLS Configuration Proposal

**Status:** Proposed — requires review before implementation  
**Scope:** OpenIoT Network layer, ESP32/Arduino + PlatformIO first  
**Related contract:** `docs/PHASE_27_NETWORK_CONTRACT.md`  
**Decision boundary:** This document proposes an implementation path; it does not change frozen architecture, API direction, or existing source code.

## 1. Purpose

Enable the existing MQTT client to connect to a broker that requires TLS and username/password authentication, such as HiveMQ Cloud on port 8883, while preserving current non-TLS behavior and native-test determinism.

The current adapter in `platform/network/src/network.cpp` constructs `PubSubClient` over `WiFiClient`, calls `setServer(host, port)`, and connects with a fixed client ID without credentials. The current `MqttConfig` has host, port, and reconnect interval only. Therefore the current implementation does not establish TLS or authenticate with broker credentials.

## 2. Constraints and invariants

- Preserve the frozen layered architecture. Network stays in its current layer; no application-to-HAL or other dependency inversion changes.
- Preserve existing entry points where practical, especially `Mqtt::begin(const char*, std::uint16_t)` and `Mqtt::begin(const MqttConfig&)`.
- Keep native tests deterministic and independent of Wi-Fi, certificates, and a live broker.
- Do not put passwords, private keys, or environment-specific credentials in source control, tests, logs, or build reports.
- Never silently use an insecure TLS mode in production. Certificate verification must be the default for TLS.
- Keep reconnect behavior bounded and compatible with the current reconnect interval contract.
- Do not claim hardware validation based on native tests or HiveMQ Web Client tests.

## 3. Proposed configuration surface

Extend `MqttConfig` additively after an API-direction review. Proposed fields:

| Field | Purpose | Rule |
|---|---|---|
| `host`, `port`, `reconnect_interval_ms` | Existing connection settings | Preserve current semantics |
| `client_id` | Broker client identity | Optional; use the existing default only when omitted |
| `username` | MQTT authentication username | Optional |
| `password` | MQTT authentication password | Optional; never log |
| `tls_enabled` | Select TLS transport | Default false for backward compatibility |
| `ca_certificate_pem` | Broker CA trust anchor in PEM form | Required when TLS is enabled, unless an explicitly reviewed platform trust-store strategy is adopted |

This is a proposal, not a finalized API. The team should confirm pointer lifetime/ownership rules for strings and PEM data before coding. Configuration values must remain valid for as long as the client uses them, or be copied into bounded owned storage if that matches existing project memory rules.

### Compatibility behavior

- Existing calls to `begin(host, port)` retain current non-TLS behavior.
- Existing `MqttConfig` initializations that omit new fields continue to work because new fields have safe defaults.
- TLS mode with a missing/invalid trust configuration fails closed with an appropriate error; it must not silently fall back to plaintext.
- A non-empty username with an empty password, or other invalid credential combinations, must be handled explicitly and tested.

## 4. Transport design

Use the ESP32 Arduino TLS-capable client (`WiFiClientSecure`) under the existing `PubSubClient` adapter when `tls_enabled` is true, and retain `WiFiClient` for the legacy non-TLS path.

Before implementation, verify the exact `PubSubClient` constructor and lifetime requirements for the pinned dependency version. Both the underlying transport and `PubSubClient` must have stable lifetimes; avoid constructing temporary client objects whose lifetime ends while MQTT is using them.

For TLS:
1. Configure the CA trust anchor before attempting a connection.
2. Verify the broker certificate and hostname using the supported ESP32 TLS client behavior.
3. Connect to the configured TLS port (HiveMQ Cloud commonly uses 8883; the configured port remains authoritative).
4. Call the authenticated `PubSubClient::connect(client_id, username, password)` overload when credentials are configured; use the no-credential overload only when explicitly intended.
5. Keep MQTT publish/subscribe/loop/reconnect behavior behind the existing `Mqtt` interface.

Do not use `setInsecure()` as a production workaround. If a temporary diagnostic-only insecure mode is ever considered, it requires a separate explicit decision, must not be the default, and must not be accepted as production-ready validation.

## 5. Error handling and observability

- Validate required fields before changing connection state.
- Report failures through the project's existing `ErrorCode` and logging conventions; do not print secrets or full credential-bearing configuration.
- Distinguish invalid configuration from Wi-Fi unavailable and broker connection failure where the existing error model permits it. Do not invent new error codes without checking the Foundation contract.
- Keep reconnection scheduling governed by `reconnect_interval_ms`; do not create a tight retry loop.
- Ensure `end()` disconnects MQTT and safely resets transport state.

## 6. Test plan

### Native/unit tests

Add tests for:
- Legacy `begin(host, port)` behavior remains valid.
- New configuration defaults preserve non-TLS compatibility.
- TLS enabled without a CA trust anchor is rejected.
- Invalid host/port and invalid credential combinations are rejected deterministically.
- Publish/subscribe guards and reconnect scheduling remain unchanged.
- Credentials and certificate contents are not included in logs.

Native tests should use the existing simulated connection seam and must not require a real broker or certificate parser.

### ESP32 build checks

- Build the existing `esp32dev` environment with the repository-pinned PlatformIO and library versions.
- Verify the correct TLS client headers and API exist for the pinned Arduino-ESP32 framework.
- Confirm RAM/flash reports and ensure no dynamic allocation is introduced into critical-path code beyond what the existing TLS/MQTT libraries require; document unavoidable library allocations.

### Physical integration checks

Using the ESP32 and a dedicated test broker account:
1. Connect to Wi-Fi.
2. Connect to HiveMQ Cloud using TLS certificate verification and username/password stored outside the repository.
3. Publish telemetry to a test topic and confirm receipt in HiveMQ Web Client.
4. Subscribe to a test command topic and confirm delivery.
5. Test wrong password, wrong CA, and unreachable broker; confirm failure without insecure fallback.
6. Interrupt Wi-Fi or broker access, restore it, and verify bounded reconnection.
7. Observe stability over an agreed test interval.

Record firmware commit, board, framework/library versions, broker endpoint (host only; never secrets), topic names, timestamps, result, and relevant serial logs. Redact credentials and sensitive certificate material.

## 7. Acceptance criteria

- Existing non-TLS callers and native tests remain backward compatible.
- TLS connections verify broker certificates and do not fall back to plaintext.
- Username/password authentication works without exposing credentials in repository or logs.
- ESP32 build passes with pinned dependencies.
- Physical publish/subscribe and recovery evidence is recorded separately from native test evidence.
- No frozen architecture or API decision is changed without explicit review.

## 8. Implementation sequence

1. Review and approve the proposed API/configuration and CA trust strategy.
2. Implement the additive configuration and transport selection in the Network adapter.
3. Add/update native tests and run the native test suite.
4. Build the ESP32 target and record the build report.
5. Perform physical HiveMQ TLS integration tests and record evidence.
6. Update the relevant network contract and Phase 31 evidence only after the implementation and tests establish the behavior.

## 9. Open decisions before implementation

- **Trust anchor delivery:** PEM CA certificate supplied by the application/configuration versus a platform-managed trust bundle. Initial recommendation: explicit CA PEM to keep the trust decision visible and reproducible.
- **Configuration ownership:** caller-owned strings with documented lifetime versus bounded owned storage. Choose based on existing memory and configuration conventions.
- **Credential policy:** allow anonymous TLS when username/password are absent, or require credentials for the initial HiveMQ use case. Recommendation: support both TLS-only and TLS + credentials without embedding secrets.
- **Error mapping:** reuse existing error codes where possible; add no code until the Foundation error contract has been checked.

No implementation, API freeze change, or hardware test is implied by this proposal alone.
