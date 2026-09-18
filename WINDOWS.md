# Windows build notes

DoxRain Cherokee is currently Windows-first.

## Supported targets

- `x86_64-pc-windows-msvc` — 64-bit Windows
- `i686-pc-windows-msvc` — 32-bit Windows

Rust 1.77.2 is pinned because it is the final stable compiler release with the original Windows 7 baseline. Use the Visual Studio C++ build tools and the MSVC Rust targets.

## Build both architectures

```bat
cargo build --release --target x86_64-pc-windows-msvc
cargo build --release --target i686-pc-windows-msvc
```

## Run

```bat
doxrain.exe examples\\hello.dox
doxrain.exe --check examples\\hello.dox
doxrain.exe --version
doxrain.exe --help
```

Do not copy DLLs from a development machine into the release directory unless a future dependency explicitly requires them. The current milestone uses the Rust standard library only.

## Verification matrix

Before a Windows release, test the produced executables on clean Windows 7 64-bit, Windows 7 32-bit, Windows 10, and Windows 11 systems. Test both normal execution and `--check`.
