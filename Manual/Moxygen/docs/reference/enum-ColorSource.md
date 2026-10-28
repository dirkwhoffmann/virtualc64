---
title: "ColorSource"
---
# ColorSource

```cpp
enum ColorSource
```

Defined in Components/VICII/VICIITypes.h:226

Source of a pixel color in the graphics synthesizer.

| Value | Description |
|-------|-------------|
| `D021` | Color comes from background color register. |
| `D022` | Color comes from first extended color register. |
| `D023` | Color comes from second extended color register. |
| `CHAR_LO` | Color comes from the low byte of the fetched character. |
| `CHAR_HI` | Color comes from the high byte of the fetched character. |
| `COLRAM3` | Color comes from the color RAM (lower three 3 bits). |
| `COLRAM4` | Color comes from the color RAM (all 4 bits). |
| `INDEXED` | Color comes from a color register. |
| `ZERO` | Invalid display modes. |

