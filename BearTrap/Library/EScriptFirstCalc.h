//
// Created by mrboogie on 05.06.22.
//

#ifndef PLAGIARISMPROGRAMS_ESCRIPTFIRSTCALC_H
#define PLAGIARISMPROGRAMS_ESCRIPTFIRSTCALC_H

#include "CommonInclude.h"

namespace BearTrap {
    using namespace clang;

    class EScriptFirstCalc : public RecursiveASTVisitor<EScriptFirstCalc> {
    private:
        ASTContext *Context;
        const Node2NodeMap &MatchedNodes;
        const std::vector<PtrToUintMap> &Hashes;
    public:
        EScriptFirstCalc(ASTContext *Context,
                         const Node2NodeMap &MatchedNodes,
                         const std::vector<PtrToUintMap> &Hashes)
                : Context(Context), MatchedNodes(MatchedNodes), Hashes(Hashes) {}

        bool VisitDecl(Decl *decl);

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
                                             const Node2NodeMap &MatchedNodes,
                                             const std::vector<PtrToUintMap> &Hashes)
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
}

#endif //PLAGIARISMPROGRAMS_ESCRIPTFIRSTCALC_H
