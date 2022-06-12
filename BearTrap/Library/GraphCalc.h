//
// Created by mrboogie on 05.06.22.
//

#ifndef PLAGIARISMPROGRAMS_GRAPHCALC_H
#define PLAGIARISMPROGRAMS_GRAPHCALC_H

#include "CommonInclude.h"

namespace BearTrap {
    using namespace clang;

    class VisitorCalcGraph : public RecursiveASTVisitor<VisitorCalcGraph> {
    private:
        ASTContext *Context; // used for getting additional AST info
        Node2Nodes Graph;
    public:
        explicit VisitorCalcGraph(ASTContext *Context) : Context(Context) {}

#define VISIT(CLASS)                               \
      bool Visit##CLASS(CLASS *node);

#include "../../CommonLib/VisitNodes.inc"

#undef VISIT

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
}

#endif //PLAGIARISMPROGRAMS_GRAPHCALC_H
