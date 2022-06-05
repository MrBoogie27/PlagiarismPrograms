//
// Created by mrboogie on 05.06.22.
//

#include "GraphCalc.h"

bool VisitorCalcGraph::VisitVarDecl(VarDecl *var)
{
    if (!Context->getSourceManager().isInMainFile(var->getLocation())) {
        return true;
    }

    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(var);

    uint32_t cur_height = 0u;
    if (Stmt* S = var->getInit()) {
        std::uintptr_t temp_id = reinterpret_cast<std::uintptr_t>(S);
        Graph[id].push_back(temp_id);
    }

    return true;
}

bool VisitorCalcGraph::VisitFunctionDecl(FunctionDecl *func)
{
    if (!Context->getSourceManager().isInMainFile(func->getLocation())) {
        return true;
    }

    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(func);

    if (Stmt* S = func->getBody()) {
        std::uintptr_t temp_id = reinterpret_cast<std::uintptr_t>(S);
        Graph[id].push_back(temp_id);
    }

    return true;
}

bool VisitorCalcGraph::VisitStmt(Stmt *S)
{
    if (!Context->getSourceManager().isInMainFile(S->getBeginLoc())) {
        return true;
    }

    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(S);
    if (S->getStmtClass() == Stmt::DeclStmtClass) {
        DeclStmt *newD = static_cast<DeclStmt*>(S);
        for (auto *I : newD->decls()) {
            std::uintptr_t temp_id = reinterpret_cast<std::uintptr_t>(I);
            Graph[id].push_back(temp_id);
        }
    }
    for (auto D = S->child_begin(); D != S->child_end(); D++) {
        std::uintptr_t temp_id = reinterpret_cast<std::uintptr_t>(*D);
        Graph[id].push_back(temp_id);
    }
    return true;
}

bool VisitorCalcGraph::VisitCXXRecordDecl(CXXRecordDecl *Declaration) {
    FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getBeginLoc());
    return true;
};
