# DoxRain Cherokee 0.5.0

**Experimental Windows test release**

Cherokee 0.5.0 tests a new C-backed compiler/interpreter engine connected to the Rust CLI. It is not the final DoxRain compiler or runtime.

DoxRain is intended to offer Rust-inspired safety goals with simpler, more approachable syntax. Long-term targets include 2D development, future 3D/AAA work, defensive cybersecurity tooling, external applications, and embedded programming.

## Run

```bat
doxrain.exe --version
doxrain.exe --check examples\hello.dox
doxrain.exe examples\hello.dox
```

## This test build

The Rust program provides the Cargo/MSVC entrypoint and calls the C engine. The C engine currently compiles and executes the initial `let`/`show` language slice and reports source-file diagnostics with error codes and line context. The Rust full prototype remains the reference for the larger language direction while the C engine is expanded.

Implemented in this slice:

- variables with `let`
- strings and `{name}` interpolation
- numbers and basic arithmetic
- `show`
- comments beginning with `#` or `//`
- `--check`, `--version`, and `--help`
- readable errors containing a file, line, code, and source snippet

Cherokee 0.5.0 is intentionally a testing milestone. Unsupported prototype features produce a clear diagnostic instead of being silently treated as implemented.

## Included files

The release package contains `doxrain.exe`, this README, the license, and runnable `.dox` examples.

## Build

The Windows build uses Cargo with Rust 1.77.2 and the MSVC targets. Cargo invokes the C compiler through `build.rs`:

```bat
cargo build --release --target x86_64-pc-windows-msvc
cargo build --release --target i686-pc-windows-msvc
```

Keep Cherokee 0.1.0 as the historical Windows test release. This 0.5.0 build is a separate experimental engine test.
