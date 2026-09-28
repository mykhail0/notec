# notec

Noteć szesnastkowy. Concurrent calculator of reverse Polish notation written in
assembly.

## Specification

This project implements in x86_64 assembly a module for concurrent calculator on
64-bit numbers, written in base 16, using reverse Polish notation. It is
possible to launch N concurrent instances of notec, indexed from 0 to N - 1,
where N is a compilation parameter. Each notec instance can be called from C in
a separate thread using the function:

```c
uint64_t notec(uint32_t n, char const *calc);
```

`n` argument contains the index of notec instance. `calc` is an ASCIIZ string
which describes the calculation to be performed by notec. A calculation consists
of operations performed on a stack, which is empty at first. String's characters
are interpreted as follows:

- `0 to 9, A to F, a to f` – the character is interpreted as a digit in base 16.
if notec is in the input mode, then the number at the top of the stack is
shifted one position left and the given digit goes to the least significant
position. If notec is not in the input mode, then the value of the given digit
is put on the stack. Notec enters input mode when meeting characters from this
group and exits input mode when encountering any character outside of this
group.
- `=` – exit input mode.
- `+` – pop two values from the stack, compute their sum and put it on the
stack.
- `*` – pop two values from the stack, compute their product and put it on the
stack.
- `-` – negate arithmetically the value on top of the stack.
- `&` – pop two values from the stack, compute their AND and put it on the
stack.
- `|` – pop two values from the stack, compute their OR and put it on the stack.
- `^` – pop two values from the stack, compute their XOR and put it on the
stack.
- `~` – negate bits of the value on top of the stack.
- `Z` – pop a value from the stack.
- `Y` – put a value on the stack, which is the current top of the stack, in
other words duplicate the value on top of the stack.
- `X` – swap two values from the top of the stack with each other.
- `N` – put the number of notecs on the stack.
- `n` – put the index of this notec instance on the stack.
- `W` – pop a value from the stack, treat it as an index of notec instance `m`.
Wait until operation `W` is performed by notec `m` such that `n` was popped and
swap values on top of stacks `m` and `n`.
- `g` – call (implemented somewhere in either C or Assembly) the following
function:

```c
int64_t debug(uint32_t n, uint64_t *stack_pointer);
```

`n` parameter is the index of a notec instance calling this function.
`stack_pointer` parameter points to the top of the stack. `debug` function may
modify the stack. The return value of the function shows by how many positions
the top of the stack should be shifted afterwards.

After notec finishes execution of `notec`, the return value is the value from
the top of the stack. All operations are performed on 64-bit numbers modulo
2^64. A calculation is correct if it consists only of the characters described
above, is null-terminated (string ends with a zero byte), does not reach for a
value from the stack if it is empty and does not deadlock. Notec behavior for
incorrect calculations is undefined.

## Example

Example usage is in this [file](src/example.c). A [makefile](makefile) is
supplied with a default target which compiles the example, clean target and a
test target. The number of notec instances `N` must be given like so:

```bash
make target N=10
```
