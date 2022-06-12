//
// Created by mrboogie on 05.06.22.
//

#ifndef PLAGIARISMPROGRAMS_TOPDOWNMATCHER_H
#define PLAGIARISMPROGRAMS_TOPDOWNMATCHER_H

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

using PtrToUintMap = std::unordered_map<std::uintptr_t, uint32_t>;

class VisitorTopDownMatcher : public RecursiveASTVisitor<VisitorTopDownMatcher> {
public:
    explicit VisitorTopDownMatcher(ASTContext *Context, const PtrToUintMap& myHashes)
            : Context(Context)
            , Hashes(myHashes)
    {
    }

    bool VisitVarDecl(VarDecl *var);
    bool VisitFunctionDecl(FunctionDecl *func);
    bool VisitStmt(Stmt *st);
    bool VisitCXXRecordDecl(CXXRecordDecl *Declaration);

    bool shouldTraversePostOrder() const {
        return true;
    }

private:
    ASTContext *Context; // used for getting additional AST info
    const PtrToUintMap& Hashes;
};

class ASTConsumerTopDownMatcher : public clang::ASTConsumer {
public:
    explicit ASTConsumerTopDownMatcher(ASTContext *Context, const PtrToUintMap& myHashes)
            : Visitor(Context, myHashes) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context);
private:
    VisitorTopDownMatcher Visitor;
};

class ASTFrontendTopDown: public clang::ASTFrontendAction {
public:
    explicit ASTFrontendTopDown(const std::map<std::string, PtrToUintMap>& hashes)
        : Hashes(hashes) {

    }

    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
            clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
        for (auto&[key, _] : Hashes) {
            llvm::outs() << key << "\n";
        }
        llvm::outs() << "current: " << InFile << "\n";
        llvm::outs().flush();
        if (Hashes.count(InFile)) {
            // std::cout << "File in map" << std::endl;
        } else {
            // std::cout << "File not in map" << std::endl;
        }

        return std::unique_ptr<clang::ASTConsumer>(
                new ASTConsumerTopDownMatcher(&Compiler.getASTContext(), Hashes.at(InFile)));
    }

private:
    std::map<std::string, PtrToUintMap> Hashes;
};


std::unique_ptr<FrontendActionFactory> newTopDownFrontendActionFactory(const std::map<std::string, PtrToUintMap>& Hashes);

#endif //PLAGIARISMPROGRAMS_TOPDOWNMATCHER_H
