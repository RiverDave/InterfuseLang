# Interfuse
- Programming language developed for fun. Its syntax its inspired on C, its based upon most modern programming languages and its currently under development.

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
- LLVM
- CLI11 (used for parsing command line arguments)
- emcc (Used in the shell script to compile the generated LLVM IR to WebAssembly)

## How to run(Tested on MacOS)

After cloning this repo:
```bash
mkdir build
cd build
cmake ..
make
```

To compile the program, you can use the following command:
```bash
./interfuse {path_to_file} {binary_name} # Will generate an executable with the specified binary name
```

- Flags:
```bash
-v # Verbose mode (prints the LLVM IR code)
-w # Wasm compilation (uses emcc to compile the generated LLVM IR to WebAssembly)

```


