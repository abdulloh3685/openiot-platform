# OpenIoT Firmware Identity Standard

## Status

**ACTIVE — Initial implementation**

This standard defines the firmware identity information printed at boot so a physical device can be traced back to the exact repository source used to build it.

## Scope

This is an observability and build-metadata standard. It does not introduce a new architecture layer, package, or runtime dependency.

The frozen architecture remains:

`Application → SDK → Services / Network → Core Runtime → HAL → Platform → Hardware`

## Required boot identity

Every firmware build targeting the physical platform should expose:

- Product name
- Software version
- Build number
- Git commit
- Git working-tree state
- Build date
- Build time
- PlatformIO environment
- Target board

The identity is printed before the normal boot validation log.

## Field definitions

### Product

The firmware/product identity.

Current Smart Farming product:

`OpenIoT Smart Farming`

### Version

The software version from the existing Foundation version identity.

Current repository baseline:

`0.1.0`

The version must not be changed merely because a firmware was rebuilt.

### Build

The current implementation uses the repository commit count as the deterministic build/revision number.

Example:

`Build: #123`

This number is **not an upload counter** and does not mean that the device has been flashed exactly 123 times. It identifies the repository revision sequence used for the build.

### Git Commit

The short Git commit identifies the exact committed source revision.

Example:

`Git Commit: 0942894f`

The full commit is also embedded in the firmware metadata for future machine-readable use.

### Git State

- `clean`: the firmware was built from a clean Git working tree.
- `dirty`: local uncommitted changes existed when the firmware was built.

A dirty build must not be treated as uniquely reproducible from the commit hash alone.

### Build Date / Time

The C/C++ compiler build date and time are embedded into the firmware and printed at boot.

### Environment

The PlatformIO environment used for the build.

Example:

`esp32dev`

### Board

The physical target board.

Current Smart Farming target:

`ESP32 DevKitC V4 / ESP32 Dev Module`

## Example Serial banner

```text
============================================================
 OpenIoT Platform - Firmware Identity
------------------------------------------------------------
 Product      : OpenIoT Smart Farming
 Version      : 0.1.0
 Build        : #123
 Git Commit   : 0942894f
 Git State    : clean
 Build Date   : Oct  6 2026
 Build Time   : 16:42:18
 Environment  : esp32dev
 Board        : ESP32 DevKitC V4 / ESP32 Dev Module
============================================================
```

The exact build number, Git commit, and build timestamp are generated from the actual build environment and therefore vary by build.

## Build metadata generation

PlatformIO runs `scripts/build_metadata.py` as a PRE build script.

The script obtains:

- Git short commit
- Git full commit
- repository commit count
- working-tree state
- active PlatformIO environment

The values are compiled into the firmware as preprocessor definitions. No runtime Git access is required.

This follows the existing no-dynamic-allocation / embedded-runtime direction because Git and Python are used only during the host-side build process.

PlatformIO supports PRE build scripts and compile-time definitions for this purpose. See the PlatformIO build flags and advanced scripting documentation for the underlying mechanism.

## Validation requirement

For Phase 31 physical validation, the first part of the captured Serial Monitor log should include the firmware identity banner.

A field validation record should associate the observed hardware behavior with:

1. Firmware Version
2. Build number
3. Git Commit
4. Git State
5. Build Date/Time
6. PlatformIO Environment
7. Board

This prevents a physical validation result from being incorrectly attributed to a different firmware revision.

## Reproducibility rule

For a production-quality validation result:

- Prefer `Git State: clean`.
- Record the Git commit shown by the device.
- Record the complete Serial Monitor identity banner.
- Keep the corresponding GitHub commit available in the repository history.

A `dirty` build may be used for development debugging, but it must be explicitly identified as a local working-tree build.

## Future extension

The identity standard can later be exposed through existing device/network telemetry without changing the architecture. No such network behavior is introduced by this initial implementation.

## Change control

This standard does not create a new PKG or Phase.

It is an implementation/documentation enhancement within the existing Foundation / PlatformIO / Runtime boundaries.
