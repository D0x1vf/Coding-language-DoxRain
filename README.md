# DoxRain Cherokee

DoxRain Cherokee is the Windows-first foundation for the DoxRain programming language and its early host/runtime direction.

This repository is currently an early interpreter-based milestone. It is intended for experimentation, feedback, and foundation work rather than a final language runtime.

## Repository layout

- `src/` — Rust source for the interpreter and CLI
- `host/windows/` — Windows host layer and platform-specific behavior
- `examples/` — example `.dox` programs
- `docs/` — architecture, updater, and tutorial documentation
- `docs/tutorials/` — multilingual tutorial set (main tutorial entry point)
- `frameworks/` — framework manifests and framework design docs
- `.github/workflows/` — GitHub Actions workflows for build automation
- `Cargo.toml` — Rust project configuration
- `WINDOWS.md` — Windows build and target notes

## Current status

The project is currently a Windows-first foundation release.

Supported target states in this repo:

- Windows x64: `x86_64-pc-windows-msvc`
- Windows x86: `i686-pc-windows-msvc`

The current interpreter supports a small early language surface: variables, strings, arithmetic, `show`, `--check`, and basic CLI behavior. Larger features described elsewhere in the project are still planned or under active development.

## Releases

The current public release is a Windows test-platform build:

- `DoxRain-Cherokee-Windows-x64.zip`
- `DoxRain-Cherokee-Windows-x86.zip`

These are attached to the GitHub release and are intended for early evaluation.

## Main tutorials

The active tutorial set is in the multilingual docs folder:

- `docs/tutorials/DoxRain_Tutorial_EN-GB.md`
- `docs/tutorials/DoxRain_Tutorial_ES.md`
- `docs/tutorials/DoxRain_Tutorial_FR.md`
- `docs/tutorials/DoxRain_Tutorial_AR.md`
- `docs/tutorials/DoxRain_Tutorial_RU.md`
- `docs/tutorials/DoxRain_Tutorial_UK.md`
- `docs/tutorials/DoxRain_Tutorial_ZH.md`

These are the main tutorial sources for the project.

## Core docs

- `docs/ARCHITECTURE.md` — project architecture
- `docs/UNIFIED_ARCHITECTURE.md` — unified design model
- `docs/CHEROKEE_FRAMEWORK.md` — Cherokee framework concepts
- `docs/FRAMEWORK_VERSIONS.md` — framework versioning notes
- `docs/HYBRID_EXECUTION.md` — hybrid execution model
- `docs/UPDATER.md` — updater design and responsibilities
- `WINDOWS.md` — Windows build notes and validation matrix
- `frameworks/cherokee/README.md` — Cherokee framework overview

## Build

Windows builds are produced with Rust and MSVC targets:

```bat
cargo build --release --target x86_64-pc-windows-msvc
cargo build --release --target i686-pc-windows-msvc
```

## Quick run

From the extracted Windows build folder:

```bat
doxrain.exe --version
doxrain.exe --check examples\hello.dox
doxrain.exe examples\hello.dox
```

## Example

```dox
let platform = "Windows"
show "Hello from {platform}"
```

## Notes

- This is a foundation-level project and a test platform release.
- The repo is intentionally organized around a Windows-first host model.
- The multilingual tutorial set is the canonical documentation for learning the language.
- The root-level tutorial file is kept only as a compatibility stub and redirects users to the real tutorial directory.
