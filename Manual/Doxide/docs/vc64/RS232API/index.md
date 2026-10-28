---
generator: doxide
---


# RS232API

**`class RS232API : public API`**

RS232 Public API


## Operators

| Name | Description |
| ---- | ----------- |
| [operator<<](#operator_u003c_u003c) |  Returns the component's current state. |
| [operator<<](#operator_u003c_u003c) | Feeds a string into the RS232 adapter. |

## Functions

| Name | Description |
| ---- | ----------- |
| [readIncoming](#readIncoming) |  Read a printable byte from the incoming logbuffer. |
| [readOutgoing](#readOutgoing) |  Read the logbuffer that records outgoing data. |
| [readIncomingPrintableByte](#readIncomingPrintableByte) |  Read a printable byte from the incoming logbuffer. |
| [readOutgoingPrintableByte](#readOutgoingPrintableByte) |  Read a printable byte from the outgoing logbuffer. |

## Operator Details

### operator<<<a name="operator_u003c_u003c"></a>

!!! function "void operator&lt;&lt;(char c)"
    
     Returns the component's current state.
     Feeds textual data into the RS232 adapter.
     This function emulates the transmission of incoming data from an
     external device.
    

!!! function "void operator&lt;&lt;(const string &amp;s)"
    
    Feeds a string into the RS232 adapter.
    

## Function Details

### readIncoming<a name="readIncoming"></a>
!!! function "std::u16string readIncoming()"
    
     Read a printable byte from the incoming logbuffer.
     Calling this function empties the logbuffer. When the logbuffer is
     empty and a new data packet is received, a Msg::RS232_IN message is
     sent to the GUI.
    

### readIncomingPrintableByte<a name="readIncomingPrintableByte"></a>
!!! function "int readIncomingPrintableByte()"
    
     Read a printable byte from the incoming logbuffer.
    

### readOutgoing<a name="readOutgoing"></a>
!!! function "std::u16string readOutgoing()"
    
     Read the logbuffer that records outgoing data.
     Calling this function empties the logbuffer. When the logbuffer is
     empty and a new data packet is sent, a Msg::RS232_OUT message is
     sent to the GUI.
    

### readOutgoingPrintableByte<a name="readOutgoingPrintableByte"></a>
!!! function "int readOutgoingPrintableByte()"
    
     Read a printable byte from the outgoing logbuffer.
    

