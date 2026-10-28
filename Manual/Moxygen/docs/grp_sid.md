---
title: "SID"
---
# SID

The SID sound chip.

## Classes

| Name | Description |
|------|-------------|
| [`SIDAPI`](vc64-SIDAPI.md#sidapi) | SID Public [API](vc64-API.md#api) |
| [`SIDConfig`](vc64-SIDConfig.md#sidconfig) | Configuration of a SID chip. |
| [`SIDInfo`](vc64-SIDInfo.md#sidinfo) | State of a SID chip. |
| [`VoiceInfo`](vc64-VoiceInfo.md#voiceinfo) | State of a SID voice. |

## Enumerations

| Name | Description |
|------|-------------|
| [`SIDEngine`](#sidengine)  | SID backend. |
| [`SIDRevision`](#sidrevision)  | Chip revision. |
| [`SamplingMethod`](#samplingmethod)  | Sampling method of the reSID engine. |

---

### SIDEngine

```cpp
enum SIDEngine
```

Defined in Components/SID/SIDTypes.h:62

SID backend.

| Value | Description |
|-------|-------------|
| `RESID` |  |

---

### SIDRevision

```cpp
enum SIDRevision
```

Defined in Components/SID/SIDTypes.h:34

Chip revision.

| Value | Description |
|-------|-------------|
| `MOS_6581` |  |
| `MOS_8580` |  |

---

### SamplingMethod

```cpp
enum SamplingMethod
```

Defined in Components/SID/SIDTypes.h:88

Sampling method of the reSID engine.

| Value | Description |
|-------|-------------|
| `FAST` |  |
| `INTERPOLATE` |  |
| `RESAMPLE` |  |
| `RESAMPLE_FASTMEM` |  |
