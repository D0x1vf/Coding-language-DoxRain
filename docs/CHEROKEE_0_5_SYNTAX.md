# Cherokee 0.5.0 syntax target

This document defines the implementation target for the experimental fuller engine. It is a target contract, not a claim that every item is already complete.

## Statements

```dox
let count = 3
count = count + 1
show count

if count > 2 {
    show "large"
} else {
    show "small"
}

for i in 0..count {
    show i
}
```

## Functions

```dox
fn multiply(a, b) {
    return a * b
}

let result = multiply(6, 7)
show result
```

## Values and expressions

The target value model contains integers, floats, booleans, strings, and a void result. The expression model targets:

- `+`, `-`, `*`, `/`
- `==`, `!=`, `<`, `>`, `<=`, `>=`
- parentheses
- unary minus
- variable references
- function calls
- interpolation such as `"value = {result}"`

## Error contract

Every syntax or runtime failure should preserve its source location and produce:

- a stable error code
- a human-readable message
- filename
- line and column
- source-line context
- an optional help message

Unsupported syntax must produce an explicit diagnostic; it must not silently execute as a different statement.
