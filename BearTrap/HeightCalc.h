//
// Created by mrboogie on 04.06.22.
//

#ifndef PLAGIARISMPROGRAMS_HEIGHTCALC_H
#define PLAGIARISMPROGRAMS_HEIGHTCALC_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "llvm-10/llvm/Support/raw_ostream.h"
#include "clang/Basic/LangOptions.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Decl.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"

using namespace clang;
using namespace clang::tooling;

class VisitorCalcHeights : public RecursiveASTVisitor<VisitorCalcHeights> {
private:
    ASTContext *Context; // used for getting additional AST info
    std::unordered_map<std::uintptr_t, uint32_t> heights;
public:

    explicit VisitorCalcHeights(ASTContext *Context) : Context(Context) { }

    bool VisitVarDecl(VarDecl *var);
    bool VisitFunctionDecl(FunctionDecl *func);
    bool VisitStmt(Stmt *S);
    bool VisitCXXRecordDecl(CXXRecordDecl *Declaration);
    bool shouldTraversePostOrder() const {
        return true;
    }
    std::unordered_map<std::uintptr_t, uint32_t> GetHeights() const {
        return heights;
    }
};

class ASTConsumerCalcHeights : public clang::ASTConsumer {
public:
    explicit ASTConsumerCalcHeights(ASTContext *Context)
            : Visitor(Context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context);
private:
    VisitorCalcHeights Visitor;
};

class ASTFrontendCalcHeights : public clang::ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
            clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
        return std::make_unique<ASTConsumerCalcHeights>(&Compiler.getASTContext());
    }
};

#endif //PLAGIARISMPROGRAMS_HEIGHTCALC_H
