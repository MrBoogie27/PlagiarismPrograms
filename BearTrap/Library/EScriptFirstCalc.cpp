//
// Created by mrboogie on 05.06.22.
//

#include "EScriptFirstCalc.h"
#include "NodeCompares.h"

bool EScriptFirstCalc::VisitDecl(Decl *decl)
{
    if (!Context->getSourceManager().isInMainFile(decl->getLocation())) {
        return true;
    }
    std::cout << "I'm in EScriptFirstCalc::VisitDecl" << std::endl;
    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(decl);
    if (!Hashes[1].count(id)) {
        std::cout << "no hash" << std::endl;
        return true;
    }
    std::cout << "have hash" << std::endl;
    if (MatchedNodes.count(id)) {
        std::cout << "have match" << std::endl;
        double similarity = 1.0;
        std::uintptr_t matchedId = MatchedNodes.at(id);
        if (Hashes[1].at(id) != Hashes[0].at(matchedId)) {
            std::cout << "no match hash" << std::endl;
            similarity = Compare(decl, reinterpret_cast<Decl *>(matchedId));
        }
        double weight = 1.0 - similarity;
        EditScript.push_back({EditModes::Update, weight});
    } else {
        std::cout << "no match" << std::endl;
        EditScript.push_back({EditModes::Add, 1.0});
    }

    return true;
}

bool EScriptFirstCalc::VisitStmt(Stmt *S)
{
    if (!Context->getSourceManager().isInMainFile(S->getBeginLoc())) {
        return true;
    }

    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(S);
    if (!Hashes[1].count(id)) {
        return true;
    }
    std::cout << "I'm in EScriptFirstCalc::VisitStmt" << std::endl;
    if (!Hashes[1].count(id)) {
        std::cout << "no hash" << std::endl;
        return true;
    }
    std::cout << "have hash" << std::endl;
    if (MatchedNodes.count(id)) {
        std::cout << "have match" << std::endl;
        double similarity = 1.0;
        std::uintptr_t matchedId = MatchedNodes.at(id);
        if (Hashes[1].at(id) != Hashes[0].at(matchedId)) {
            std::cout << "no match hash" << std::endl;
            similarity = Compare(S, reinterpret_cast<Stmt *>(matchedId));
        }
        double weight = 1.0 - similarity;
        EditScript.push_back({EditModes::Update, weight});
    } else {
        std::cout << "no match" << std::endl;
        EditScript.push_back({EditModes::Add, 1.0});
    }

    return true;
}
