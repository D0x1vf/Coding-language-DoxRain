# DoxRain Cherokee 0.5.0

Cherokee 0.5.0 is an experimental DoxRain language milestone. It tests a fuller language engine built around a C compiler/interpreter backend connected to the Rust Cargo/MSVC entrypoint.

DoxRain is intended to pursue Rust-like safety goals with simpler, more approachable syntax. Its long-term targets include 2D development, future 3D and AAA-oriented development, defensive cybersecurity tooling, external applications, and embedded programming.

## What this release is

Cherokee 0.5.0 is a testing version, not a finished compiler or stable language specification. The Rust prototype remains the reference for language ideas while the C engine is expanded toward a substantially fuller implementation.

The release direction is:

```text
.dox source → C lexer/parser → C compiler/interpreter → runtime
                         ↘ Rust CLI/build integration
```

Cargo and the MSVC toolchain remain the Windows build system. That does not make Rust the main language engine: the C backend is the experimental execution engine.

## Current syntax target

The 0.5.0 engine is being expanded toward these core features:

- `let` variables and reassignment
- integers, floating-point values, booleans, and strings
- arithmetic and comparison expressions
- string interpolation
- `show` output
- `if` / `else` blocks
- `for` range loops
- user functions and return values
- comments beginning with `#` or `//`
- `--check`, `--version`, and `--help`
- source-aware diagnostics with error code, file, line, column, and source text

Only features confirmed by the executable should be treated as available. Arrays, imports, packages, a type checker, bytecode generation, and native compilation remain later work unless explicitly marked otherwise in the examples.

Example target syntax:

```dox
fn add(a, b) {
    return a + b
}

let result = add(20, 22)
if result == 42 {
    show "answer = {result}"
} else {
    show "unexpected result"
}

for i in 0..3 {
    show "item {i}"
}
```

## Run

```bat
doxrain.exe --version
doxrain.exe --check examples\hello.dox
doxrain.exe examples\hello.dox
```

## Diagnostics

The new engine is intended to report errors in a form that is useful to people, not only to the build system:

```text
error[E105]: division by zero
 --> examples/errors/division_by_zero.dox:3:10
  |
3 | show 10 / zero
  |          ^
```

Diagnostics are part of the Cherokee 0.5.0 experiment and will continue to improve as the parser and runtime grow.

## Build

Windows builds use Rust 1.77.2, Cargo, and the MSVC targets. Cargo invokes the C build through `build.rs`:

```bat
cargo build --release --target x86_64-pc-windows-msvc
cargo build --release --target i686-pc-windows-msvc
```

The dedicated workflow packages the executable, this README, the license, and the `.dox` examples for testing.

## Release history

- **Cherokee 0.1.0** — historical Windows test release using the original small Rust engine.
- **Cherokee 0.5.0** — experimental C compiler/interpreter engine and fuller syntax development.
- **Leonardo** — planned later generation after the experimental semantics and runtime are proven.

## Repository

- `src/main.rs` — Rust CLI and C-engine integration boundary
- `runtime_c/` — experimental C compiler/interpreter runtime
- `doxrain_fullcode_Prototype.rs` — Rust reference prototype
- `examples/` — executable language examples
- `.github/workflows/` — build and packaging automation
