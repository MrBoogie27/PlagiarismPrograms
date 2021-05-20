#include <iostream>
#include <string>
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

class VisitorHasher : public RecursiveASTVisitor<ExampleVisitor> {
private:
    ASTContext *Context; // used for getting additional AST info
    long long hash = 7;
    size_t number_vertex = 0;
    const int MOD = 1e9 + 7;
    const int WEIGHT_VAR_DECLARATION = 13;
    const int WEIGHT_FUNC_DECLARATION = 17;
    const int WEIGHT_CALLER = 19;

    long long pow(int number, size_t power) {
        if (power == 0)
            return 1;
        int halfPow = pow(number, power / 2);
        if (power % 2 == 0)
            return (halfPow * halfPow) % MOD;
        else
            return (number * halfPow * halfPow) % MOD;
    }
public:

    explicit VisitorHasher(ASTContext *Context) : Context(Context) { }

    virtual bool VisitVarDecl(VarDecl *var)
    {
        return true;
    }

    virtual bool VisitFunctionDecl(FunctionDecl *func)
    {
        return true;
    }

    virtual bool VisitStmt(Stmt *st)
    {
        return true;
    }

    bool VisitCXXRecordDecl(CXXRecordDecl *Declaration) {
        return true;
    }
};

class ASTConsumerHasher : public clang::ASTConsumer {
public:
    explicit ASTConsumerHasher(ASTContext *Context)
            : Visitor(Context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }
private:
    VisitorHasher Visitor;
};

class PrintAllAST : public clang::ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
            clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
        return std::unique_ptr<clang::ASTConsumer>(
                new ASTConsumerHasher(&Compiler.getASTContext()));
    }
};

static llvm::cl::OptionCategory MyToolCategory("my-tool options");

int main(int argc, const char **argv) {
    CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
    ClangTool Tool(OptionsParser.getCompilations(),
                   OptionsParser.getSourcePathList());

    int result = Tool.run(newFrontendActionFactory<PrintAllAST>().get());
    return result;
}
