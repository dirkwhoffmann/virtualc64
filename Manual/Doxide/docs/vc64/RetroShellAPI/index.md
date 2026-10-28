---
generator: doxide
---


# RetroShellAPI

**`class RetroShellAPI : public API`**

@}
RetroShell Public API


## Functions

| Name | Description |
| ---- | ----------- |
| [getInfo](#getInfo) | @name Querying the console @{ Returns the component's current state. |
| [getCachedInfo](#getCachedInfo) | Returns the most recent state snapshot recorded by the emulator thread (see getInfo()). |
| [text](#text) |  Returns a copy of the text buffer. |
| [press](#press) |  Returns the relative cursor position. |
| [press](#press) |  Informs RetroShell that a key has been typed. |
| [press](#press) |  Informs RetroShell that multiple keys have been typed. |
| [setStream](#setStream) | @} @name Controlling the output stream @{ Assign an additional output stream. |
| [execScriptAsync](#execScriptAsync) | @} @name Executing scripts @{ Executes a script. |
| [execScriptAsync](#execScriptAsync) | Executes a script that is read from a string stream. |
| [execScriptAsync](#execScriptAsync) | Executes a script that is read from a file stream. |
| [execScriptAsync](#execScriptAsync) | Executes a script that is given as a string. |

## Function Details

### execScriptAsync<a name="execScriptAsync"></a>
!!! function "void execScriptAsync(const std::filesystem::path &amp;path)"
    
    @}
    @name Executing scripts
    @{
     Executes a script.
     The script is executed asynchronously. However, RetroShell will
     send messages back to the GUI thread to inform about the execution
     state. After the last pending command has been executed,
     Msg::RSH_DONE is sent. If a command fails, the remaining commands are
     dropped and Msg::RSH_ERROR is sent instead. A 'wait' command postpones
     the rest of the script and sends Msg::RSH_WAIT.
    
     
    :material-alert-circle-outline: **Throw**
    :    Error if the file cannot be opened.
    

!!! function "void execScriptAsync(std::stringstream &amp;ss)"
    
    Executes a script that is read from a string stream.
    

!!! function "void execScriptAsync(const std::ifstream &amp;fs)"
    
    Executes a script that is read from a file stream.
    

!!! function "void execScriptAsync(const string &amp;contents)"
    
    Executes a script that is given as a string.
    

### getCachedInfo<a name="getCachedInfo"></a>
!!! function "const RetroShellInfo &amp;getCachedInfo() const"
    
    Returns the most recent state snapshot recorded by the emulator thread (see getInfo()).
    

### getInfo<a name="getInfo"></a>
!!! function "const RetroShellInfo &amp;getInfo() const"
    
    @name Querying the console
    @{
     Returns the component's current state.
    
     getInfo() reads the live state on the calling thread. The result is only
     consistent if the emulator is paused or suspended. getCachedInfo()
     returns the snapshot the emulator thread has recorded most recently,
     which is the one to use while the emulator runs. Both return references
     that can be overwritten by a later call or by the emulator thread, so
     copy the data if you need to keep it.
    

### press<a name="press"></a>
!!! function "void press(RSKey key, bool shift = false)"
    
     Returns the relative cursor position.
     The returned value is relative to the end of the input line. A value
     of 0 indicates that the cursor is at the rightmost position, that
     is, one character after the last character of the input line. If the
     cursor is at the front of the input line, the value matches the
     length of the input line.
    @}
    @name Typing characters and strings
    @{
     Informs RetroShell that a key has been typed.
     
    :material-location-enter: `key`
    :        The pressed key
     
    :material-location-enter: `shift`
    :      Status of the shift key
    

!!! function "void press(char c)"
    
     Informs RetroShell that a key has been typed.
     
    :material-location-enter: `c`
    :          The pressed key
    

!!! function "void press(const string &amp;s)"
    
     Informs RetroShell that multiple keys have been typed.
     
    :material-location-enter: `s`
    :          The typed text
    

### setStream<a name="setStream"></a>
!!! function "void setStream(std::ostream &amp;os)"
    
    @}
    @name Controlling the output stream
    @{
     Assign an additional output stream.
     In addition to writing the RetroShell output into the text buffer,
     RetroShell will write the output into the provides stream.
    

### text<a name="text"></a>
!!! function "string text() const"
    
     Returns a copy of the text buffer.
     The text buffer contains the complete contents of the console. It
     will be expanded when new output is generated. When the buffer
     grows too large, old contents is cropped.
    

