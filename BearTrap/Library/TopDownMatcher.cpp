//
// Created by mrboogie on 05.06.22.
//

#include "TopDownMatcher.h"

bool VisitorTopDownMatcher::VisitVarDecl(VarDecl *var) {
    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(var);

    std::cout << "We in VisitVarDecl " << std::hex << id << std::dec << std::endl;
    std::cout << " and our hash: " << Hashes.at(id) << std::endl;

    return true;
}
bool VisitorTopDownMatcher::VisitFunctionDecl(FunctionDecl *func) {
    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(func);

    std::cout << "We in VisitFunctionDecl " << id << std::endl;
    std::cout << " and our hash: " << Hashes.at(id) << std::endl;

    return true;
}
bool VisitorTopDownMatcher::VisitStmt(Stmt *st) {
    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(st);

    std::cout << "We in VisitStmt " << std::hex << id << std::dec << std::endl;
    std::cout << " and our hash: " << Hashes.at(id) << std::endl;

    return true;
}
bool VisitorTopDownMatcher::VisitCXXRecordDecl(CXXRecordDecl *Declaration) {
    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(Declaration);

    std::cout << "We in VisitCXXRecordDecl " << std::endl;
    std::cout << id << std::endl;

    return true;
}
void ASTConsumerTopDownMatcher::HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
}

std::unique_ptr<FrontendActionFactory> newTopDownFrontendActionFactory(const std::map<std::string, PtrToUintMap>& Hashes) {
    class TopDownFrontendActionFactory : public FrontendActionFactory {
    public:
        explicit TopDownFrontendActionFactory(const std::map<std::string, PtrToUintMap>& Hashes)
                : Hashes(Hashes) {
        }

        std::unique_ptr<FrontendAction> create() override {
            return std::make_unique<ASTFrontendTopDown>(Hashes);
        }
    private:
        const std::map<std::string, PtrToUintMap>& Hashes;
    };

    return std::unique_ptr<FrontendActionFactory>(
            new TopDownFrontendActionFactory(Hashes));
}
