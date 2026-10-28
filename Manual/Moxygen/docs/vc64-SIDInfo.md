---
title: "SIDInfo"
---
# SIDInfo

```cpp
#include <Components/SID/SIDTypes.h>
```

```cpp
struct SIDInfo
```

Defined in Components/SID/SIDTypes.h:180

State of a SID chip.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `u8` | [`volume`](#volume)  | Master volume. |
| `u16` | [`filterCutoff`](#filtercutoff)  | Filter cutoff frequency. |
| `u8` | [`filterResonance`](#filterresonance)  | Filter resonance. |
| `u8` | [`filterModeBits`](#filtermodebits)  | Filter mode bits. |
| `u8` | [`filterType`](#filtertype)  | Filter type. |
| `u8` | [`filterEnableBits`](#filterenablebits)  | Voices routed through the filter. |
| `u8` | [`potX`](#potx)  | Paddle value X (POTX). |
| `u8` | [`potY`](#poty)  | Paddle value Y (POTY). |
| [`VoiceInfo`](vc64-VoiceInfo.md#voiceinfo) | [`voice`](#voice)  | State of the three voices. |

---

### volume

```cpp
u8 volume
```

Defined in Components/SID/SIDTypes.h:182

Master volume.

---

### filterCutoff

```cpp
u16 filterCutoff
```

Defined in Components/SID/SIDTypes.h:183

Filter cutoff frequency.

---

### filterResonance

```cpp
u8 filterResonance
```

Defined in Components/SID/SIDTypes.h:184

Filter resonance.

---

### filterModeBits

```cpp
u8 filterModeBits
```

Defined in Components/SID/SIDTypes.h:185

Filter mode bits.

---

### filterType

```cpp
u8 filterType
```

Defined in Components/SID/SIDTypes.h:186

Filter type.

---

### filterEnableBits

```cpp
u8 filterEnableBits
```

Defined in Components/SID/SIDTypes.h:187

Voices routed through the filter.

---

### potX

```cpp
u8 potX
```

Defined in Components/SID/SIDTypes.h:188

Paddle value X (POTX).

---

### potY

```cpp
u8 potY
```

Defined in Components/SID/SIDTypes.h:189

Paddle value Y (POTY).

---

### voice

```cpp
VoiceInfo voice[3]
```

Defined in Components/SID/SIDTypes.h:191

State of the three voices.

