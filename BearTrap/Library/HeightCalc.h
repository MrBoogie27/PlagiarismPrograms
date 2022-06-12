//
// Created by mrboogie on 04.06.22.
//

#ifndef PLAGIARISMPROGRAMS_HEIGHTCALC_H
#define PLAGIARISMPROGRAMS_HEIGHTCALC_H

#include "CommonInclude.h"

namespace BearTrap {
    using namespace clang;

    class VisitorCalcHeights : public RecursiveASTVisitor<VisitorCalcHeights> {
    private:
        ASTContext *Context; // used for getting additional AST info
        std::unordered_map<std::uintptr_t, uint32_t> heights;
    public:

        explicit VisitorCalcHeights(ASTContext *Context) : Context(Context) {}

#define VISIT(CLASS)                               \
  bool Visit##CLASS(CLASS *node);

#include "../../CommonLib/VisitNodes.inc"

#undef VISIT

        bool shouldTraversePostOrder() const {
            return true;
        }

        std::unordered_map<std::uintptr_t, uint32_t> GetHeights() const {
            return heights;
        }
    };

    class ASTConsumerCalcHeights : public ASTConsumer {
    public:
        explicit ASTConsumerCalcHeights(ASTContext *Context)
                : Visitor(Context) {}

        virtual void HandleTranslationUnit(clang::ASTContext &Context);

        std::unordered_map<std::uintptr_t, uint32_t> GetHeights() const {
            return Visitor.GetHeights();
        }

    private:
        VisitorCalcHeights Visitor;
    };

    class ASTFrontendCalcHeights : public ASTFrontendAction {
    public:
        virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
                CompilerInstance &Compiler, llvm::StringRef InFile) {
            return std::make_unique<ASTConsumerCalcHeights>(&Compiler.getASTContext());
        }
    };
}

#endif //PLAGIARISMPROGRAMS_HEIGHTCALC_H
