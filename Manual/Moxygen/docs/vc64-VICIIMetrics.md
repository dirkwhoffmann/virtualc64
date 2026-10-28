---
title: "VICIIMetrics"
---
# VICIIMetrics

```cpp
#include <Components/VICII/VICIITypes.h>
```

```cpp
struct VICIIMetrics
```

Defined in Components/VICII/VICIITypes.h:353

Performance statistics of the graphics synthesizer.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `isize` | [`canvasFastPath`](#canvasfastpath)  | Number of canvas drawings that took the fast path. |
| `isize` | [`canvasSlowPath`](#canvasslowpath)  | Number of canvas drawings that took the slow path. |
| `isize` | [`spriteFastPath`](#spritefastpath)  | Number of sprite drawings that took the fast path. |
| `isize` | [`spriteSlowPath`](#spriteslowpath)  | Number of sprite drawings that took the slow path. |
| `isize` | [`quickExitHit`](#quickexithit)  | Number of successful quick-exit checks. |
| `isize` | [`quickExitMiss`](#quickexitmiss)  | Number of failed quick-exit checks. |

---

### canvasFastPath

```cpp
isize canvasFastPath
```

Defined in Components/VICII/VICIITypes.h:356

Number of canvas drawings that took the fast path.

---

### canvasSlowPath

```cpp
isize canvasSlowPath
```

Defined in Components/VICII/VICIITypes.h:357

Number of canvas drawings that took the slow path.

---

### spriteFastPath

```cpp
isize spriteFastPath
```

Defined in Components/VICII/VICIITypes.h:358

Number of sprite drawings that took the fast path.

---

### spriteSlowPath

```cpp
isize spriteSlowPath
```

Defined in Components/VICII/VICIITypes.h:359

Number of sprite drawings that took the slow path.

---

### quickExitHit

```cpp
isize quickExitHit
```

Defined in Components/VICII/VICIITypes.h:360

Number of successful quick-exit checks.

---

### quickExitMiss

```cpp
isize quickExitMiss
```

Defined in Components/VICII/VICIITypes.h:361

Number of failed quick-exit checks.

