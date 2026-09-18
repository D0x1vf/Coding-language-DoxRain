# DoxRain Framework Versions

DoxRain uses names for public identities and technical API values for compatibility.

Example:

```text
Public product:    DoxRain Cherokee
Framework:         Cherokee Framework
Framework API:     0.1
Engine API:        0.1
Host:              Windows
Architectures:     x86, x86_64
```

Names communicate identity. Technical API values prevent an incompatible framework from loading into an older engine. A future named release can preserve its public name while receiving compatible maintenance builds.

A framework package must declare:

- framework name and API
- required engine API
- host operating system
- supported architectures
- package checksum
- publisher signature when production updates are enabled
