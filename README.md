# DoxRain Cherokee

DoxRain Cherokee is the Windows-first continuation of the DoxRain scripting language.

This branch establishes the Windows distribution foundation. The Linux release will be handled after the Windows implementation is stable.

## Current Windows milestone

The command-line interpreter supports `.dox` files containing `let`, `show`, strings, variables, simple arithmetic, comments, and interpolation.

```text
let name = "Windows"
let answer = 40 + 2
show "Hello, {name}!"
show answer
```

Run it with:

```bat
doxrain.exe hello.dox
```

Check a file without executing it:

```bat
doxrain.exe --check hello.dox
```

## Windows support plan

The project is pinned to Rust 1.77.2 for the initial Windows 7-compatible release. Builds are planned for `x86_64-pc-windows-msvc` and `i686-pc-windows-msvc`, covering 64-bit and 32-bit Windows installations from Windows 7 through current Windows versions.

The repository's release pages and tags remain under the maintainer's manual control.

## Build on Windows

Install Rust 1.77.2 with the MSVC toolchain, then run:

```bat
cargo build --release --target x86_64-pc-windows-msvc
```

The executable is written to `target\\x86_64-pc-windows-msvc\\release\\doxrain.exe`.

## License

DoxRain is distributed under the GNU Affero General Public License v3.
