---
title: "DiskAPI"
kind: class
---
# DiskAPI

```cpp
#include <VirtualC64.h>
```

```cpp
class DiskAPI
```

Defined in VirtualC64.h:1295

Disk Public [API](/reference/vc64-API.md#api)

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `class FloppyDisk *` | [`get`](#get-2)  | Getter for the raw disk object. |

---

### get

```cpp
class FloppyDisk * get()
```

Defined in VirtualC64.h:1306

Getter for the raw disk object.

#### Returns
A pointer to the disk object or nullptr if no disk is present.

