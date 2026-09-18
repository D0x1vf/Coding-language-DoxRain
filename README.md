# DoxRain Cherokee

DoxRain Cherokee is the Windows-first foundation for the DoxRain language and its first game/application framework.

DoxRain is intended to grow toward:

- 2D game development first
- future 3D and AAA-oriented development
- cybersecurity and defensive security tooling
- external and embedded programming, including microcontrollers

## Unified model

- **DoxRain** is the language.
- **Cherokee** is the first games and applications framework.
- **Windows** is the first host platform.
- **Interpreter** is the first execution backend.
- **Bytecode and native compilation** are planned later.
- **CyberSecurity and Embedded** are later specialized frameworks.

The current branch is an early foundation. It currently provides a basic Windows CLI, `.dox` file execution, simple expressions, documentation, and build configuration. It is not yet a complete game framework or compiler.

## Framework updates

The updater will eventually discover compatible frameworks and versions, verify them, and stage them per user. It will not silently alter Windows. See [`docs/UPDATER.md`](docs/UPDATER.md).

## Build

The initial Windows 7-compatible target is Rust 1.77.2 with MSVC:

```bat
cargo build --release --target x86_64-pc-windows-msvc
cargo build --release --target i686-pc-windows-msvc
```

## Example

```dox
let platform = "Windows"
show "Hello from {platform}!"
```

This project remains Windows-first; Linux support is planned after the Windows core and Cherokee framework are stable.
