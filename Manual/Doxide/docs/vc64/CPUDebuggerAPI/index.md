---
generator: doxide
---


# CPUDebuggerAPI

**`class CPUDebuggerAPI : public API`**

CPU Debugger API


## Functions

| Name | Description |
| ---- | ----------- |
| [loggedInstructions](#loggedInstructions) |  Returns the number of instructions in the record buffer. |
| [clearLog](#clearLog) |  Empties the record buffer. |
| [logEntry](#logEntry) | Returns an entry from the record buffer. |
| [setNumberFormat](#setNumberFormat) | Determines how the disassembler displays numbers. |
| [disassemble](#disassemble) | Disassembles an instruction. |
| [disassembleRecorded](#disassembleRecorded) | Disassembles an instruction from the record buffer. |

## Function Details

### clearLog<a name="clearLog"></a>
!!! function "void clearLog()"
    
     Empties the record buffer.
    

### disassemble<a name="disassemble"></a>
!!! function "string disassemble(u16 addr, const string &amp;fmt, isize &#42;len = nullptr) const"
    
    Disassembles an instruction.
     
    :material-location-enter: `addr`
    :       Address of the instruction to disassemble
     
    :material-location-enter: `fmt`
    :        String defining the output format. The directives are
                     %p (address), %b (instruction bytes), %i (instruction),
                     and %f (flags, recorded instructions only).
     
    :material-location-enter: `len`
    :        If given, receives the length of the instruction in bytes.
     
    :material-keyboard-return: **Return**
    :    The disassembled text.
    

### disassembleRecorded<a name="disassembleRecorded"></a>
!!! function "string disassembleRecorded(isize nr, const string &amp;fmt, isize &#42;len = nullptr) const"
    
    Disassembles an instruction from the record buffer.
     
    :material-location-enter: `nr`
    :         Index of the instruction in the record buffer
     
    :material-location-enter: `fmt`
    :        String defining the output format (see disassemble())
     
    :material-location-enter: `len`
    :        If given, receives the length of the instruction in bytes.
     
    :material-keyboard-return: **Return**
    :    The disassembled text.
    

### logEntry<a name="logEntry"></a>
!!! function "RecordedInstruction logEntry(isize nr) const"
    
    Returns an entry from the record buffer. Returned by value (rather
     than by reference) since the log buffer is written concurrently by
     the emulator thread.
     
    :material-location-enter: `nr`
    :         Index of the instruction in the record buffer
     
    :material-keyboard-return: **Return**
    :    The recorded instruction, including the address (pc) it was
             fetched from.
    

### loggedInstructions<a name="loggedInstructions"></a>
!!! function "isize loggedInstructions() const"
    
     Returns the number of instructions in the record buffer.
     
    !!! note
       The record buffer is only filled in track mode. To save
                 computation time, the GUI enables track mode when the CPU
                 inspector is opened and disables track mode when it is
                 closed.
    

### setNumberFormat<a name="setNumberFormat"></a>
!!! function "void setNumberFormat(DasmNumberFormat instrFormat, DasmNumberFormat dataFormat)"
    
    Determines how the disassembler displays numbers.
     
    :material-location-enter: `instrFormat`
    :    Format for numbers inside instructions
     
    :material-location-enter: `dataFormat`
    :     Format for printed data values
    

