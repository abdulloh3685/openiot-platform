# OpenIoT Platform — Source Provenance

## Status

This repository contains **RE-IMPLEMENTED / NEW SOURCE** created after bounded historical source recovery was formally closed.

It is not claimed to be the original historical source, nor a reconstruction of missing historical files.

## Baseline

- Frozen architecture preserved.
- Historical source recovery: bounded and closed.
- Exact historical P0 source recovered: 0.
- New source lineage starts at this implementation branch.

## Evidence boundary

Historical project documents are design/intent evidence. They inform contracts but do not establish source identity.

## New lineage

```text
Frozen Architecture
  -> New Engineering Contracts
  -> Re-implementation
  -> Tests
  -> Build Evidence
  -> Review
  -> Commit
  -> Release
```

## Historical conflicts

Historical version references, OF-107 provenance, OF-106 source identity, and prior Device/DeviceManager relationships remain historical evidence only. New implementation decisions must not be represented as historical facts.
