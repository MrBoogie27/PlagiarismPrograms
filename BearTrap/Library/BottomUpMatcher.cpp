//
// Created by mrboogie on 05.06.22.
//

#include "BottomUpMatcher.h"
// VisitorButtomUpMatcherBase
bool VisitorButtomUpMatcherBase::VisitVarDecl(VarDecl *var) {
    if (!Context->getSourceManager().isInMainFile(var->getLocation())) {
        return false;
    }

    std::vector<std::uintptr_t> children_id;
    if (Stmt* S = var->getInit()) {
        std::uintptr_t temp_id = reinterpret_cast<std::uintptr_t>(S);
        children_id.push_back(temp_id);
    }
    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(var);
    UpdateMatchDescendants(id, children_id);

    return VisitVarDeclExec(var);
}

bool VisitorButtomUpMatcherBase::VisitFunctionDecl(FunctionDecl *func) {
    if (!Context->getSourceManager().isInMainFile(func->getLocation())) {
        return false;
    }
    std::vector<std::uintptr_t> children_id;
    if (Stmt* S = func->getBody()) {
        std::uintptr_t temp_id = reinterpret_cast<std::uintptr_t>(S);
        children_id.push_back(temp_id);
    }
    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(func);
    UpdateMatchDescendants(id, children_id);

    return VisitFunctionDecl(func);
}

bool VisitorButtomUpMatcherBase::VisitStmt(Stmt *st) {
    if (!Context->getSourceManager().isInMainFile(st->getBeginLoc())) {
        return false;
    }

    std::vector<std::uintptr_t> children_id;
    if (st->getStmtClass() == Stmt::DeclStmtClass) {
        DeclStmt *newD = static_cast<DeclStmt*>(st);
        for (auto *I : newD->decls()) {
            std::uintptr_t temp_id = reinterpret_cast<std::uintptr_t>(I);
            children_id.push_back(temp_id);
        }
    }
    for (auto D = st->child_begin(); D != st->child_end(); D++) {
        std::uintptr_t temp_id = reinterpret_cast<std::uintptr_t>(*D);
        children_id.push_back(temp_id);
    }
    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(st);
    UpdateMatchDescendants(id, children_id);

    return VisitStmt(st);
}

bool VisitorButtomUpMatcherBase::VisitCXXRecordDecl(CXXRecordDecl *Declaration) {
    if (!Context->getSourceManager().isInMainFile(Declaration->getLocation())) {
        return false;
    }
    std::vector<std::uintptr_t> children_id;
    return VisitCXXRecordDecl(Declaration);
}

void VisitorButtomUpMatcherBase::UpdateMatchDescendants(std::uintptr_t id, std::vector<std::uintptr_t> children) {
    auto& target = Descendants[id];
    for (auto ptr: children) {
        auto& source = Descendants[ptr];
        target.insert(source.begin(), source.end());
        target.insert(ptr);
    }
}

// VisitorButtomUpFirstMatcher

bool VisitorButtomUpFirstMatcher::VisitVarDeclExec(VarDecl *var) {
    return true;
}
bool VisitorButtomUpFirstMatcher::VisitFunctionDeclExec(FunctionDecl *func){
    return true;
}
bool VisitorButtomUpFirstMatcher::VisitStmtExec(Stmt *st){
    return true;
}
bool VisitorButtomUpFirstMatcher::VisitCXXRecordDeclExec(CXXRecordDecl *Declaration){
    return true;
}

// VisitorButtomUpSecondMatcher

bool VisitorButtomUpSecondMatcher::VisitVarDeclExec(VarDecl *var) {
    return true;
}
bool VisitorButtomUpSecondMatcher::VisitFunctionDeclExec(FunctionDecl *func){
    return true;
}
bool VisitorButtomUpSecondMatcher::VisitStmtExec(Stmt *st){
    return true;
}
bool VisitorButtomUpSecondMatcher::VisitCXXRecordDeclExec(CXXRecordDecl *Declaration){
    return true;
}
