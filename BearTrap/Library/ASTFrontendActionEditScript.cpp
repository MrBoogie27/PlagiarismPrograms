//
// Created by mrboogie on 09.06.22.
//

#include "ASTFrontendActionEditScript.h"
#include "EScriptFirstCalc.h"
#include "EScriptSecondCalc.h"

using namespace BearTrap;

double ASTFrontendActionEditScript::GenerateEditScript() {
    EditScript.clear();
    Similarity = 0.0;
    ProcessFirstTree();
    ProcessSecondTree();
    CaclSimilarity();
    return GetLastSimilarity();
}

void ASTFrontendActionEditScript::CaclSimilarity() {
    size_t allCount = Hashes[0].size() + Hashes[1].size();
    Similarity = 0.0;
    assert(allCount >= EditScript.size());
    for (auto& [action, weight] : EditScript) {
        Similarity += weight;
    }
    if (allCount) {
        Similarity = 1.0 - Similarity / allCount;
    }
    assert(Similarity + EPS >= 0.0 && Similarity <= 1.0 + EPS);
}

double ASTFrontendActionEditScript::GetLastSimilarity() const {
    return Similarity;
}

ScriptStruct ASTFrontendActionEditScript::GetEditScript() const {
    return EditScript;
}

void ASTFrontendActionEditScript::ProcessFirstTree() {
    ASTContext& context = ASTs[1]->getASTContext();
    ASTConsumerEScriptFirstCalc consumer(&context, MatchedNodes, Hashes);
    consumer.HandleTranslationUnit(context);
    auto tmpEditScript = consumer.GetEditScript();
    EditScript.insert(EditScript.end(), tmpEditScript.begin(), tmpEditScript.end());
}

void ASTFrontendActionEditScript::ProcessSecondTree() {
    ASTContext& context = ASTs[0]->getASTContext();

    ASTConsumerEScriptSecondCalc consumer(&context, MatchedNodes, Hashes[0]);
    consumer.HandleTranslationUnit(context);
    auto tmpEditScript = consumer.GetEditScript();
    EditScript.insert(EditScript.end(), tmpEditScript.begin(), tmpEditScript.end());
}
