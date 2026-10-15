# Zomlang

Zomlang is a custom programming language compiler built with LLVM. 

This project is deeply inspired by and closely follows the official [LLVM Kaleidoscope Tutorial](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html). It serves as a hands-on journey into learning compiler construction, lexing, parsing, and LLVM IR generation.

## Project Status: Work in Progress

The compiler is currently in active development and is **not yet complete**. I am progressively working through the Kaleidoscope chapters.

### Current Progress:
- [x] **Chapter 1:** Kaleidoscope Introduction and the Lexer
- [x] **Chapter 2:** Implementing a Parser and AST
- [x] **Chapter 3:** Code generation to LLVM IR
- [x] **Chapter 4:** Adding JIT and Optimizer Support
- [x] **Chapter 5:** Extending the Language: Control Flow (`if`/`then`/`else`)
- [ ] *Remaining chapters (User-defined operators, mutable variables, compiling to object code, etc.) are yet to be implemented.*

## 💻 What it can do

Thanks to the completion of Chapter 5, Zomlang currently supports functions, JIT compilation, optimization, and control flow. 

It can successfully parse and evaluate the following recursive Fibonacci sequence script:

```text
def fib(x)
    if x then
        if x - 1 then
            fib(x - 1) + fib(x - 2)
        else
            1
    else
        0;

fib(10)
```

<img width="1917" height="1077" alt="image" src="https://github.com/user-attachments/assets/40b8a5a5-c559-481e-8bd4-b6b82c025d36" />


## 🛠️ Environment & Setup

To ensure a smooth development process with LLVM dependencies, **I set up WSL (Windows Subsystem for Linux) to run and build LLVM**. 

If you are trying to build this project on a Windows machine, it is highly recommended to use WSL (Ubuntu) along with the standard Linux LLVM toolchain.
