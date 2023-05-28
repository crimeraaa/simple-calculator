# calcu.c, a simple command-line calculator

*Perform a single math operation on 2 numbers.*

Usage: 

```
./calcu num1 operand num2
```

Help:

```
./calcu --help
```

> You can also use `--h`, `help` and `h`.

## The Numbers

`num1` and `num2` must be real numbers. They can be either integers or double-precision floats. 

They can also be negative! Just fix an unary negation (`-`) before your chosen number, like `-1.3`.

Parentheses aren't supported yet sadly.

## The Operators

`operand` must be a single character representing the operation you want to perform on these two numbers.

| Symbol | Operation      | Note                           |
|--------|----------------|--------------------------------|
| `+`    | Addition       |                                |
| `-`    | Subtraction    |                                |
| `x`    | Multiplication | Don't use `*`                  |
| `/`    | Division       |                                |
| `%`    | Modulo         | `double` will be cast to `int` |
| `^`    | Exponent       |                                |

# Building

This is literally just a single C file, so as long as you have a C compiler you should be able to build this.

For example, I use gcc.

```
gcc ./calcu.c -o ./calcu.exe
```