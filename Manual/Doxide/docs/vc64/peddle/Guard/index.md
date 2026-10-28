---
generator: doxide
---


# Guard

**`struct Guard`**

Base structure for a single breakpoint or watchpoint


## Variables

| Name | Description |
| ---- | ----------- |
| [addr](#addr) | The observed address  |
| [enabled](#enabled) | Disabled guards never trigger  |
| [hits](#hits) | Counts the number of hits  |
| [ignore](#ignore) | Ignore counter  |

## Functions

| Name | Description |
| ---- | ----------- |
| [eval](#eval) | Returns true if the guard hits  |
| [moveTo](#moveTo) | Replaces the address by another  |

## Variable Details

### addr<a name="addr"></a>

!!! variable "u32 addr"
    
    The observed address
    

### enabled<a name="enabled"></a>

!!! variable "bool enabled"
    
    Disabled guards never trigger
    

### hits<a name="hits"></a>

!!! variable "isize hits"
    
    Counts the number of hits
    

### ignore<a name="ignore"></a>

!!! variable "isize ignore"
    
    Ignore counter
    

## Function Details

### eval<a name="eval"></a>
!!! function "bool eval(u32 addr)"
    
    Returns true if the guard hits
    

### moveTo<a name="moveTo"></a>
!!! function "void moveTo(u32 newAddr)"
    
    Replaces the address by another
    

