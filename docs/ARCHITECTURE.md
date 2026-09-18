# DoxRain architecture

## Current layers

```text
.dox source
    ↓
DoxRain CLI (`doxrain.exe`)
    ↓
Cherokee Framework contract
    ↓
Windows host adapter
    ↓
Windows user-mode services
```

## Design rule

The language engine should not know that it is running on Windows. Windows-specific behavior belongs in the host adapter. This lets the same Cherokee language remain stable when a Linux host is added later.

## What “attaches to Windows” means

Cherokee is designed to attach to Windows as a user-mode runtime: users launch `doxrain.exe`, and the runtime connects language programs to approved Windows services. It is not intended to patch Windows, run inside the kernel, or silently change the operating system.

## Framework identity

- Product: DoxRain Cherokee
- Framework: Cherokee Framework
- Host: Windows Host
- Technical engine version: currently `0.1.0`

The display name may remain stable while the technical API versions change under compatibility rules.

## Planned implementation order

1. Move lexer, parser, AST, and interpreter into core modules.
2. Define runtime traits for console, paths, packages, and processes.
3. Implement the Windows host using those traits.
4. Add the Cherokee standard library.
5. Add package manifests and framework compatibility checks.
6. Add a second host only after Windows behavior is tested.
