---
title: "C64Key"
---
# C64Key

```cpp
#include <Peripherals/Keyboard/C64Key.h>
```

```cpp
struct C64Key
```

Defined in Peripherals/Keyboard/C64Key.h:21

Representation of a key on the C64 keyboard

## Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `isize` | [`nr`](#nr-1)  | Unique key identifier (0 .. 65). |
| `isize` | [`row`](#row)  | Row index of this key in the keyboard matrix. |
| `isize` | [`col`](#col)  | Column index of this key in the keyboard matrix. |

---

### nr

```cpp
isize nr = -1
```

Defined in Peripherals/Keyboard/C64Key.h:25

Unique key identifier (0 .. 65).

---

### row

```cpp
isize row = -1
```

Defined in Peripherals/Keyboard/C64Key.h:31

Row index of this key in the keyboard matrix.

> [!NOTE]
> The RESTORE and the SHIFT LOCK key have no keyboard matrix representation.

---

### col

```cpp
isize col = -1
```

Defined in Peripherals/Keyboard/C64Key.h:37

Column index of this key in the keyboard matrix.

> [!NOTE]
> The RESTORE and the SHIFT LOCK key have no keyboard matrix representation.

## Public Methods

| Return | Name | Description |
|--------|------|-------------|
|  | [`C64Key`](#c64key-1)  | Creates a key from its identifier (0 ... 65). |
|  | [`C64Key`](#c64key-2)  | Creates a key from its position in the keyboard matrix. |

---

### C64Key

```cpp
C64Key(isize nr = 0)
```

Defined in Peripherals/Keyboard/C64Key.h:134

Creates a key from its identifier (0 ... 65).

---

### C64Key

```cpp
C64Key(isize row, isize col)
```

Defined in Peripherals/Keyboard/C64Key.h:136

Creates a key from its position in the keyboard matrix.

## Public Static Attributes

| Return | Name | Description |
|--------|------|-------------|
| const [`C64Key`](#c64key-1) | [`del`](#del) `static` | INST/DEL key. |
| const [`C64Key`](#c64key-1) | [`ret`](#ret) `static` | RETURN key. |
| const [`C64Key`](#c64key-1) | [`curLeftRight`](#curleftright) `static` | Cursor left/right key. |
| const [`C64Key`](#c64key-1) | [`F7F8`](#f7f8) `static` | F7/F8 key. |
| const [`C64Key`](#c64key-1) | [`F1F2`](#f1f2) `static` | F1/F2 key. |
| const [`C64Key`](#c64key-1) | [`F3F4`](#f3f4) `static` | F3/F4 key. |
| const [`C64Key`](#c64key-1) | [`F5F6`](#f5f6) `static` | F5/F6 key. |
| const [`C64Key`](#c64key-1) | [`curUpDown`](#curupdown) `static` | Cursor up/down key. |
| const [`C64Key`](#c64key-1) | [`digit3`](#digit3) `static` | 3 key |
| const [`C64Key`](#c64key-1) | [`W`](#w) `static` | Key W. |
| const [`C64Key`](#c64key-1) | [`A`](#a-1) `static` | Key A. |
| const [`C64Key`](#c64key-1) | [`digit4`](#digit4) `static` | 4 key |
| const [`C64Key`](#c64key-1) | [`Z`](#z) `static` | Key Z. |
| const [`C64Key`](#c64key-1) | [`S`](#s) `static` | Key S. |
| const [`C64Key`](#c64key-1) | [`E`](#e) `static` | Key E. |
| const [`C64Key`](#c64key-1) | [`leftShift`](#leftshift) `static` | Left SHIFT key. |
| const [`C64Key`](#c64key-1) | [`digit5`](#digit5) `static` | 5 key |
| const [`C64Key`](#c64key-1) | [`R`](#r) `static` | Key R. |
| const [`C64Key`](#c64key-1) | [`D`](#d) `static` | Key D. |
| const [`C64Key`](#c64key-1) | [`digit6`](#digit6) `static` | 6 key |
| const [`C64Key`](#c64key-1) | [`C`](#c) `static` | Key C. |
| const [`C64Key`](#c64key-1) | [`F`](#f) `static` | Key F. |
| const [`C64Key`](#c64key-1) | [`T`](#t) `static` | Key T. |
| const [`C64Key`](#c64key-1) | [`X`](#x-2) `static` | Key X. |
| const [`C64Key`](#c64key-1) | [`digit7`](#digit7) `static` | 7 key |
| const [`C64Key`](#c64key-1) | [`Y`](#y-2) `static` | Key Y. |
| const [`C64Key`](#c64key-1) | [`G`](#g) `static` | Key G. |
| const [`C64Key`](#c64key-1) | [`digit8`](#digit8) `static` | 8 key |
| const [`C64Key`](#c64key-1) | [`B`](#b) `static` | Key B. |
| const [`C64Key`](#c64key-1) | [`H`](#h) `static` | Key H. |
| const [`C64Key`](#c64key-1) | [`U`](#u) `static` | Key U. |
| const [`C64Key`](#c64key-1) | [`V`](#v) `static` | Key V. |
| const [`C64Key`](#c64key-1) | [`digit9`](#digit9) `static` | 9 key |
| const [`C64Key`](#c64key-1) | [`I`](#i) `static` | Key I. |
| const [`C64Key`](#c64key-1) | [`J`](#j) `static` | Key J. |
| const [`C64Key`](#c64key-1) | [`digit0`](#digit0) `static` | 0 key |
| const [`C64Key`](#c64key-1) | [`M`](#m) `static` | Key M. |
| const [`C64Key`](#c64key-1) | [`K`](#k) `static` | Key K. |
| const [`C64Key`](#c64key-1) | [`O`](#o) `static` | Key O. |
| const [`C64Key`](#c64key-1) | [`N`](#n) `static` | Key N. |
| const [`C64Key`](#c64key-1) | [`plus`](#plus) `static` | * key |
| const [`C64Key`](#c64key-1) | [`P`](#p) `static` | Key P. |
| const [`C64Key`](#c64key-1) | [`L`](#l) `static` | Key L. |
| const [`C64Key`](#c64key-1) | [`minus`](#minus) `static` | * key |
| const [`C64Key`](#c64key-1) | [`period`](#period) `static` | . key |
| const [`C64Key`](#c64key-1) | [`colon`](#colon) `static` | : key |
| const [`C64Key`](#c64key-1) | [`at`](#at) `static` | @ key |
| const [`C64Key`](#c64key-1) | [`comma`](#comma) `static` | , key |
| const [`C64Key`](#c64key-1) | [`pound`](#pound) `static` | Pound sign key. |
| const [`C64Key`](#c64key-1) | [`asterisk`](#asterisk) `static` | * key |
| const [`C64Key`](#c64key-1) | [`semicolon`](#semicolon) `static` | ; key |
| const [`C64Key`](#c64key-1) | [`home`](#home) `static` | CLR/HOME key. |
| const [`C64Key`](#c64key-1) | [`rightShift`](#rightshift) `static` | Right SHIFT key. |
| const [`C64Key`](#c64key-1) | [`equal`](#equal) `static` | = key |
| const [`C64Key`](#c64key-1) | [`upArrow`](#uparrow) `static` | Up arrow key. |
| const [`C64Key`](#c64key-1) | [`slash`](#slash) `static` | / key |
| const [`C64Key`](#c64key-1) | [`digit1`](#digit1) `static` | 1 key |
| const [`C64Key`](#c64key-1) | [`leftArrow`](#leftarrow) `static` | Left arrow key. |
| const [`C64Key`](#c64key-1) | [`control`](#control) `static` | CTRL key. |
| const [`C64Key`](#c64key-1) | [`digit2`](#digit2) `static` | 2 key |
| const [`C64Key`](#c64key-1) | [`space`](#space) `static` | SPACE key. |
| const [`C64Key`](#c64key-1) | [`commodore`](#commodore) `static` | Commodore key key. |
| const [`C64Key`](#c64key-1) | [`Q`](#q) `static` | Key Q. |
| const [`C64Key`](#c64key-1) | [`runStop`](#runstop) `static` | RUN/STOP key. |
| const [`C64Key`](#c64key-1) | [`restore`](#restore) `static` | RESTORE (not part of the keyboard matrix) key. |
| const [`C64Key`](#c64key-1) | [`shiftLock`](#shiftlock) `static` | SHIFT LOCK (not part of the keyboard matrix) key. |

---

### del

`static`

```cpp
const C64Key del
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:45

INST/DEL key.

---

### ret

`static`

```cpp
const C64Key ret
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:46

RETURN key.

---

### curLeftRight

`static`

```cpp
const C64Key curLeftRight
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:47

Cursor left/right key.

---

### F7F8

`static`

```cpp
const C64Key F7F8
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:48

F7/F8 key.

---

### F1F2

`static`

```cpp
const C64Key F1F2
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:49

F1/F2 key.

---

### F3F4

`static`

```cpp
const C64Key F3F4
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:50

F3/F4 key.

---

### F5F6

`static`

```cpp
const C64Key F5F6
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:51

F5/F6 key.

---

### curUpDown

`static`

```cpp
const C64Key curUpDown
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:52

Cursor up/down key.

---

### digit3

`static`

```cpp
const C64Key digit3
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:55

3 key

---

### W

`static`

```cpp
const C64Key W
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:56

Key W.

---

### A

`static`

```cpp
const C64Key A
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:57

Key A.

---

### digit4

`static`

```cpp
const C64Key digit4
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:58

4 key

---

### Z

`static`

```cpp
const C64Key Z
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:59

Key Z.

---

### S

`static`

```cpp
const C64Key S
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:60

Key S.

---

### E

`static`

```cpp
const C64Key E
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:61

Key E.

---

### leftShift

`static`

```cpp
const C64Key leftShift
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:62

Left SHIFT key.

---

### digit5

`static`

```cpp
const C64Key digit5
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:65

5 key

---

### R

`static`

```cpp
const C64Key R
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:66

Key R.

---

### D

`static`

```cpp
const C64Key D
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:67

Key D.

---

### digit6

`static`

```cpp
const C64Key digit6
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:68

6 key

---

### C

`static`

```cpp
const C64Key C
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:69

Key C.

---

### F

`static`

```cpp
const C64Key F
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:70

Key F.

---

### T

`static`

```cpp
const C64Key T
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:71

Key T.

---

### X

`static`

```cpp
const C64Key X
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:72

Key X.

---

### digit7

`static`

```cpp
const C64Key digit7
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:75

7 key

---

### Y

`static`

```cpp
const C64Key Y
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:76

Key Y.

---

### G

`static`

```cpp
const C64Key G
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:77

Key G.

---

### digit8

`static`

```cpp
const C64Key digit8
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:78

8 key

---

### B

`static`

```cpp
const C64Key B
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:79

Key B.

---

### H

`static`

```cpp
const C64Key H
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:80

Key H.

---

### U

`static`

```cpp
const C64Key U
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:81

Key U.

---

### V

`static`

```cpp
const C64Key V
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:82

Key V.

---

### digit9

`static`

```cpp
const C64Key digit9
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:85

9 key

---

### I

`static`

```cpp
const C64Key I
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:86

Key I.

---

### J

`static`

```cpp
const C64Key J
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:87

Key J.

---

### digit0

`static`

```cpp
const C64Key digit0
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:88

0 key

---

### M

`static`

```cpp
const C64Key M
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:89

Key M.

---

### K

`static`

```cpp
const C64Key K
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:90

Key K.

---

### O

`static`

```cpp
const C64Key O
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:91

Key O.

---

### N

`static`

```cpp
const C64Key N
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:92

Key N.

---

### plus

`static`

```cpp
const C64Key plus
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:95

* key

---

### P

`static`

```cpp
const C64Key P
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:96

Key P.

---

### L

`static`

```cpp
const C64Key L
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:97

Key L.

---

### minus

`static`

```cpp
const C64Key minus
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:98

* key

---

### period

`static`

```cpp
const C64Key period
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:99

. key

---

### colon

`static`

```cpp
const C64Key colon
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:100

: key

---

### at

`static`

```cpp
const C64Key at
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:101

@ key

---

### comma

`static`

```cpp
const C64Key comma
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:102

, key

---

### pound

`static`

```cpp
const C64Key pound
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:105

Pound sign key.

---

### asterisk

`static`

```cpp
const C64Key asterisk
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:106

* key

---

### semicolon

`static`

```cpp
const C64Key semicolon
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:107

; key

---

### home

`static`

```cpp
const C64Key home
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:108

CLR/HOME key.

---

### rightShift

`static`

```cpp
const C64Key rightShift
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:109

Right SHIFT key.

---

### equal

`static`

```cpp
const C64Key equal
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:110

= key

---

### upArrow

`static`

```cpp
const C64Key upArrow
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:111

Up arrow key.

---

### slash

`static`

```cpp
const C64Key slash
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:112

/ key

---

### digit1

`static`

```cpp
const C64Key digit1
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:115

1 key

---

### leftArrow

`static`

```cpp
const C64Key leftArrow
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:116

Left arrow key.

---

### control

`static`

```cpp
const C64Key control
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:117

CTRL key.

---

### digit2

`static`

```cpp
const C64Key digit2
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:118

2 key

---

### space

`static`

```cpp
const C64Key space
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:119

SPACE key.

---

### commodore

`static`

```cpp
const C64Key commodore
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:120

Commodore key key.

---

### Q

`static`

```cpp
const C64Key Q
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:121

Key Q.

---

### runStop

`static`

```cpp
const C64Key runStop
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:122

RUN/STOP key.

---

### restore

`static`

```cpp
const C64Key restore
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:125

RESTORE (not part of the keyboard matrix) key.

---

### shiftLock

`static`

```cpp
const C64Key shiftLock
```

Type: const [`C64Key`](#c64key-1)

Defined in Peripherals/Keyboard/C64Key.h:126

SHIFT LOCK (not part of the keyboard matrix) key.

## Public Static Methods

| Return | Name | Description |
|--------|------|-------------|
| std::vector< [`C64Key`](#c64key-1) > | [`translate`](#translate) `static` | Translates a character into a series of C64 keys. |

---

### translate

`static`

```cpp
static std::vector< C64Key > translate(char c)
```

Defined in Peripherals/Keyboard/C64Key.h:148

Translates a character into a series of C64 keys.

#### Returns
A vector of C64 keys. 

> [!NOTE]
> The return type is a vector as some characters additionally require the shift key to be pressed.

