# Monkey Programming Language Interpreter

A tree-walking interpreter for the Monkey programming language, implemented in C++.

## About

This project is a complete implementation of the Monkey programming language interpreter, following the book [**Writing An Interpreter In Go**](https://interpreterbook.com/) by Thorsten Ball. While the original book uses Go, this implementation is written in **C++17** with modern practices and memory management.

## Example Program

Save this as `test.monkey`:

```monkey
puts("Running Fibonacci")
let fibonacci = fn(x) {
    if (x == 0) {
        0
    } else {
        if (x == 1) {
            return 1;
        } else {
            fibonacci(x - 1) + fibonacci(x - 2);
        }
    }
};
fibonacci(10);

puts("Running Execution Of Arrays")
let map = fn(arr, f) {
    let iter = fn(arr, accumulated) {
        if (len(arr) == 0) {
            accumulated
        } else {
            iter(rest(arr), push(accumulated, f(first(arr))));
        }
    };
    iter(arr, []);
};

let a = [1, 2, 3, 4];
let double = fn(x) { x * 2 };
map(a, double);

let reduce = fn(arr, initial, f) {
    let iter = fn(arr, result) {
        if (len(arr) == 0) {
            result
        } else {
            iter(rest(arr), f(result, first(arr)));
        }
    };
    iter(arr, initial);
};

let sum = fn(arr) {
    reduce(arr, 0, fn(initial, el) { initial + el });
};
sum([1, 2, 3, 4, 5]);

puts("Running HashLiterals")
let people = [{"name": "Alice", "age": 24}, {"name": "Anna", "age": 28}];
people[0]["name"];
people[1]["age"];
people[1]["age"] + people[0]["age"];
let getName = fn(person) { person["name"]; };
getName(people[0]);
getName(people[1]);

puts("Running Function Closures")
let newAdder = fn(x) {
    fn(y) {
        x + y
    }
};
let addTwo = newAdder(2);
addTwo(3);
let addThree = newAdder(3);
addThree(10);
```

**Output:**
```
Running Fibonacci
55
Running Execution Of Arrays
[2, 4, 6, 8]
15
Running HashLiterals
Alice
28
52
Alice
Anna
Running Function Closures
5
13
```

## Learning Resources

- **Original Book:** [Writing An Interpreter In Go](https://interpreterbook.com/) by Thorsten Ball
- **Sequel:** [Writing A Compiler In Go](https://compilerbook.com/)

## Acknowledgments

Special thanks to **Thorsten Ball** for writing the excellent book that served as the foundation for this project.

## License

This project is created for educational purposes.
