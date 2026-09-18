# Windows host contract

The Windows host is the adapter between the Cherokee Framework and Windows.

## First milestone

The host must work as a normal user-mode console application and must not require administrator privileges for ordinary script execution.

It should provide:

1. Console input/output through standard streams.
2. UTF-8 source loading with clear read errors.
3. Application-relative paths for local scripts.
4. Per-user package/config/cache directories.
5. Windows-friendly exit codes.
6. A stable place for future process and filesystem APIs.

## Compatibility rules

- Do not require Windows 10-only APIs in the Windows 7 build.
- Keep the core interpreter free of Windows API calls.
- Use the MSVC targets configured by `rust-toolchain.toml`.
- Any future Windows API dependency must document its minimum supported Windows version.
- Do not install services, registry entries, shell extensions, or system-wide files without an explicit installer feature and user consent.

## Future host layout

```text
host/windows/
├── console
├── paths
├── packages
└── process
```
