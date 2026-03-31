# Enterprise Wi-Fi V1 Status

## Goal

Ship a minimal PEAP-capable firmware state without expanding scope.

V1 includes:

- PSK Wi-Fi
- PEAP Wi-Fi with `SSID + Username + Password`
- BLUFI provisioning for PEAP
- Audio provisioning for PEAP
- Legacy PSK settings compatibility

V1 excludes:

- Custom `Identity`
- `CA Certificate`
- Hotspot web UI support for PEAP
- GitHub push

## State Model

Each state must stay in one of these values only:

- `done`
- `blocked`
- `deferred`
- `todo`

## Current State

| State | Value | Exit Criteria | Notes |
| --- | --- | --- | --- |
| `ScopeLocked` | `done` | V1 scope is fixed and no longer expanding | PEAP only, no Identity/CA/UI expansion |
| `CoreModelReady` | `done` | Internal model stores `auth_type/ssid/username/password` | Implemented with `NetworkProfile` |
| `ProvisioningReady` | `done` | BLUFI and audio provisioning can write PEAP-capable settings | Hotspot web UI explicitly excluded from V1 |
| `ConnectionReady` | `done` | Runtime can branch between PSK and enterprise connection flows | Implemented with `WifiProfileConnector` |
| `CompatReady` | `done` | Existing PSK users keep working after upgrade | Falls back to legacy `wifi` namespace |
| `BuildVerified` | `blocked` | Successful ESP-IDF build in a real toolchain | Current machine has no `idf.py` / `IDF_PATH` |
| `DeviceVerified` | `todo` | Hardware tests pass for PSK + PEAP + fallback behavior | Needs real device and network |

## Mandatory For V1 Complete

V1 is complete only when all of these are true:

- `ScopeLocked = done`
- `ConnectionReady = done`
- `BuildVerified = done`
- `DeviceVerified = done`

## Deferred Items

These items must remain out of scope unless V1 is explicitly reopened:

- Hotspot web UI PEAP fields
- Custom outer identity field
- CA certificate management
- Broader enterprise compatibility work beyond PEAP username/password
- GitHub push

## Mutation Rules

Only mutate the plan by changing state values or moving an item between `mandatory` and `deferred`.

Allowed mutations:

- If Hotspot PEAP UI becomes required, change `ProvisioningReady` exit criteria and move Hotspot UI out of deferred.
- If device testing is intentionally skipped, change `DeviceVerified` from mandatory to deferred.
- If backward compatibility is no longer required, change `CompatReady` to deferred.

Disallowed mutations:

- Do not add Identity support unless scope is explicitly reopened.
- Do not add CA certificate handling unless scope is explicitly reopened.
- Do not silently turn deferred items into implicit requirements.

## Verification Checklist

Build verification:

- ESP-IDF environment available
- Full firmware build completes
- No link errors from enterprise Wi-Fi APIs

Device verification:

- Fresh device can connect to normal PSK Wi-Fi
- Device with legacy stored PSK settings still connects after upgrade
- BLUFI can provision PEAP credentials and connect
- Audio provisioning can provision PEAP credentials and connect
- Wrong PEAP password fails cleanly and returns to provisioning path

