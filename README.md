# Interfuse
- Programming language developed for fun. Its syntax its inspired on C, its based upon most modern programming languages and its currently under development.

Its core syntax as of now:

- Mutable Variable declaration:
```c

@sum : int; 
```

- Function declaration:
```c

fn @add(@a: int, @b: int) -> int {
    ret @a + @b;
};
```

- Mutable Variable assignment to function expression:
```c

@sum : int = @add(1, 2);
```

- Basic Logical Paths:
```c

@a : int;

if @sum {

    @a = 10;

} else {

    @a = 100;
};

```

- As of now it is also possible to print an expression to the standard output via the `@out` function
which is essentially a wrapper around the `printf` function in C.
```c
@out(@sum);
``` 

