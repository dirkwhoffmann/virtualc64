---
generator: doxide
---


# InputLine

**`struct InputLine`**

A line of input for RetroShell


## Types

| Name | Description |
| ---- | ----------- |
| [Source](Source/) | Origin of an input line  |

## Type Aliases

| Name | Description |
| ---- | ----------- |
| [Completion](#Completion) | Signature of the completion handler  |

## Variables

| Name | Description |
| ---- | ----------- |
| [id](#id) | Line number, RPC identifier, etc. |
| [type](#type) | Indicates where the command comes from  |
| [input](#input) | The command to execute  |
| [completion](#completion) | Optional completion handler  |

## Functions

| Name | Description |
| ---- | ----------- |
| [isUserCommand](#isUserCommand) | Returns true if the line was typed by the user  |
| [isScriptCommand](#isScriptCommand) | Returns true if the line stems from a script  |
| [isRpcCommand](#isRpcCommand) | Returns true if the line is a JSON RPC request  |
| [isRshCommand](#isRshCommand) | Returns true if the line is a remote shell request  |

## Type Alias Details

### Completion<a name="Completion"></a>

!!! typedef "using Completion = std::function&lt;void(const string &amp;output, std::exception_ptr error)&gt;"
    
    Signature of the completion handler
    

## Variable Details

### completion<a name="completion"></a>

!!! variable "Completion completion"
    
    Optional completion handler
    

### id<a name="id"></a>

!!! variable "isize id"
    
    Line number, RPC identifier, etc.
    

### input<a name="input"></a>

!!! variable "string input"
    
    The command to execute
    

### type<a name="type"></a>

!!! variable "Source type"
    
    Indicates where the command comes from
    

## Function Details

### isRpcCommand<a name="isRpcCommand"></a>
!!! function "bool isRpcCommand() const"
    
    Returns true if the line is a JSON RPC request
    

### isRshCommand<a name="isRshCommand"></a>
!!! function "bool isRshCommand() const"
    
    Returns true if the line is a remote shell request
    

### isScriptCommand<a name="isScriptCommand"></a>
!!! function "bool isScriptCommand() const"
    
    Returns true if the line stems from a script
    

### isUserCommand<a name="isUserCommand"></a>
!!! function "bool isUserCommand() const"
    
    Returns true if the line was typed by the user
    

