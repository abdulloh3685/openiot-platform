# OpenIoT Platform — New Engineering Decisions

**Decision class:** NEW ENGINEERING DECISION

## NED-001 — New Source Identity
Historical source recovery remains closed. All implementation on this line is new source.

**Status:** APPROVED for implementation lineage

## NED-002 — Version
New implementation line starts at `0.1.0`.

This is an implementation version and is not the historical canonical version.

**Status:** APPROVED

## NED-003 — Device Model
`DeviceManager` owns system/device identity. A separate `Device` implementation is not required for the initial core runtime line.

**Status:** APPROVED for v0.1.0 scope

## NED-004 — EventBus
A new EventBus contract is implemented under the frozen event-driven architecture. It is not identified as historical OF-107.

**Status:** APPROVED

## NED-005 — HAL Foundation Scope
v0.1.0 implements the portable contract surface for GPIO, ADC and PWM. UART, I2C, SPI and Watchdog remain subsequent implementation scope.

**Status:** APPROVED

## NED-006 — Build
PlatformIO environments are explicitly defined for `esp32dev` using Arduino and a native test environment. The recovered historical `platformio.ini` is treated as evidence only.

**Status:** APPROVED

## NED-007 — Test Gate
Core smoke validation is included in the repository. Full hardware, integration, stress and field validation remain release gates beyond the initial source bootstrap.

**Status:** APPROVED
