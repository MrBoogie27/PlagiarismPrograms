//
// Created by mrboogie on 04.06.22.
//

#include "HeightCalc.h"

bool VisitorCalcHeights::VisitVarDecl(VarDecl *var)
{
    if (!Context->getSourceManager().isInMainFile(var->getLocation())) //checks if the node is in the main = input file.
    {
        return true;
    }

    uint32_t cur_height = 0u;
    if (Stmt* S = var->getInit()) {
        std::uintptr_t temp_id = reinterpret_cast<std::uintptr_t>(S);
        if (heights.count(temp_id)) {
            cur_height = std::max(cur_height, heights[temp_id] + 1);
        }
    }

    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(var);
    heights[id] = cur_height;
    return true;
}

bool VisitorCalcHeights::VisitFunctionDecl(FunctionDecl *func)
{
    if (!Context->getSourceManager().isInMainFile(func->getLocation())) //checks if the node is in the main = input file.
    {
        return true;
    }

    uint32_t cur_height = 0u;

    if (Stmt* S = func->getBody()) {
        std::uintptr_t temp_id = reinterpret_cast<std::uintptr_t>(S);
        if (heights.count(temp_id)) {
            cur_height = std::max(cur_height, heights[temp_id] + 1);
        }
    }
    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(func);
    heights[id] = cur_height;
    return true;
}

bool VisitorCalcHeights::VisitStmt(Stmt *S)
{
    if (!Context->getSourceManager().isInMainFile(S->getBeginLoc())) //checks if the node is in the main = input file.
    {
        return true;
    }

    uint32_t cur_height = 0u;
    if (S->getStmtClass() == Stmt::DeclStmtClass) {
        DeclStmt *newD = static_cast<DeclStmt*>(S);
        for (auto *I : newD->decls()) {
            std::uintptr_t temp_id = reinterpret_cast<std::uintptr_t>(I);
            if (heights.count(temp_id)) {
                cur_height = std::max(cur_height, heights[temp_id] + 1);
            }
        }
    }
    for (auto D = S->child_begin(); D != S->child_end(); D++) {
        std::uintptr_t temp_id = reinterpret_cast<std::uintptr_t>(*D);
        if (heights.count(temp_id)) {
            cur_height = std::max(cur_height, heights[temp_id] + 1);
        }
    }
    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(S);
    heights[id] = cur_height;

    return true;
}

bool VisitorCalcHeights::VisitCXXRecordDecl(CXXRecordDecl *Declaration) {
    FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getBeginLoc());
    return true;
};

void ASTConsumerCalcHeights::HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());

    Context.getTranslationUnitDecl()->dump();
    llvm::outs().flush();
    for (auto& [key, value]: Visitor.GetHeights()) {
        std::cout << std::hex << key << " " << std::dec << value << "\n";
    }
    std::cout.flush();
};
