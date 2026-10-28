---
generator: doxide
---


# C64Key

**`struct C64Key`**

Representation of a key on the C64 keyboard


## Variables

| Name | Description |
| ---- | ----------- |
| [nr](#nr) |  Unique key identifier (0 .. 65)  |
| [row](#row) |  Row index of this key in the keyboard matrix !!! note The RESTORE and the SHIFT LOCK key have no keyboard matrix representation.  |
| [col](#col) |  Column index of this key in the keyboard matrix !!! note The RESTORE and the SHIFT LOCK key have no keyboard matrix representation.  |
| [del](#del) | INST/DEL key  |
| [ret](#ret) | RETURN key  |
| [curLeftRight](#curLeftRight) | Cursor left/right key  |
| [F7F8](#F7F8) | F7/F8 key  |
| [F1F2](#F1F2) | F1/F2 key  |
| [F3F4](#F3F4) | F3/F4 key  |
| [F5F6](#F5F6) | F5/F6 key  |
| [curUpDown](#curUpDown) | Cursor up/down key  |
| [digit3](#digit3) | 3 key  |
| [W](#W) | Key W  |
| [A](#A) | Key A  |
| [digit4](#digit4) | 4 key  |
| [Z](#Z) | Key Z  |
| [S](#S) | Key S  |
| [E](#E) | Key E  |
| [leftShift](#leftShift) | Left SHIFT key  |
| [digit5](#digit5) | 5 key  |
| [R](#R) | Key R  |
| [D](#D) | Key D  |
| [digit6](#digit6) | 6 key  |
| [C](#C) | Key C  |
| [F](#F) | Key F  |
| [T](#T) | Key T  |
| [X](#X) | Key X  |
| [digit7](#digit7) | 7 key  |
| [Y](#Y) | Key Y  |
| [G](#G) | Key G  |
| [digit8](#digit8) | 8 key  |
| [B](#B) | Key B  |
| [H](#H) | Key H  |
| [U](#U) | Key U  |
| [V](#V) | Key V  |
| [digit9](#digit9) | 9 key  |
| [I](#I) | Key I  |
| [J](#J) | Key J  |
| [digit0](#digit0) | 0 key  |
| [M](#M) | Key M  |
| [K](#K) | Key K  |
| [O](#O) | Key O  |
| [N](#N) | Key N  |
| [plus](#plus) | + key  |
| [P](#P) | Key P  |
| [L](#L) | Key L  |
| [minus](#minus) | - key  |
| [period](#period) | . |
| [colon](#colon) | : key  |
| [at](#at) | @ key  |
| [comma](#comma) | , key  |
| [pound](#pound) | Pound sign key  |
| [asterisk](#asterisk) | * key  |
| [semicolon](#semicolon) | ; key  |
| [home](#home) | CLR/HOME key  |
| [rightShift](#rightShift) | Right SHIFT key  |
| [equal](#equal) | = key  |
| [upArrow](#upArrow) | Up arrow key  |
| [slash](#slash) | / key  |
| [digit1](#digit1) | 1 key  |
| [leftArrow](#leftArrow) | Left arrow key  |
| [control](#control) | CTRL key  |
| [digit2](#digit2) | 2 key  |
| [space](#space) | SPACE key  |
| [commodore](#commodore) | Commodore key key  |
| [Q](#Q) | Key Q  |
| [runStop](#runStop) | RUN/STOP key  |
| [restore](#restore) | RESTORE (not part of the keyboard matrix) key  |
| [shiftLock](#shiftLock) | SHIFT LOCK (not part of the keyboard matrix) key  |

## Functions

| Name | Description |
| ---- | ----------- |
| [C64Key](#C64Key) | Creates a key from its identifier (0 ... 65)  |
| [C64Key](#C64Key) | Creates a key from its position in the keyboard matrix  |
| [translate](#translate) |      Translates a character into a series of C64 keys. |

## Variable Details

### A<a name="A"></a>

!!! variable "static const C64Key A"
    
    Key A
    

### B<a name="B"></a>

!!! variable "static const C64Key B"
    
    Key B
    

### C<a name="C"></a>

!!! variable "static const C64Key C"
    
    Key C
    

### D<a name="D"></a>

!!! variable "static const C64Key D"
    
    Key D
    

### E<a name="E"></a>

!!! variable "static const C64Key E"
    
    Key E
    

### F<a name="F"></a>

!!! variable "static const C64Key F"
    
    Key F
    

### F1F2<a name="F1F2"></a>

!!! variable "static const C64Key F1F2"
    
    F1/F2 key
    

### F3F4<a name="F3F4"></a>

!!! variable "static const C64Key F3F4"
    
    F3/F4 key
    

### F5F6<a name="F5F6"></a>

!!! variable "static const C64Key F5F6"
    
    F5/F6 key
    

### F7F8<a name="F7F8"></a>

!!! variable "static const C64Key F7F8"
    
    F7/F8 key
    

### G<a name="G"></a>

!!! variable "static const C64Key G"
    
    Key G
    

### H<a name="H"></a>

!!! variable "static const C64Key H"
    
    Key H
    

### I<a name="I"></a>

!!! variable "static const C64Key I"
    
    Key I
    

### J<a name="J"></a>

!!! variable "static const C64Key J"
    
    Key J
    

### K<a name="K"></a>

!!! variable "static const C64Key K"
    
    Key K
    

### L<a name="L"></a>

!!! variable "static const C64Key L"
    
    Key L
    

### M<a name="M"></a>

!!! variable "static const C64Key M"
    
    Key M
    

### N<a name="N"></a>

!!! variable "static const C64Key N"
    
    Key N
    

### O<a name="O"></a>

!!! variable "static const C64Key O"
    
    Key O
    

### P<a name="P"></a>

!!! variable "static const C64Key P"
    
    Key P
    

### Q<a name="Q"></a>

!!! variable "static const C64Key Q"
    
    Key Q
    

### R<a name="R"></a>

!!! variable "static const C64Key R"
    
    Key R
    

### S<a name="S"></a>

!!! variable "static const C64Key S"
    
    Key S
    

### T<a name="T"></a>

!!! variable "static const C64Key T"
    
    Key T
    

### U<a name="U"></a>

!!! variable "static const C64Key U"
    
    Key U
    

### V<a name="V"></a>

!!! variable "static const C64Key V"
    
    Key V
    

### W<a name="W"></a>

!!! variable "static const C64Key W"
    
    Key W
    

### X<a name="X"></a>

!!! variable "static const C64Key X"
    
    Key X
    

### Y<a name="Y"></a>

!!! variable "static const C64Key Y"
    
    Key Y
    

### Z<a name="Z"></a>

!!! variable "static const C64Key Z"
    
    Key Z
    

### asterisk<a name="asterisk"></a>

!!! variable "static const C64Key asterisk"
    
    * key
    

### at<a name="at"></a>

!!! variable "static const C64Key at"
    
    @ key
    

### col<a name="col"></a>

!!! variable "isize col"
    
     Column index of this key in the keyboard matrix
     
    !!! note
       The RESTORE and the SHIFT LOCK key have no keyboard matrix
                 representation.
    

### colon<a name="colon"></a>

!!! variable "static const C64Key colon"
    
    : key
    

### comma<a name="comma"></a>

!!! variable "static const C64Key comma"
    
    , key
    

### commodore<a name="commodore"></a>

!!! variable "static const C64Key commodore"
    
    Commodore key key
    

### control<a name="control"></a>

!!! variable "static const C64Key control"
    
    CTRL key
    

### curLeftRight<a name="curLeftRight"></a>

!!! variable "static const C64Key curLeftRight"
    
    Cursor left/right key
    

### curUpDown<a name="curUpDown"></a>

!!! variable "static const C64Key curUpDown"
    
    Cursor up/down key
    

### del<a name="del"></a>

!!! variable "static const C64Key del"
    
    INST/DEL key
    

### digit0<a name="digit0"></a>

!!! variable "static const C64Key digit0"
    
    0 key
    

### digit1<a name="digit1"></a>

!!! variable "static const C64Key digit1"
    
    1 key
    

### digit2<a name="digit2"></a>

!!! variable "static const C64Key digit2"
    
    2 key
    

### digit3<a name="digit3"></a>

!!! variable "static const C64Key digit3"
    
    3 key
    

### digit4<a name="digit4"></a>

!!! variable "static const C64Key digit4"
    
    4 key
    

### digit5<a name="digit5"></a>

!!! variable "static const C64Key digit5"
    
    5 key
    

### digit6<a name="digit6"></a>

!!! variable "static const C64Key digit6"
    
    6 key
    

### digit7<a name="digit7"></a>

!!! variable "static const C64Key digit7"
    
    7 key
    

### digit8<a name="digit8"></a>

!!! variable "static const C64Key digit8"
    
    8 key
    

### digit9<a name="digit9"></a>

!!! variable "static const C64Key digit9"
    
    9 key
    

### equal<a name="equal"></a>

!!! variable "static const C64Key equal"
    
    = key
    

### home<a name="home"></a>

!!! variable "static const C64Key home"
    
    CLR/HOME key
    

### leftArrow<a name="leftArrow"></a>

!!! variable "static const C64Key leftArrow"
    
    Left arrow key
    

### leftShift<a name="leftShift"></a>

!!! variable "static const C64Key leftShift"
    
    Left SHIFT key
    

### minus<a name="minus"></a>

!!! variable "static const C64Key minus"
    
    - key
    

### nr<a name="nr"></a>

!!! variable "isize nr"
    
     Unique key identifier (0 .. 65)
    

### period<a name="period"></a>

!!! variable "static const C64Key period"
    
    . key
    

### plus<a name="plus"></a>

!!! variable "static const C64Key plus"
    
    + key
    

### pound<a name="pound"></a>

!!! variable "static const C64Key pound"
    
    Pound sign key
    

### restore<a name="restore"></a>

!!! variable "static const C64Key restore"
    
    RESTORE (not part of the keyboard matrix) key
    

### ret<a name="ret"></a>

!!! variable "static const C64Key ret"
    
    RETURN key
    

### rightShift<a name="rightShift"></a>

!!! variable "static const C64Key rightShift"
    
    Right SHIFT key
    

### row<a name="row"></a>

!!! variable "isize row"
    
     Row index of this key in the keyboard matrix
     
    !!! note
       The RESTORE and the SHIFT LOCK key have no keyboard matrix
                 representation.
    

### runStop<a name="runStop"></a>

!!! variable "static const C64Key runStop"
    
    RUN/STOP key
    

### semicolon<a name="semicolon"></a>

!!! variable "static const C64Key semicolon"
    
    ; key
    

### shiftLock<a name="shiftLock"></a>

!!! variable "static const C64Key shiftLock"
    
    SHIFT LOCK (not part of the keyboard matrix) key
    

### slash<a name="slash"></a>

!!! variable "static const C64Key slash"
    
    / key
    

### space<a name="space"></a>

!!! variable "static const C64Key space"
    
    SPACE key
    

### upArrow<a name="upArrow"></a>

!!! variable "static const C64Key upArrow"
    
    Up arrow key
    

## Function Details

### C64Key<a name="C64Key"></a>
!!! function "C64Key(isize nr = 0)"
    
    Creates a key from its identifier (0 ... 65)
    

!!! function "C64Key(isize row, isize col)"
    
    Creates a key from its position in the keyboard matrix
    

### translate<a name="translate"></a>
!!! function "static std::vector&lt;C64Key&gt; translate(char c)"
    
         Translates a character into a series of C64 keys.
     
    :material-keyboard-return: **Return**
    :        A vector of C64 keys.
     
    !!! note
           The return type is a vector as some characters additionally
                     require the shift key to be pressed.
    

