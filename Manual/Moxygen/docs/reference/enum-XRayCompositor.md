---
title: "XRayCompositor"
---
# XRayCompositor

```cpp
enum XRayCompositor
```

Defined in Components/VICII/XRayTypes.h:64

Who merges the X-Ray texture into the emulator texture.

| Value | Description |
|-------|-------------|
| `CPU` | The emulator does it. |
| `GPU` | The host does it (e.g., in its merge shader). |

