//
// Created by mrboogie on 09.06.22.
//

#ifndef PLAGIARISMPROGRAMS_ESCRIPTSECONDCALC_H
#define PLAGIARISMPROGRAMS_ESCRIPTSECONDCALC_H

#include "CommonInclude.h"

namespace BearTrap {

    using namespace clang;

    class EScriptSecondCalc : public RecursiveASTVisitor<EScriptSecondCalc> {
    private:
        ASTContext *Context;
        const Node2NodeMap &MatchedNodes;
        const PtrToUintMap &Hash;
    public:
        EScriptSecondCalc(ASTContext *Context,
                          const Node2NodeMap &MatchedNodes,
                          const PtrToUintMap &Hash)
                : Context(Context), MatchedNodes(MatchedNodes), Hash(Hash) {}

        bool VisitDecl(Decl *decl);

        bool VisitStmt(Stmt *S);

        ScriptStruct GetEditScript() const {
            return EditScript;
        }

    private:
        ScriptStruct EditScript;
    };

    class ASTConsumerEScriptSecondCalc : public clang::ASTConsumer {
    public:
        explicit ASTConsumerEScriptSecondCalc(ASTContext *Context,
                                              const Node2NodeMap &MatchedNodes,
                                              const PtrToUintMap &Hash)
                : Visitor(Context, MatchedNodes, Hash) {}

        virtual void HandleTranslationUnit(clang::ASTContext &Context) {
            Visitor.TraverseDecl(Context.getTranslationUnitDecl());
        }

        ScriptStruct GetEditScript() const {
            return Visitor.GetEditScript();
        }

    private:
        EScriptSecondCalc Visitor;
    };
}

#endif //PLAGIARISMPROGRAMS_ESCRIPTSECONDCALC_H
