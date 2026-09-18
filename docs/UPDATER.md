# Framework updater plan

The updater discovers compatible DoxRain frameworks and stages them in per-user directories. It must not silently modify Windows system files.

## Commands

```bat
doxrain frameworks list
doxrain frameworks check
doxrain frameworks update
```

## Update process

```text
Fetch registry over HTTPS
    ↓
Verify registry signature
    ↓
Filter by framework API, host, architecture, and engine API
    ↓
Download archive to a temporary file
    ↓
Verify archive signature and SHA-256
    ↓
Stage in a versioned per-user directory
    ↓
Ask the user before activation
    ↓
Activate atomically or roll back
```

Recommended locations:

```text
%LOCALAPPDATA%\\DoxRain\\Frameworks\\Cherokee\\<version>\\
%LOCALAPPDATA%\\DoxRain\\Updates\\
```

## Safety requirements

- HTTPS only
- Signed registry metadata
- Signed framework archives or signed manifests
- SHA-256 integrity verification
- Explicit compatibility checks
- User approval before activation
- Atomic activation and rollback
- No administrator rights for normal user installs
- No automatic service, registry, shell, or kernel changes
- Offline/check-only mode

The first implementation should list and check packages before it installs anything. Automatic installation should wait until signature verification and rollback are implemented.
