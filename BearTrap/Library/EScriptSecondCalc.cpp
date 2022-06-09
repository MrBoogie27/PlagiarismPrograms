//
// Created by mrboogie on 09.06.22.
//

#include "EScriptSecondCalc.h"

bool EScriptSecondCalc::VisitDecl(Decl *decl)
{
    if (!Context->getSourceManager().isInMainFile(decl->getLocation())) {
        return true;
    }
    std::cout << "I'm in EScriptSecondCalc::VisitDecl" << std::endl;
    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(decl);
    if (!Hash.count(id)) {
        std::cout << "no hash" << std::endl;
        return true;
    }
    std::cout << "have hash" << std::endl;
    if (!MatchedNodes.count(id)) {
        std::cout << "no match" << std::endl;
        EditScript.push_back({EditModes::Delete, 1.0});
    }

    return true;
}

bool EScriptSecondCalc::VisitStmt(Stmt *S)
{
    if (!Context->getSourceManager().isInMainFile(S->getBeginLoc())) {
        return true;
    }

    std::cout << "I'm in EScriptSecondCalc::VisitStmt" << std::endl;
    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(S);
    if (!Hash.count(id)) {
        std::cout << "no hash" << std::endl;
        return true;
    }
    std::cout << "have hash" << std::endl;
    if (!MatchedNodes.count(id)) {
        std::cout << "no match" << std::endl;
        EditScript.push_back({EditModes::Delete, 1.0});
    }

    return true;
}
