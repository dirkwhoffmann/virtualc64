---
title: "VoiceInfo"
---
# VoiceInfo

```cpp
#include <Components/SID/SIDTypes.h>
```

```cpp
struct VoiceInfo
```

Defined in Components/SID/SIDTypes.h:161

State of a SID voice.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `u8` | [`reg`](#reg)  | Raw voice registers. |
| `u16` | [`frequency`](#frequency)  | Frequency value. |
| `u16` | [`pulseWidth`](#pulsewidth)  | Pulse width value. |
| `u8` | [`waveform`](#waveform)  | Waveform selection bits. |
| `bool` | [`ringMod`](#ringmod)  | Ring modulation is on. |
| `bool` | [`hardSync`](#hardsync)  | Hard sync is on. |
| `bool` | [`gateBit`](#gatebit)  | Gate bit. |
| `bool` | [`testBit`](#testbit)  | Test bit. |
| `u8` | [`attackRate`](#attackrate)  | Envelope attack rate. |
| `u8` | [`decayRate`](#decayrate)  | Envelope decay rate. |
| `u8` | [`sustainRate`](#sustainrate)  | Envelope sustain level. |
| `u8` | [`releaseRate`](#releaserate)  | Envelope release rate. |
| `bool` | [`filterEnableBit`](#filterenablebit)  | The voice passes through the filter. |

---

### reg

```cpp
u8 reg[7]
```

Defined in Components/SID/SIDTypes.h:163

Raw voice registers.

---

### frequency

```cpp
u16 frequency
```

Defined in Components/SID/SIDTypes.h:164

Frequency value.

---

### pulseWidth

```cpp
u16 pulseWidth
```

Defined in Components/SID/SIDTypes.h:165

Pulse width value.

---

### waveform

```cpp
u8 waveform
```

Defined in Components/SID/SIDTypes.h:166

Waveform selection bits.

---

### ringMod

```cpp
bool ringMod
```

Defined in Components/SID/SIDTypes.h:167

Ring modulation is on.

---

### hardSync

```cpp
bool hardSync
```

Defined in Components/SID/SIDTypes.h:168

Hard sync is on.

---

### gateBit

```cpp
bool gateBit
```

Defined in Components/SID/SIDTypes.h:169

Gate bit.

---

### testBit

```cpp
bool testBit
```

Defined in Components/SID/SIDTypes.h:170

Test bit.

---

### attackRate

```cpp
u8 attackRate
```

Defined in Components/SID/SIDTypes.h:171

Envelope attack rate.

---

### decayRate

```cpp
u8 decayRate
```

Defined in Components/SID/SIDTypes.h:172

Envelope decay rate.

---

### sustainRate

```cpp
u8 sustainRate
```

Defined in Components/SID/SIDTypes.h:173

Envelope sustain level.

---

### releaseRate

```cpp
u8 releaseRate
```

Defined in Components/SID/SIDTypes.h:174

Envelope release rate.

---

### filterEnableBit

```cpp
bool filterEnableBit
```

Defined in Components/SID/SIDTypes.h:175

The voice passes through the filter.

