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

class VisitorButtomUpSecondMatcher : public VisitorButtomUpMatcherBase {
public:
    VisitorButtomUpSecondMatcher(ASTContext *Context,
                                 OneOfDeclStmt node,
                                 std::unordered_set<std::uintptr_t>&& node_child_match,
                                 size_t countDesc,
                                 Node2NodeMap& matchers)
            : VisitorButtomUpMatcherBase(Context, matchers)
            , OriginalNode(node)
            , NodeChildMatch(std::move(node_child_match))
            , NodeCountDesc(countDesc)
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
        // std::cout << "INDEX: " << OriginalNode.index() << std::endl;
        // std::cout << "SIZE match: " << NodeChildMatch.size() << std::endl;
        // std::cout << "SIZE child: " << NodeCountDesc << std::endl;
    }

    bool VisitDeclExec(Decl *decl);
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
    std::unordered_set<std::uintptr_t> NodeChildMatch;
    size_t NodeCountDesc;
    std::pair<std::uintptr_t, double> BestNode;
private:
    double Dice(std::uintptr_t currentNode) const;
    void UpdateBest(std::uintptr_t currentNode, double dice);
};

class ASTConsumerButtomUpSecondMatcher : public clang::ASTConsumer {
public:
    explicit ASTConsumerButtomUpSecondMatcher(ASTContext *Context,
                                              OneOfDeclStmt node,
                                              std::unordered_set<std::uintptr_t>&& node_child_match,
                                              size_t countDesc,
                                              Node2NodeMap& matchers)
            : Visitor(Context, node, std::move(node_child_match), countDesc, matchers)
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

class VisitorButtomUpFirstMatcher : public VisitorButtomUpMatcherBase {
public:
    VisitorButtomUpFirstMatcher(ASTContext *Context, ASTContext *Context2, Node2NodeMap& matchers)
            : VisitorButtomUpMatcherBase(Context, matchers)
            , Context2(Context2)
    {
    }

    bool VisitVarDeclExec(VarDecl *var) override;
    bool VisitFunctionDeclExec(FunctionDecl *func) override;
    bool VisitStmtExec(Stmt *st) override;
    bool VisitCXXRecordDeclExec(CXXRecordDecl *Declaration) override;

private:
    ASTContext *Context2;
private:
    std::unordered_set<std::uintptr_t> GetChildMatch(std::uintptr_t node);

    template<class NodePtr>
    bool VisitAllNode(NodePtr node){
        std::uintptr_t id = reinterpret_cast<std::uintptr_t>(node);
        if (Matchers.count(id)) {
            return true;
        }

        ASTConsumerButtomUpSecondMatcher consumer(Context2,
                                                  node,
                                                  GetChildMatch(id),
                                                  Descendants[id].size(),
                                                  Matchers);

        consumer.HandleTranslationUnit(*Context2);
        return true;
    }
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


#endif //PLAGIARISMPROGRAMS_BOTTOMUPMATCHER_H
