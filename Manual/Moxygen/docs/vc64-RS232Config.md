---
title: "RS232Config"
---
# RS232Config

```cpp
#include <Peripherals/Network/RS232Types.h>
```

```cpp
struct RS232Config
```

Defined in Peripherals/Network/RS232Types.h:62

Configuration of the RS232 adapter.

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| [`CommunicationDevice`](grp_userserial.md#communicationdevice) | [`device`](#device)  | Connected RS232 device. |
| `isize` | [`baud`](#baud)  | Transmission rate (baud). |

---

### device

```cpp
CommunicationDevice device
```

Type: [`CommunicationDevice`](grp_userserial.md#communicationdevice)

Defined in Peripherals/Network/RS232Types.h:64

Connected RS232 device.

---

### baud

```cpp
isize baud
```

Defined in Peripherals/Network/RS232Types.h:65

Transmission rate (baud).

