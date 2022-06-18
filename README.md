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
cmake --build ./cmake-build-debug --target hasher_AST_tool -- -j 6
```
### Run hasher with two file
```./cmake-build-debug/hasher_AST_tool ./Tests/example.cpp ./Tests/example_duplicate.cpp```

## BearTrap
### About
This compare two AST with GumTree algorithm
### Build tool
```
cmake --build ./cmake-build-debug --target bearTrap -- -j 6
```
### Run hasher with exactly two file
```./cmake-build-debug/bearTrap .Tests/example.cpp .Tests/example_duplicate.cpp```

## Python program
before you need build c++ Tools
```cd ./PythonFrontend```
### Update data in DB
#### Update hashes in DB
```./main.py writer_hashes```
#### Update similarity in DB by hash or hash + levenshtein from DB
```./main.py writer_similarity_v[1,3]```
#### Update similarity in DB by bearTrap tool
```./main.py writer_bear_trap```
#### Update similarity in DB by simian tool
```./main.py writer_simian```
#### GUI for manual check similarity
```./main.py manual_similarity```


### Get local Data
#### Get subtree hashes
```./main.py hasher -f ./Tests/example.cpp```
#### Get metric similarity of programs: [0, 1] by subtree hashes
```./main.py compare2AST -f ./Tests/example.cpp -s ./Tests/example_duplicate.cpp```

### Get local Data
#### Get subtree hashes
```./main.py hasher```
#### Get metric similarity of programs: [0, 1] by subtree hashes
```./main.py compare2AST```

### Get stats
#### Get some table, histogram and graph by metrics
```./main.py get_stats
