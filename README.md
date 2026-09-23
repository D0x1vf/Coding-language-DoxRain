# DoxRain

DoxRain is an experimental programming language designed to make programming readable without giving up useful low-level control. Its syntax is intended to be approachable for beginners while remaining suitable for serious software projects.

The language is being developed with these goals:

- clear, human-readable source code
- predictable behavior and useful diagnostics
- simple syntax for learning and rapid development
- strong safety goals inspired by modern systems languages
- a path from scripting and applications to games, tools, and embedded software

DoxRain is still under active development. The language rules are being tested and may change.

## Language style

A small DoxRain program looks like this:

```dox
let name = "DoxRain"
let answer = 40 + 2

show "Hello from {name}"
show "The answer is {answer}"
```

DoxRain uses readable statements and familiar expressions. Comments can begin with `#` or `//`.

```dox
# A variable can be changed during a program.
let score = 10
score = score + 5
show score
```

## Core syntax direction

The current language design is being expanded around:

- variables and assignment
- integers and floating-point numbers
- booleans and strings
- arithmetic expressions
- comparisons such as `==`, `!=`, `<`, `>`, `<=`, and `>=`
- string interpolation with `{name}`
- `show` output
- `if` and `else` blocks
- range-based `for` loops
- functions with parameters and return values
- comments

Example:

```dox
fn add(a, b) {
    return a + b
}

let result = add(20, 22)

if result == 42 {
    show "The answer is {result}"
} else {
    show "The result was {result}"
}

for i in 0..3 {
    show "item {i}"
}
```

This example describes the current syntax target. Check the examples and implementation before relying on a feature in a project.

## Errors that explain the problem

DoxRain is intended to show errors in a way that helps a person fix the source code. Diagnostics should identify the error, file, line, column, and the relevant source text.

```text
error[E105]: division by zero
 --> examples/errors/division_by_zero.dox:3:10
  |
3 | show 10 / zero
  |          ^
```

The diagnostic system is part of the language design, not only a compiler detail. Clear errors are especially important as the syntax grows.

## Current implementation

The project is testing a compiler/interpreter hybrid:

```text
.dox source
    ↓
lexer and parser
    ↓
compiler/interpreter runtime
    ↓
program output
```

The current test engine is written in C and integrated with a Rust command-line and build boundary. The Rust full prototype is kept as a reference implementation for language ideas and behavior while the C engine is expanded.

Cherokee is the name used for the current Windows-first test-release line. Cherokee 0.5.0 is an experimental milestone for the newer engine; it is not a final DoxRain specification.

## Try DoxRain

From a built executable:

```bat
doxrain.exe examples\hello.dox
doxrain.exe --check examples\hello.dox
doxrain.exe --version
doxrain.exe --help
```

The `examples/` directory contains small programs and diagnostic fixtures. Examples that describe planned syntax are marked accordingly.

## Build on Windows

The Windows build uses Cargo and the MSVC targets. Cargo compiles the C engine through `build.rs`:

```bat
cargo build --release --target x86_64-pc-windows-msvc
cargo build --release --target i686-pc-windows-msvc
```

Rust 1.77.2 is currently pinned for the Windows-first test configuration. Visual Studio C++ Build Tools are required for the MSVC targets.

## Project status

DoxRain is a language experiment, not a stable production compiler. The following work is still evolving:

- fuller parsing and evaluation
- complete control flow and function behavior
- structured type and runtime rules
- arrays, modules, and libraries
- bytecode and native compilation options
- cross-platform hosts

The immediate goal is a coherent, readable language core that can be tested through both the Rust reference prototype and the C-backed experimental engine.
