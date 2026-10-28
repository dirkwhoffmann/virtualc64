---
generator: doxide
---


# RemoteManagerAPI

**`class RemoteManagerAPI : public API`**

RemoteManager Public API


## Functions

| Name | Description |
| ---- | ----------- |
| [getInfo](#getInfo) | @name Analyzing the emulator @{ Returns the component's current state. |
| [getCachedInfo](#getCachedInfo) | Returns the most recent state snapshot recorded by the emulator thread (see getInfo()). |
| [send](#send) | @} @name Sending packets @{ Sends a raw payload through the specified remote server. |

## Function Details

### getCachedInfo<a name="getCachedInfo"></a>
!!! function "const RemoteManagerInfo &amp;getCachedInfo() const"
    
    Returns the most recent state snapshot recorded by the emulator thread (see getInfo()).
    

### getInfo<a name="getInfo"></a>
!!! function "const RemoteManagerInfo &amp;getInfo() const"
    
    @name Analyzing the emulator
    @{
     Returns the component's current state.
    
     getInfo() reads the live state on the calling thread. The result is only
     consistent if the emulator is paused or suspended. getCachedInfo()
     returns the snapshot the emulator thread has recorded most recently,
     which is the one to use while the emulator runs. Both return references
     that can be overwritten by a later call or by the emulator thread, so
     copy the data if you need to keep it.
    

### send<a name="send"></a>
!!! function "void send(ServerType server, const string &amp;payload)"
    
    @}
    @name Sending packets
    @{
     Sends a raw payload through the specified remote server.
     It is delivered through the server's currently configured transport and
     silently dropped if no client is connected.
     
    :material-location-enter: `server`
    :      The server to send through.
     
    :material-location-enter: `payload`
    :     The raw payload to send.
    

