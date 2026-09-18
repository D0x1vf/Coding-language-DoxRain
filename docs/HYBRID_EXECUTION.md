# Hybrid execution plan

DoxRain will support more than one execution backend while keeping one language.

## Planned commands

```bat
doxrain run game.dox
doxrain check game.dox
doxrain build game.dox --bytecode
doxrain build game.dox --target windows-x64
```

## Backend order

1. Complete the AST interpreter.
2. Add a bytecode representation and virtual machine.
3. Add native compilation for Windows.
4. Add an embedded backend with a restricted, deterministic runtime.

Every backend must use shared lexer, parser, diagnostics, module, and compatibility rules. A program must not silently change meaning because it was interpreted or compiled.
