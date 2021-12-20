# PlagiarismPrograms
This is a tool to compare two C++ programs for plagiarism

## About library

Library documentation is [here](https://clang.llvm.org/docs/LibTooling.html).

I used clang version 10.

## Printer
### Build tool
```
cmake --build /mnt/d/MIPT/PlagiarismPrograms/cmake-build-debug --target print_AST_tool -- -j 6
```
### Run print

Output AST: 
```clang Test/example.cpp -I. -Xclang -ast-dump -fsyntax-only``` 

My Output:
```print_AST_tool Test/example.cpp```

Also can print good AST:
```
clang++ -emit-ast Test/example.cpp
print_AST_tool_c example.ast
```

### Run with two file
```print_AST_tool Test/example.cpp Test/test.cpp```

## Hasher
### Build tool
```
cmake --build /mnt/d/MIPT/PlagiarismPrograms/cmake-build-debug --target hasher_AST_tool -- -j 6
```
### Run hasher with two file
```hasher_AST_tool Tests/example.cpp Tests/example_duplicate.cpp```

## Python program
before you need build Hasher tool
### Update data in DB
#### Update similarity in DB
```./main.py writer_similarity --host <host> --password <password> -U <user> -db <db>```
#### Update hashes in DB
```./main.py writer_hashes --host <host> --password <password> -U <user> -db <db>```

### Get local Data
#### Get subtree hashes
```./main.py hasher -f ./Tests/example.cpp```
#### Get metric similarity of programs: [0, 1]
```./main.py compare2AST -f ./Tests/example.cpp -s ./Tests/example_duplicate.cpp```
