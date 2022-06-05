//
// Created by mrboogie on 05.06.22.
//

#ifndef PLAGIARISMPROGRAMS_BOTTOMUPMATCHER_H
#define PLAGIARISMPROGRAMS_BOTTOMUPMATCHER_H

#include <variant>
#include <unordered_set>
#include "CommonInclude.h"

using namespace clang;
using namespace clang::tooling;

using Node2SetNodes = std::unordered_map<std::uintptr_t, std::unordered_set<std::uintptr_t> >;
using Node2NodeMap = std::unordered_map<std::uintptr_t, std::uintptr_t>;
using OneOfDeclStmt = std::variant<Decl*, Stmt*>;

class VisitorButtomUpMatcherBase : public RecursiveASTVisitor<VisitorButtomUpMatcherBase> {
public:
    VisitorButtomUpMatcherBase(ASTContext *Context, Node2NodeMap& matchers)
            : Context(Context)
            , Matchers(matchers)
    {
    }

    bool VisitVarDecl(VarDecl *var);
    bool VisitFunctionDecl(FunctionDecl *func);
    bool VisitStmt(Stmt *st);
    bool VisitCXXRecordDecl(CXXRecordDecl *Declaration);

    virtual bool VisitVarDeclExec(VarDecl *var) = 0;
    virtual bool VisitFunctionDeclExec(FunctionDecl *func) = 0;
    virtual bool VisitStmtExec(Stmt *st) = 0;
    virtual bool VisitCXXRecordDeclExec(CXXRecordDecl *Declaration) = 0;

    bool shouldTraversePostOrder() const {
        return true;
    }

protected:
    ASTContext *Context;
    Node2NodeMap& Matchers;
    Node2SetNodes Descendants;
protected:
    void UpdateMatchDescendants(std::uintptr_t id, std::vector<std::uintptr_t> children);
};

class VisitorButtomUpFirstMatcher : public VisitorButtomUpMatcherBase {
public:
    VisitorButtomUpFirstMatcher(ASTContext *Context, ASTContext *Context2, Node2NodeMap& matchers)
            : VisitorButtomUpMatcherBase(Context, matchers)
            , Context2(Context)
    {
    }

    bool VisitVarDeclExec(VarDecl *var) override;
    bool VisitFunctionDeclExec(FunctionDecl *func) override;
    bool VisitStmtExec(Stmt *st) override;
    bool VisitCXXRecordDeclExec(CXXRecordDecl *Declaration) override;

private:
    ASTContext *Context2;
};

class VisitorButtomUpSecondMatcher : public VisitorButtomUpMatcherBase {
public:
    VisitorButtomUpSecondMatcher(ASTContext *Context, OneOfDeclStmt node, Node2NodeMap& matchers)
            : VisitorButtomUpMatcherBase(Context, matchers)
            , OriginalNode(node)
            , BestNode({0, 0})
    {
        switch (OriginalNode.index()) {
            case 0:
                OriginalPtrNode = reinterpret_cast<std::uintptr_t>(std::get<Decl*>(OriginalNode));
                break;
            case 1:
                OriginalPtrNode = reinterpret_cast<std::uintptr_t>(std::get<Stmt*>(OriginalNode));
                break;
            default:
                assert(false);
        };
    }

    bool VisitVarDeclExec(VarDecl *var) override;
    bool VisitFunctionDeclExec(FunctionDecl *func) override;
    bool VisitStmtExec(Stmt *st) override;
    bool VisitCXXRecordDeclExec(CXXRecordDecl *Declaration) override;

    std::uintptr_t GetBestNode() const {
        return BestNode.first;
    }

    std::uintptr_t GetOriginalPtrNode() const {
        return OriginalPtrNode;
    }

private:
    OneOfDeclStmt OriginalNode;
    std::uintptr_t OriginalPtrNode;
    std::pair<std::uintptr_t, double> BestNode;
};

class ASTConsumerButtomUpFirstMatcher : public clang::ASTConsumer {
public:
    explicit ASTConsumerButtomUpFirstMatcher(ASTContext *Context, ASTContext *Context2, Node2NodeMap& matchers)
            : Visitor(Context, Context2, matchers) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }
private:
    VisitorButtomUpFirstMatcher Visitor;
};

class ASTConsumerButtomUpSecondMatcher : public clang::ASTConsumer {
public:
    explicit ASTConsumerButtomUpSecondMatcher(ASTContext *Context, OneOfDeclStmt node, Node2NodeMap& matchers)
            : Visitor(Context, node, matchers)
            , Matchers(matchers) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
        std::uintptr_t source = Visitor.GetOriginalPtrNode();
        std::uintptr_t target = Visitor.GetBestNode();
        if (source && target) {
            Matchers[source] = target;
            Matchers[target] = source;
        }
    }
private:
    VisitorButtomUpSecondMatcher Visitor;
    Node2NodeMap& Matchers;
};

#endif //PLAGIARISMPROGRAMS_BOTTOMUPMATCHER_H
