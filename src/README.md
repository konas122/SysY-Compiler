## 说明

本文件夹存放的是实现编译器的代码, 可进行**语法分析**、**语义分析**和**代码生成**的测试. 

### 注意
- 若想得到语法树(AST), 则要取消 [main.cpp](./main.cpp#1) 第一行对 [AST](./main.cpp#1) 的注释, 否则输出的将会是汇编代码而不是(AST). 
  > 若用 `makefile` 中的测试命令, 语法树(AST)将会默认输出到 `result.txt` 中.
- 若想得到汇编代码, 则要对 [main.cpp](./main.cpp#1) 第一行的宏 [AST](./main.cpp#1) 进行注释. 
