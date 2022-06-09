//
// Created by mrboogie on 05.06.22.
//

#ifndef PLAGIARISMPROGRAMS_ESCRIPTFIRSTCALC_H
#define PLAGIARISMPROGRAMS_ESCRIPTFIRSTCALC_H

#include "CommonInclude.h"
#include "EditModes.h"

using namespace clang;
using namespace clang::tooling;

using Node2NodeMap = std::unordered_map<std::uintptr_t, std::uintptr_t>;
using PtrToUintMap = std::unordered_map<std::uintptr_t, uint32_t>;
using ScriptStruct = std::vector<std::pair<EditModes, uint32_t> >;

class EScriptFirstCalc : public RecursiveASTVisitor<EScriptFirstCalc> {
private:
    ASTContext *Context;
    const Node2NodeMap& MatchedNodes;
    const std::vector<PtrToUintMap>& Hashes;
public:
    EScriptFirstCalc(ASTContext *Context,
                     const Node2NodeMap& MatchedNodes,
                     const std::vector<PtrToUintMap>& Hashes)
        : Context(Context)
        , MatchedNodes(MatchedNodes)
        , Hashes(Hashes)
        { }

    bool VisitDecl(Decl *var);
    bool VisitStmt(Stmt *S);

    ScriptStruct GetEditScript() const {
        return EditScript;
    }
private:
    ScriptStruct EditScript;
};

class ASTConsumerEScriptFirstCalc : public clang::ASTConsumer {
public:
    explicit ASTConsumerEScriptFirstCalc(ASTContext *Context,
                                         const Node2NodeMap& MatchedNodes,
                                         const std::vector<PtrToUintMap>& Hashes)
            : Visitor(Context, MatchedNodes, Hashes) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

    ScriptStruct GetEditScript() const {
        return Visitor.GetEditScript();
    }
private:
    EScriptFirstCalc Visitor;
};

#endif //PLAGIARISMPROGRAMS_ESCRIPTFIRSTCALC_H
