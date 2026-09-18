# DoxRain unified architecture

DoxRain is the language. Frameworks provide specialized capabilities. Hosts connect those frameworks to an operating system or device.

```text
DoxRain source
    ↓
Shared language core
    ├── Interpreter       development and scripts
    ├── Bytecode VM       portable applications and games
    ├── Native compiler   high-performance applications
    └── Embedded backend  microcontrollers

Frameworks
    ├── Cherokee          games and applications
    ├── CyberSecurity     defensive security tooling
    └── Embedded          hardware and firmware

Hosts
    ├── Windows first
    ├── Linux later
    └── Microcontroller targets
```

## Rules

1. The parser and language semantics must remain platform-independent.
2. Windows functionality belongs in a Windows host adapter.
3. Frameworks declare the engine and host API they require.
4. The interpreter and compiler share the same frontend and language rules.
5. Framework updates are installed per-user and require compatibility checks.

## Current status

The repository is currently an early Windows foundation. The interpreter is not yet the complete prototype runtime. Cherokee is currently an architecture and contract, not yet a game engine.
