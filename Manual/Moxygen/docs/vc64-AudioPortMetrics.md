---
title: "AudioPortMetrics"
---
# AudioPortMetrics

```cpp
#include <Ports/AudioPortTypes.h>
```

```cpp
struct AudioPortMetrics
```

Defined in Ports/AudioPortTypes.h:54

Audio statistics.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `double` | [`fillLevel`](#filllevel)  | Current ring buffer fill level. |
| `u64` | [`bufferUnderflows`](#bufferunderflows)  | Number of buffer underflows since power up. |
| `u64` | [`bufferOverflows`](#bufferoverflows)  | Number of buffer overflows since power up. |
| `i64` | [`producedSamples`](#producedsamples)  | Total number of generated sampleds. |
| `i64` | [`idleSamples`](#idlesamples)  | Total number of samples produced without executing SID. |
| `i64` | [`consumedSamples`](#consumedsamples)  | Total number of sampels grabbed by the audio backend. |

---

### fillLevel

```cpp
double fillLevel
```

Defined in Ports/AudioPortTypes.h:57

Current ring buffer fill level.

---

### bufferUnderflows

```cpp
u64 bufferUnderflows
```

Defined in Ports/AudioPortTypes.h:60

Number of buffer underflows since power up.

---

### bufferOverflows

```cpp
u64 bufferOverflows
```

Defined in Ports/AudioPortTypes.h:63

Number of buffer overflows since power up.

---

### producedSamples

```cpp
i64 producedSamples
```

Defined in Ports/AudioPortTypes.h:66

Total number of generated sampleds.

---

### idleSamples

```cpp
i64 idleSamples
```

Defined in Ports/AudioPortTypes.h:69

Total number of samples produced without executing SID.

---

### consumedSamples

```cpp
i64 consumedSamples
```

Defined in Ports/AudioPortTypes.h:72

Total number of sampels grabbed by the audio backend.

