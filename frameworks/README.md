# Framework package manifest

Framework packages use a manifest so the updater can determine whether a package is safe to stage for the current installation.

Example:

```toml
name = "cherokee"
display_name = "Cherokee Framework"
framework_api = "0.1"
engine_api = "0.1"
host = "windows"
architectures = ["x86", "x86_64"]
archive = "cherokee-0.1.0.zip"
sha256 = "REPLACE_WITH_RELEASE_CHECKSUM"
```

A production manifest must also contain signed metadata. A plain checksum detects corruption but does not prove who published a package.
