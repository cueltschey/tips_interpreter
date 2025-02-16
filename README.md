## TIPS Interpreter

This program reads and executes Ten Instruction [Pascal](https://en.wikipedia.org/wiki/Pascal_\(programming_language\)) Subset (TIPS). It uses flex for lexing and C/C++ for parsing and execution. I developed this as part of a class for Mississippi State University, CSE 4714 theory and implementation of programming languages. All of the code in this repository is original, and not provided by Mississippi State.

## Building and Running

To build the interpreter use make:

```bash
git clone https://github.com/cueltschey/tips_interpreter
cd tips_interpreter
make tips
```

To run using the test program:
```bash
./tips test.pas
```

## Arguments

**-s**: Shows the symbol table
**-p**: Prints output while parsing
**-t**: Shows program syntax tree
