//
// Created by mrboogie on 05.06.22.
//

#ifndef PLAGIARISMPROGRAMS_COMMONINCLUDE_H
#define PLAGIARISMPROGRAMS_COMMONINCLUDE_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <variant>
#include "clang/Basic/LangOptions.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include <clang/Tooling/Tooling.h>
#include "EditModes.h"

namespace BearTrap {
    using ASTUnitPtr = std::unique_ptr<clang::ASTUnit>;
    using ASTUnitPtrs = std::vector<ASTUnitPtr>;
    using Node2NodeMap = std::unordered_map<std::uintptr_t, std::uintptr_t>;
    using Node2Nodes = std::unordered_map<std::uintptr_t, std::vector<std::uintptr_t> >;
    using Node2SetNodes = std::unordered_map<std::uintptr_t, std::unordered_set<std::uintptr_t> >;
    using OneOfDeclStmt = std::variant<clang::Decl *, clang::Stmt *>;
    using PtrToUintMap = std::unordered_map<std::uintptr_t, uint32_t>;
    using ScriptStruct = std::vector<std::pair<EditModes, double> >;
}

#endif //PLAGIARISMPROGRAMS_COMMONINCLUDE_H
