//
// Created by mrboogie on 05.06.22.
//

#include "BottomUpMatcher.h"
#include "NodeCompares.h"

// VisitorButtomUpMatcherBase
bool VisitorButtomUpMatcherBase::VisitVarDecl(VarDecl *var) {
    if (!Context->getSourceManager().isInMainFile(var->getLocation())) {
        return true;
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
        return true;
    }
    std::vector<std::uintptr_t> children_id;
    if (Stmt* S = func->getBody()) {
        std::uintptr_t temp_id = reinterpret_cast<std::uintptr_t>(S);
        children_id.push_back(temp_id);
    }
    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(func);
    UpdateMatchDescendants(id, children_id);

    return VisitFunctionDeclExec(func);
}

bool VisitorButtomUpMatcherBase::VisitStmt(Stmt *st) {
    if (!Context->getSourceManager().isInMainFile(st->getBeginLoc())) {
        return true;
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

    return VisitStmtExec(st);
}

bool VisitorButtomUpMatcherBase::VisitCXXRecordDecl(CXXRecordDecl *Declaration) {
    if (!Context->getSourceManager().isInMainFile(Declaration->getLocation())) {
        return true;
    }
    std::vector<std::uintptr_t> children_id;
    return VisitCXXRecordDeclExec(Declaration);
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

std::unordered_set<std::uintptr_t> VisitorButtomUpFirstMatcher::GetChildMatch(std::uintptr_t node) {
    std::unordered_set<std::uintptr_t> answer;
    for (auto elem: Descendants[node]) {
        if (Matchers.count(elem)) {
            answer.insert(Matchers.at(elem));
        }
    }
    return answer;
}

bool VisitorButtomUpFirstMatcher::VisitVarDeclExec(VarDecl *var) {
    return VisitAllNode(var);
}
bool VisitorButtomUpFirstMatcher::VisitFunctionDeclExec(FunctionDecl *func){
    return VisitAllNode(func);
}
bool VisitorButtomUpFirstMatcher::VisitStmtExec(Stmt *st){
    return VisitAllNode(st);
}
bool VisitorButtomUpFirstMatcher::VisitCXXRecordDeclExec(CXXRecordDecl *Declaration){
    return VisitAllNode(Declaration);
}

// VisitorButtomUpSecondMatcher

double VisitorButtomUpSecondMatcher::Dice(std::uintptr_t currentNode) const {
    const auto& currentDesc = Descendants.at(currentNode);
    std::cout << "len Desc: " << currentDesc.size() << std::endl;
    uint32_t countMatch = 0;
    for (auto child: currentDesc) {
        countMatch += NodeChildMatch.count(child);
    }

    return 2.0 * countMatch / (currentDesc.size() + NodeCountDesc);
}

void VisitorButtomUpSecondMatcher::UpdateBest(std::uintptr_t currentNode, double dice) {
    if (dice > BestNode.second) {
        BestNode = { currentNode, dice };
    }
}

bool VisitorButtomUpSecondMatcher::VisitDeclExec(Decl *decl) {
    if (OriginalNode.index() != 0) {
        return true;
    }
    auto declNode = std::get<Decl*>(OriginalNode);
    std::cout << "classes: " << declNode->getDeclKindName() << " " << decl->getDeclKindName() << std::endl;
    if (declNode->getKind() != decl->getKind()) {
        return true;
    }
    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(decl);
    bool nodeIsInternal = static_cast<bool>(NodeCountDesc);
    bool currentIsInternal = !Descendants[id].empty();
    std::cout << "is Leaf: " << nodeIsInternal << " " << currentIsInternal << std::endl;
    if (nodeIsInternal != currentIsInternal) {
        return true;
    }
    std::cout << "Compare two node: " << std::hex << reinterpret_cast<std::uintptr_t>(declNode) << " " << id << std::dec << std::endl;
    double dice = Compare(declNode, decl);
    if (nodeIsInternal) {
        dice = (dice + Dice(id)) / 2.0;
    }
    std::cout << "Get Dice " << dice << " for VisitDecl " << std::hex << id << std::dec << std::endl;
    UpdateBest(id, dice);

    return true;
}

bool VisitorButtomUpSecondMatcher::VisitVarDeclExec(VarDecl *var) {
    return VisitDeclExec(var);
}
bool VisitorButtomUpSecondMatcher::VisitFunctionDeclExec(FunctionDecl *func){
    return VisitDeclExec(func);
}
bool VisitorButtomUpSecondMatcher::VisitStmtExec(Stmt *st){
    if (OriginalNode.index() != 1) {
        return true;
    }
    auto stmtNode = std::get<Stmt*>(OriginalNode);
    std::cout << "classes: " << stmtNode->getStmtClassName() << " " << st->getStmtClassName() << std::endl;
    if (stmtNode->getStmtClass() != st->getStmtClass()) {
        return true;
    }
    std::cout << "get class" << std::endl;
    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(st);
    bool nodeIsInternal = static_cast<bool>(NodeCountDesc);
    bool currentIsInternal = !Descendants[id].empty();
    std::cout << "is Leaf: " << nodeIsInternal << " " << currentIsInternal << std::endl;
    if (nodeIsInternal != currentIsInternal) {
        return true;
    }
    std::cout << "Compare two node: " << std::hex << reinterpret_cast<std::uintptr_t>(stmtNode) << " " << id << std::dec << std::endl;
    double dice = Compare(stmtNode, st);
    if (nodeIsInternal) {
        dice = (dice + Dice(id)) / 2.0;
    }
    std::cout << "Get Dice " << dice << " for VisitStmt " << std::hex << id << std::dec << std::endl;
    UpdateBest(id, dice);

    return true;
}
bool VisitorButtomUpSecondMatcher::VisitCXXRecordDeclExec(CXXRecordDecl *Declaration){
    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(Declaration);

    return true;
}
