# a64 - an assembler for c64 (and other things using 6502)


## Preprocessing

Preprocessing is handled via gasp, available through older
binutil packages. It is possible to disable it in ymain.c

This adds includes and macros. Example:

~~~
.include "x.s"

.macro woot a
 lda \a
.endmacro

 woot 2
~~~

It also requires that all instructions start at least one whitespace
from the left side, or they are converted to labels.

gasp is invoked with "alternate" syntax, giving access to the "local" keyword,
which enables labels within macros.

This means that the radix-translations available in gasp and other
features also are available.

## Syntax


The assembler is line-oriented. Several line-types exists:

### The instruction line

Contains an assembler instruction with arguments

### The pragma line

Contains a directive to the assembler. Implemented directives are

* `cpu <any-string>`	Not used. Just for compatibility.
* `org <expression>`        Start emitting code at `<expression>`. Can also
			be written as an assignment, `* = <expression>`
			Note: ORG _doesn't work_ when using gasp!
* `rorg <expression>`       Keep emitting code at current PC, but treat branches,
			labels etc as if the code is at `<expression>`
* `rend`			Stop relocated section and return to real PC

### Pseudoinstructions

* `.byte <space-separated list of bytes or strings> `
* `dc.b  <comma-separated list of bytes or strings>`

### Comments

A comment begins with ; and reaches to the end of the current line

### Labels

Any line may start with a label, on the format "<string>:"

### Expressions

* `<expr`		Lower eight bits of the expression
* `>expr`		Upper eight bits of the expression, shifted into a byte
* `expr+expr`	Addition
* `expr-expr`	Subtraction
* `.` 		Current PC

Default radix is decimal. Hex is indicated with `$` or `0x`

## Binary includes


You may include a binary file at current pc with pseudo-instruction "incbin".
It expects a quoted string as filename, and it is case-sensitive if the
platform is case-sensitive (eg unix).

example: 
~~~
incbin "MUSIC.BIN"
~~~

If you have a program file to include and want to
discard the load adress, use incprog instead.

## Code segments

Each changed PC results in a separate code segment file, which is
directly loadable into the C64. (The first two bytes are the load address)

The segments may be linked into a large file with padded holes using l64.
