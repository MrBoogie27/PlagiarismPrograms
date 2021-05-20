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

class ExampleVisitor : public RecursiveASTVisitor<ExampleVisitor> {
private:
    ASTContext *Context; // used for getting additional AST info

public:

    explicit ExampleVisitor(ASTContext *Context) : Context(Context) { }

    virtual bool VisitVarDecl(VarDecl *var)
    {
        if (Context->getSourceManager().isInMainFile(var->getLocation())) //checks if the node is in the main = input file.
        {
            llvm::outs() << "VarDecl (" << var->getNameAsString() << ")\n";
        }
        return true;
    }

    virtual bool VisitFunctionDecl(FunctionDecl *func)
    {
        if (Context->getSourceManager().isInMainFile(func->getLocation())) //checks if the node is in the main = input file.
        {
            llvm::outs() << "FunctionDecl (" << func->getNameAsString() << ")\n";
            std::string funcName = func->getNameAsString();
            std::string funcType = func->getCallResultType().getAsString();
            llvm::outs() << "Found function declaration: " << funcName << " of type "
                         << funcType << "\n";
        }
        return true;
    }

    virtual bool VisitStmt(Stmt *st)
    {
        if (Context->getSourceManager().isInMainFile(st->getBeginLoc())) //checks if the node is in the main = input file.
        {
            llvm::outs() << st->getStmtClassName() << " (" << " " << ")\n";
            if (CallExpr *call = dyn_cast<CallExpr>(st)) {
                FunctionDecl *func_decl = call->getDirectCallee();
                std::string funcCall = func_decl->getNameInfo().getName().getAsString();
                llvm::outs() << "Found function call: " << funcCall << " with arguments ";
                for (int i = 0, j = call->getNumArgs(); i < j; i++) {
                    std::string TypeS;
                    llvm::raw_string_ostream s(TypeS);
                    clang::LangOptions lng;
                    const clang::PrintingPolicy Policy(lng);
                    call->getArg(i)->printPretty(s, 0, Policy);
                    llvm::outs() << s.str() << ", ";
                }
                llvm::outs() << "\n";
            }
        }
        return true;
    }

    bool VisitCXXRecordDecl(CXXRecordDecl *Declaration) {
        FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getBeginLoc());
        return true;
    }
};

class ASTConsumerPrint : public clang::ASTConsumer {
public:
    explicit ASTConsumerPrint(ASTContext *Context)
            : Visitor(Context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }
private:
    ExampleVisitor Visitor;
};

class PrintAllAST : public clang::ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
            clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
        return std::unique_ptr<clang::ASTConsumer>(
                new ASTConsumerPrint(&Compiler.getASTContext()));
    }
};

static llvm::cl::OptionCategory MyToolCategory("my-tool options");

int main(int argc, const char **argv) {
    CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
    ClangTool Tool(OptionsParser.getCompilations(),
                   OptionsParser.getSourcePathList());

    // run the Clang Tool, creating a new FrontendAction (explained below)
    int result = Tool.run(newFrontendActionFactory<PrintAllAST>().get());
    return result;
}
