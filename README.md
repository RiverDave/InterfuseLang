# Interfuse

Programming language developed for fun & with the intention of learning compiler theory!. Its syntax its inspired on C, its based upon most modern programming languages and its currently under development.

## Installation(Tested on MacOS)

Build the project using the following commands:

```bash
# Make sure to have LLVM installed on your system. It may be different
# depending on your OS.
mkdir build
cd build
cmake ..
make
```

To compile the program, you can use the following command:

```bash
./interfuse [path_to_file] [flags] [binary_name] # Will generate an executable with the specified binary name
```

- Flags:

```bash
-v # Verbose mode (prints the LLVM IR code & explicitly display lexing process)
-o # Output file name
-d # Dump LLVM IR to a file
```

## Examples

Its core syntax as of now:

- Mutable Variable declaration:

```c

@sum : int
```

- Function declaration:

```c

fn @add(@a: int, @b: int) -> int {
    ret @a + @b
}
```

- Mutable Variable assignment to function expression:

```c

@sum : int = @add(1, 2)
```

- Basic Logical Paths:

```c

@a : int

if @sum {

    @a = 10

} else {

    @a = 100
};

```

- As of now it is also possible to print an expression to the standard output via either the printf function or puts function which are wrappers around their original C counterparts:

```c
@printf(@sum)
# or
@puts("Hello, World!")
```

- For loops :

```c

fn @my_fn() -> void {

    for @i in @i < 100 : @i + 1 {
       @out(@i)
    }

}

```

- A more complex function:

```rust

fn @power(@base : int, @exponent : int) -> int {
  @result : int = 1
  for @i in @i <= @exponent : @i + 1 {
    @result = @result * @base
  }
  ret @result
}

```

## Dependencies

[LLVM](https://llvm.org/)

## Libraries & Techniques used

Interfuse is written purely in C++17. It uses the following libraries:

- **[CLI11](https://github.com/CLIUtils/CLI11)**: Used to parse command line arguments.
- **[GNU Bison](https://github.com/akimd/bison)**: Employed uniquely for the parsing process.
- **[LLVM](https://llvm.org/)**: LLVM Represents the Core and most fundamental part of the compiler. It is used to generate the intermediate representation of the code(machine code.

## Motivation

Interfuse was created with the goal of challenging myself to create a challenging programming project of larger scale. This project has been the effort of months of research and development, and it has been a great self learning experience.

## License
Interfuse is licensed under the GNU General Public License. See the LICENSE file for more information.

