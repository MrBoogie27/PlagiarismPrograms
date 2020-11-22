# PlagiarismPrograms
This is a tool to compare two C++ programs for plagiarism

Output AST: 
```clang example.cpp -I. -Xclang -ast-dump -fsyntax-only``` 

My Output:
```print_AST_tool example.cpp```

Also can print good AST:
```
clang++ -emit-ast example.cpp
print_AST_tool_c example.ast
```
