---
title: "AudioPortConfig"
---
# AudioPortConfig

```cpp
#include <Ports/AudioPortTypes.h>
```

```cpp
struct AudioPortConfig
```

Defined in Ports/AudioPortTypes.h:25

Configuration of the audio port.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `i64` | [`vol`](#vol)  | Channel volumes. |
| `i64` | [`pan`](#pan-1)  | Channel pan factors. |
| `i64` | [`volL`](#voll)  | Master volume (left). |
| `i64` | [`volR`](#volr)  | Master volume (right). |
| `isize` | [`bufferSize`](#buffersize)  | Sample buffer capacity. |
| `bool` | [`asr`](#asr)  | Adaptive Sample Rate enable switch. |

---

### vol

```cpp
i64 vol[4]
```

Defined in Ports/AudioPortTypes.h:28

Channel volumes.

---

### pan

```cpp
i64 pan[4]
```

Defined in Ports/AudioPortTypes.h:31

Channel pan factors.

---

### volL

```cpp
i64 volL
```

Defined in Ports/AudioPortTypes.h:34

Master volume (left).

---

### volR

```cpp
i64 volR
```

Defined in Ports/AudioPortTypes.h:35

Master volume (right).

---

### bufferSize

```cpp
isize bufferSize
```

Defined in Ports/AudioPortTypes.h:38

Sample buffer capacity.

---

### asr

```cpp
bool asr
```

Defined in Ports/AudioPortTypes.h:41

Adaptive Sample Rate enable switch.

