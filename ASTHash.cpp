#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
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

class VisitorHasher : public RecursiveASTVisitor<VisitorHasher> {
private:
    ASTContext *Context; // used for getting additional AST info
    std::vector<int> all_states;
    std::vector<long long> last_access;
    size_t height = 0;
    size_t max_height = 0;
    const int MOD = 1e9 + 7;
    const int WEIGHT_VAR_DECLARATION = 13;
    const int WEIGHT_FUNC_DECLARATION = 17;
    const int WEIGHT_CALLER = 19;
    const int WEIGHT_SOMETHING = 23;

    long long pow(int number, size_t power) {
        if (power == 0)
            return 1ll;
        long long halfPow = pow(number, power / 2);
        if (power % 2 == 0)
            return (halfPow * halfPow) % MOD;
        else
            return (((number * halfPow) % MOD) * halfPow) % MOD;
    }

    long long get_uniq_hash(int identifier) {
        return pow(identifier, max_height - height + 1);
    }
public:

    explicit VisitorHasher(ASTContext *Context) : Context(Context) { }

    bool VisitVarDecl(VarDecl *var)
    {
        if (Context->getSourceManager().isInMainFile(var->getLocation())) //checks if the node is in the main = input file.
        {
            last_access.push_back(get_uniq_hash(WEIGHT_VAR_DECLARATION));
        }
        return true;
    }

    bool VisitFunctionDecl(FunctionDecl *func)
    {
        if (Context->getSourceManager().isInMainFile(func->getLocation())) //checks if the node is in the main = input file.
        {
            last_access.push_back(get_uniq_hash(WEIGHT_FUNC_DECLARATION));
        }
        return true;
    }

    bool VisitStmt(Stmt *st)
    {
        if (Context->getSourceManager().isInMainFile(st->getBeginLoc())) //checks if the node is in the main = input file.
        {
            long long hash = 7;
            for (int child_hash: last_access) {
                hash = (hash + child_hash) % MOD;
            }
            last_access.clear();
            if (CallExpr *call = dyn_cast<CallExpr>(st)) {
                hash = (hash + get_uniq_hash(WEIGHT_CALLER)) % MOD;
                hash = (hash + get_uniq_hash(static_cast<int>(call->getNumArgs()))) % MOD;
            } else {
                hash = (hash + get_uniq_hash(WEIGHT_SOMETHING)) % MOD;
            }
            all_states.push_back(hash);
            last_access.push_back(hash);
            --height;
            if (!height) {
                max_height = 0;
            }
        }
        return true;
    }

    bool VisitCXXRecordDecl(CXXRecordDecl *Declaration) {
        FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getBeginLoc());
        return true;
    }

    bool shouldTraversePostOrder() const {
        return true;
    }

    bool dataTraverseStmtPre(Stmt *S) {
        ++height;
        max_height = std::max(max_height, height);
        return true;
    }

    long long GetHash() const {
        return all_states.back();
    }

    std::vector<int> GetIdsAst() const {
        return all_states;
    }
};

class ASTConsumerHasher : public clang::ASTConsumer {
public:
    explicit ASTConsumerHasher(ASTContext *Context)
            : Visitor(Context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
        std::cout << Visitor.GetHash() << std::endl;
        for (int id: Visitor.GetIdsAst()) {
            std::cout << id << " ";
        }
        std::cout << std::endl;
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

static llvm::cl::OptionCategory MyToolCategory("Additional options");

int main(int argc, const char **argv) {
    CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
    ClangTool Tool(OptionsParser.getCompilations(),
                   OptionsParser.getSourcePathList());

    int result = Tool.run(newFrontendActionFactory<PrintAllAST>().get());
    return result;
}
