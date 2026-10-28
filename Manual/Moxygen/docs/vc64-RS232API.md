---
title: "RS232API"
---
# RS232API

```cpp
#include <VirtualC64.h>
```

```cpp
class RS232API
```

Defined in VirtualC64.h:1106

RS232 Public [API](vc64-API.md#api)

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
| `void` | [`operator<<`](#operator)  | Returns the component's current state. |
| `void` | [`operator<<`](#operator-1)  | Feeds a string into the RS232 adapter. |
| `std::u16string` | [`readIncoming`](#readincoming)  | Read a printable byte from the incoming logbuffer. Calling this function empties the logbuffer. When the logbuffer is empty and a new data packet is received, a [Msg::RS232_IN](grp_messages.md#msg-1) message is sent to the GUI. |
| `std::u16string` | [`readOutgoing`](#readoutgoing)  | Read the logbuffer that records outgoing data. Calling this function empties the logbuffer. When the logbuffer is empty and a new data packet is sent, a [Msg::RS232_OUT](grp_messages.md#msg-1) message is sent to the GUI. |
| `int` | [`readIncomingPrintableByte`](#readincomingprintablebyte)  | Read a printable byte from the incoming logbuffer. |
| `int` | [`readOutgoingPrintableByte`](#readoutgoingprintablebyte)  | Read a printable byte from the outgoing logbuffer. |

---

### operator<<

```cpp
void operator<<(char c)
```

Defined in VirtualC64.h:1123

Returns the component's current state.

Feeds textual data into the RS232 adapter. This function emulates the transmission of incoming data from an external device.

---

### operator<<

```cpp
void operator<<(const string & s)
```

Defined in VirtualC64.h:1125

Feeds a string into the RS232 adapter.

---

### readIncoming

```cpp
std::u16string readIncoming()
```

Defined in VirtualC64.h:1132

Read a printable byte from the incoming logbuffer. Calling this function empties the logbuffer. When the logbuffer is empty and a new data packet is received, a [Msg::RS232_IN](grp_messages.md#msg-1) message is sent to the GUI.

---

### readOutgoing

```cpp
std::u16string readOutgoing()
```

Defined in VirtualC64.h:1139

Read the logbuffer that records outgoing data. Calling this function empties the logbuffer. When the logbuffer is empty and a new data packet is sent, a [Msg::RS232_OUT](grp_messages.md#msg-1) message is sent to the GUI.

---

### readIncomingPrintableByte

```cpp
int readIncomingPrintableByte()
```

Defined in VirtualC64.h:1143

Read a printable byte from the incoming logbuffer.

---

### readOutgoingPrintableByte

```cpp
int readOutgoingPrintableByte()
```

Defined in VirtualC64.h:1147

Read a printable byte from the outgoing logbuffer.

