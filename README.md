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

## 词法分析

`src/sysy.l`

## 语法分析

测试样例在 `test/syntax` 中

### 使用

> 使用命令 `make syntax3` 来测试 `test/syntax/test3.c`
> 
> 使用命令 `make syntax4` 来测试 `test/syntax/test4.c`

使用其他测试样例, 可以依次类推

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

1. 运行 `batch_compile.sh` 根据里面的提示, 批量将 `test/asm_test` 中的测试代码翻译成 x86 汇编代码, 并存放到 `out` 文件夹中
2. 运行 `compile.sh` 根据里面的提示, 将 `test/asm_test` 中指定的测试代码翻译成 x86 汇编并生成可执行文件. 生成结果同样存放在 `out` 文件夹中
