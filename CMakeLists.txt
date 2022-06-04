cmake_minimum_required(VERSION 3.10)
project(PlagiarismPrograms)

find_package(Clang REQUIRED)

message(STATUS "CMAKE_ROOT ${CMAKE_ROOT}")
message(STATUS "CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}")

set(CMAKE_CXX_STANDARD 17)
set(LLVM_LINK_COMPONENTS support)

add_definitions(${LLVM_DEFINITIONS})

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})

add_executable(print_AST_tool ASTDump.cpp)
add_executable(print_AST_tool_C ASTDump-C.cpp)
add_executable(hasher_AST_tool ASTHash.cpp)
add_executable(bearTrap matcher.cpp BearTrap/HeightCalc.cpp BearTrap/HeightCalc.h)

target_link_libraries(print_AST_tool PRIVATE clangTooling)
target_link_libraries(hasher_AST_tool PRIVATE clangTooling)
target_link_libraries(bearTrap PRIVATE clangTooling)
