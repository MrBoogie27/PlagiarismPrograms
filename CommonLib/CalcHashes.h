//
// Created by mrboogie on 04.06.22.
//

#ifndef PLAGIARISMPROGRAMS_CALCHASHES_H
#define PLAGIARISMPROGRAMS_CALCHASHES_H

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

class VisitorCalcHashes : public RecursiveASTVisitor<VisitorCalcHashes> {
public:
    static void ExpandPowPrimes(size_t length = DEFAUL_LEGTH_PRIME_POWS);
    static uint32_t HashFunction(std::vector<uint32_t> elems);
    static uint32_t GetPPower(size_t num_p, size_t power);
    inline static uint32_t AddByModule(uint32_t left, uint32_t right);
    inline static uint32_t MultiplyByModule(uint32_t left, uint32_t right);
public:
    explicit VisitorCalcHashes(ASTContext *Context)
        : Context(Context)
    {
        ExpandPowPrimes();
    }

    bool VisitVarDecl(VarDecl *var);
    bool VisitFunctionDecl(FunctionDecl *func);
    bool VisitStmt(Stmt *st);
    bool VisitCXXRecordDecl(CXXRecordDecl *Declaration);

    bool shouldTraversePostOrder() const {
        return true;
    }

    PtrToUintMap GetHashes() const {
        return Hashes;
    }

private:
    static constexpr std::array<uint32_t, 2> Ps = {31, 433494437};
    static constexpr uint32_t DEFAUL_LEGTH_PRIME_POWS = 100;
    static constexpr uint32_t MOD = 1e9 + 7;
    static constexpr uint32_t START_WEIGHT_DECLARATION = 28657;
    static constexpr uint32_t START_WEIGHT_SOMETHING = 33461;
    static std::vector<std::vector<uint32_t> > ps_pows;

private:
    ASTContext *Context; // used for getting additional AST info
    PtrToUintMap Hashes;
};

class ASTConsumerCalcHashes : public clang::ASTConsumer {
public:
    explicit ASTConsumerCalcHashes(ASTContext *Context)
            : Visitor(Context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context);
private:
    VisitorCalcHashes Visitor;
};

class ASTFrontendCalcHashes : public clang::ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
            clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
        return std::unique_ptr<clang::ASTConsumer>(
                new ASTConsumerCalcHashes(&Compiler.getASTContext()));
    }
};

#endif //PLAGIARISMPROGRAMS_CALCHASHES_H
