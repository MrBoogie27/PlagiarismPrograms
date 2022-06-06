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

add_executable(print_AST_tool ASTDump/ASTDump.cpp)
add_executable(print_AST_tool_C ASTDump/ASTDump-C.cpp)
add_executable(hasher_AST_tool ASTHasher/ASTHash.cpp CommonLib/CalcHashes.cpp CommonLib/CalcHashes.h)
add_executable(bearTrap BearTrap/matcher.cpp BearTrap/Library/HeightCalc.cpp BearTrap/Library/HeightCalc.h CommonLib/CalcHashes.cpp CommonLib/CalcHashes.h BearTrap/Library/TopDownMatcher.cpp BearTrap/Library/TopDownMatcher.h BearTrap/Library/ASTFrontendActionMatcher.cpp BearTrap/Library/ASTFrontendActionMatcher.h BearTrap/Library/CommonInclude.h BearTrap/Library/GraphCalc.cpp BearTrap/Library/GraphCalc.h BearTrap/Library/BottomUpMatcher.cpp BearTrap/Library/BottomUpMatcher.h BearTrap/Library/NodeCompares.cpp BearTrap/Library/NodeCompares.h)

target_link_libraries(print_AST_tool PRIVATE clangTooling)
target_link_libraries(hasher_AST_tool PRIVATE clangTooling)
target_link_libraries(bearTrap PRIVATE clangTooling)
