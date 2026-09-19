# DoxRain Cherokee 1.0.0 — Tutorial (English)

## Release

DoxRain Cherokee 1.0.0 is the first Windows-first foundation release of the DoxRain language. It targets Windows 7 through current Windows versions with x86 and x64 MSVC builds. Linux support is planned for a future release.

The release direction is 2D development first, followed by future 3D/AAA work, defensive cybersecurity tooling, and external or embedded programming. The interpreter is the first execution backend; bytecode and native compilation are planned later.

No compiled ZIP assets are attached to the release yet. The project is therefore a foundation and development release, not a finished production framework.

## Language source

The pure Rust prototype (`doxrain_fullcode_Prototype.rs`) contains a lexer, parser, AST, environment, interpreter, package manager, and an experimental compiler/VM direction. It is a reference implementation of the language model, while the Cherokee branch defines the Windows host and framework architecture.

## Basic syntax

```dox
let name = "DoxRain"
let answer = 40 + 2
show "Hello, {name}!"
show answer
```

Values include integers, floating-point numbers, booleans, strings, lists, and `Void`. Operators include `+`, `-`, `*`, `/`, `%`, `==`, `!=`, `<`, `>`, `<=`, and `>=`.

```dox
if answer >= 42 {
    show "Correct"
} else {
    show "Try again"
}

for i in 0..3 {
    show "Item {i}"
}
```

Functions use `fn`. The prototype supports scoped execution and `return` syntax, but return propagation and function integration remain under development.

```dox
fn greet(user) {
    show "Welcome, {user}"
}

greet("Rain")
```

## Packages and frameworks

```dox
pkg list
pkg install scene
use scene
pkg remove scene
```

The prototype package manager is local/mock infrastructure. Cherokee framework packages will eventually be installed per user after compatibility, checksum, and signature checks.

## Domain direction

Cherokee is the first game/application framework. Planned APIs include console, filesystem, process, environment, packages, and logging. Future domain frameworks include CyberSecurity and Embedded.

## Building

The planned targets are:

```bat
cargo build --release --target x86_64-pc-windows-msvc
cargo build --release --target i686-pc-windows-msvc
```

Test Windows 7 SP1, Windows 10, and Windows 11 on both appropriate architectures before claiming compatibility.

## Important limitations

The prototype is not yet a complete compiler, game engine, security suite, or embedded toolchain. Domain builtins and the compiler/VM need validation, tests, diagnostics, and a stable CLI before production use. Security features must be defensive and authorised only.
