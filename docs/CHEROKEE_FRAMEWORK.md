# Cherokee Framework decision record

## Decision

Cherokee Framework will be a user-mode host framework around the DoxRain interpreter, beginning with Windows support.

## Why

This gives Cherokee a stable place for operating-system services without putting Windows-specific code into the parser or evaluator. It also leaves a clear path for a future Linux host.

## Not included in the first Windows milestone

- Windows kernel drivers
- Shell replacement
- Automatic system-wide installation
- Background services
- Registry modification
- GUI framework

Those capabilities would require separate design, security review, and explicit user-facing configuration.
