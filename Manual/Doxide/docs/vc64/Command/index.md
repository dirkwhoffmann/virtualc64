---
generator: doxide
---


# Command

**`struct Command`**

A command that is fed into the command queue


## Variables

| Name | Description |
| ---- | ----------- |
| [type](#type) | Type of the command  |
| [sender](#sender) | Sender of the command (optional)  |
| [value](#value) | Command payload  |
| [value2](#value2) | Generic payload  |
| [alarm](#alarm) | Payload of alarm commands  |
| [config](#config) | Payload of configuration commands  |
| [coord](#coord) | Payload of mouse commands  |
| [action](#action) | Payload of joystick commands  |
| [guard](#guard) | Payload of breakpoint and watchpoint commands  |
| [joyState](#joyState) | Payload of joystick state commands  |
| [key](#key) | Payload of keyboard commands  |
| [tape](#tape) | Payload of tape commands  |
| [str](#str) | Attached string (may be null)  |

## Functions

| Name | Description |
| ---- | ----------- |
| [Command](#Command) | Creates an empty command  |
| [Command](#Command) | Creates a command with two integer values  |
| [Command](#Command) | Creates a command with a sender  |
| [Command](#Command) | Creates a command with an alarm payload  |
| [Command](#Command) | Creates a command with a configuration payload  |
| [Command](#Command) | Creates a command with a coordinate payload  |
| [Command](#Command) | Creates a command with a joystick payload  |
| [Command](#Command) | Creates a command with a guard payload  |
| [Command](#Command) | Creates a command with a joystick state payload  |
| [Command](#Command) | Creates a command with a keyboard payload  |
| [Command](#Command) | Creates a command with a tape payload  |

## Variable Details

### action<a name="action"></a>

!!! variable "GamePadCmd action"
    
    Payload of joystick commands
    

### alarm<a name="alarm"></a>

!!! variable "AlarmCmd alarm"
    
    Payload of alarm commands
    

### config<a name="config"></a>

!!! variable "ConfigCmd config"
    
    Payload of configuration commands
    

### coord<a name="coord"></a>

!!! variable "CoordCmd coord"
    
    Payload of mouse commands
    

### guard<a name="guard"></a>

!!! variable "GuardCmd guard"
    
    Payload of breakpoint and watchpoint commands
    

### joyState<a name="joyState"></a>

!!! variable "JoyStateCmd joyState"
    
    Payload of joystick state commands
    

### key<a name="key"></a>

!!! variable "KeyCmd key"
    
    Payload of keyboard commands
    

### sender<a name="sender"></a>

!!! variable "void &#42;sender"
    
    Sender of the command (optional)
    

### str<a name="str"></a>

!!! variable "const char &#42;str"
    
    Attached string (may be null)
    

### tape<a name="tape"></a>

!!! variable "TapeCmd tape"
    
    Payload of tape commands
    

### type<a name="type"></a>

!!! variable "Cmd type"
    
    Type of the command
    

### value<a name="value"></a>

!!! variable "i64 value"
    
    Command payload
    

### value2<a name="value2"></a>

!!! variable "i64 value2"
    
    Generic payload
    

## Function Details

### Command<a name="Command"></a>
!!! function "Command()"
    
    Creates an empty command
    

!!! function "Command(Cmd type, i64 v1 = 0, i64 v2 = 0)"
    
    Creates a command with two integer values
    

!!! function "Command(Cmd type, void &#42;s, i64 v1 = 0, i64 v2 = 0)"
    
    Creates a command with a sender
    

!!! function "Command(Cmd type, const AlarmCmd &amp;cmd)"
    
    Creates a command with an alarm payload
    

!!! function "Command(Cmd type, const ConfigCmd &amp;cmd)"
    
    Creates a command with a configuration payload
    

!!! function "Command(Cmd type, const CoordCmd &amp;cmd)"
    
    Creates a command with a coordinate payload
    

!!! function "Command(Cmd type, const GamePadCmd &amp;cmd)"
    
    Creates a command with a joystick payload
    

!!! function "Command(Cmd type, const GuardCmd &amp;cmd)"
    
    Creates a command with a guard payload
    

!!! function "Command(Cmd type, const JoyStateCmd &amp;cmd)"
    
    Creates a command with a joystick state payload
    

!!! function "Command(Cmd type, const KeyCmd &amp;cmd)"
    
    Creates a command with a keyboard payload
    

!!! function "Command(Cmd type, const TapeCmd &amp;cmd)"
    
    Creates a command with a tape payload
    

