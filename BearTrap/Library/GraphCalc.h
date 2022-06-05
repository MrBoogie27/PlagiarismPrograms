//
// Created by mrboogie on 05.06.22.
//

#ifndef PLAGIARISMPROGRAMS_GRAPHCALC_H
#define PLAGIARISMPROGRAMS_GRAPHCALC_H

#include "CommonInclude.h"

using namespace clang;
using namespace clang::tooling;

using Node2Nodes = std::unordered_map<std::uintptr_t, std::vector<std::uintptr_t> >;

class VisitorCalcGraph : public RecursiveASTVisitor<VisitorCalcGraph> {
private:
    ASTContext *Context; // used for getting additional AST info
    Node2Nodes Graph;
public:
    explicit VisitorCalcGraph(ASTContext *Context) : Context(Context) { }

    bool VisitVarDecl(VarDecl *var);
    bool VisitFunctionDecl(FunctionDecl *func);
    bool VisitStmt(Stmt *S);
    bool VisitCXXRecordDecl(CXXRecordDecl *Declaration);

    Node2Nodes GetGraph() const {
        return Graph;
    }
};

class ASTConsumerCalcGraph : public clang::ASTConsumer {
public:
    explicit ASTConsumerCalcGraph(ASTContext *Context)
            : Visitor(Context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

    Node2Nodes GetGraph() const {
        return Visitor.GetGraph();
    }
private:
    VisitorCalcGraph Visitor;
};

class ASTFrontendCalcGraph : public clang::ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
            clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
        return std::make_unique<ASTConsumerCalcGraph>(&Compiler.getASTContext());
    }
};

#endif //PLAGIARISMPROGRAMS_GRAPHCALC_H
