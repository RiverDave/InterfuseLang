# Pourer/Mix
- Programming language developed for fun. Its syntax its based upon most modern programming languages and its currently under development.

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
