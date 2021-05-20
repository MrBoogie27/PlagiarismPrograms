# PlagiarismPrograms
This is a tool to compare two C++ programs for plagiarism

## About library

Library documentation is [here](https://clang.llvm.org/docs/LibTooling.html).

I used clang version 10.

## Build tool
```
cmake --build /mnt/d/MIPT/PlagiarismPrograms/cmake-build-debug --target print_AST_tool -- -j 6
```
## Run print

Output AST: 
```clang Test/example.cpp -I. -Xclang -ast-dump -fsyntax-only``` 

My Output:
```print_AST_tool Test/example.cpp```

Also can print good AST:
```
clang++ -emit-ast Test/example.cpp
print_AST_tool_c example.ast
```

## Run with two file
```print_AST_tool Test/example.cpp Test/test.cpp```