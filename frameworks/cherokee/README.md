# Cherokee Framework

The Cherokee Framework is the Windows host contract for DoxRain Cherokee programs.

It is a user-mode runtime layer. It does **not** modify the Windows kernel, replace the Windows shell, or install itself into protected operating-system components. Instead, `doxrain.exe` hosts the Cherokee language engine and exposes safe Windows services to DoxRain programs.

## Responsibilities

- Start and stop a Cherokee program
- Provide console input and output
- Resolve application and package directories
- Load Cherokee libraries
- Report runtime errors
- Keep Windows-specific behavior separate from the language core

## Compatibility

The first host is Windows 7 through current Windows releases. Linux support will use a separate host implementation later while reusing the language core and framework contract.

## Planned API areas

- `console`
- `filesystem`
- `process`
- `environment`
- `packages`
- `logging`

The API is intentionally documented before implementation so framework behavior can be reviewed and changed without silently breaking scripts.
