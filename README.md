# SysY-Compiler

**SysY** 语言编译器

## 环境

```shell
Ubuntu 20.04
flex
bison
gcc
as (GNU assembler)
```

## Build

```shell
$ make all

# for clean
$ make clean
```


### 使用

> 使用命令 `cd lexer` 来到 `lexer` 文件夹中
> 
> 使用命令 `make test3` 来测试 `test/test3.c`
> 
> 使用命令 `make test4` 来测试 `test/test4.c`

使用其他测试样例, 可以依次类推

## 语法分析

测试样例在 `test/syntax` 中

### 使用

**注意:** 若想得到语法树(AST), 则要取消 `src/main.cpp` 第一行对 [AST](./src/main.cpp#1) 的注释, 否则输出的将会是汇编代码而不是(AST). 

> 使用命令 `make syntax3` 来测试 `test/syntax/test3.c`
> 
> 使用命令 `make syntax4` 来测试 `test/syntax/test4.c`

使用其他测试样例, 可以依次类推. 若用类似上面的测试命令, 语法树(AST)将会默认输出到 `result.txt` 中. 

## 语义分析

测试样例在 `test/semantic` 中

### 使用

> 使用命令 `make check3` 来测试 `test/semantic/test3.c`
>
> 使用命令 `make check4` 来测试 `test/semantic/test4.c`

使用其他测试样例, 可以依次类推

## 代码生成

测试样例在 `test/asm_test` 中

### 使用

**注意:** 要对 `src/main.cpp` 第一行的宏 [AST](./src/main.cpp#1) 进行注释, 才能生成汇编代码. 

1. 运行 `batch_compile.sh` 根据里面的提示, 批量将 `test/asm_test` 中的测试代码翻译成 x86 汇编代码, 并存放到 `out` 文件夹中
2. 运行 `compile.sh` 根据里面的提示, 将 `test/asm_test` 中指定的测试代码翻译成 x86 汇编并生成可执行文件. 生成结果同样存放在 `out` 文件夹中
