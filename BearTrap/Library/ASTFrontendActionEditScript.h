//
// Created by mrboogie on 09.06.22.
//

#ifndef PLAGIARISMPROGRAMS_ASTFRONTENDACTIONEDITSCRIPT_H
#define PLAGIARISMPROGRAMS_ASTFRONTENDACTIONEDITSCRIPT_H

#include <queue>
#include "CommonInclude.h"
#include "HeightCalc.h"
#include "../../CommonLib/CalcHashes.h"
#include "GraphCalc.h"
#include "BottomUpMatcher.h"
#include "EditModes.h"

using namespace clang;
using namespace clang::tooling;

using ASTUnitPtr = std::unique_ptr<ASTUnit>;
using ASTUnitPtrs = std::vector<ASTUnitPtr>;

using Node2NodeMap = std::unordered_map<std::uintptr_t, std::uintptr_t>;
using PtrToUintMap = std::unordered_map<std::uintptr_t, uint32_t>;
using Node2Nodes = std::unordered_map<std::uintptr_t, std::vector<std::uintptr_t> >;
using ScriptStruct = std::vector<std::pair<EditModes, double> >;

class ASTFrontendActionEditScript{
public:
    explicit ASTFrontendActionEditScript(ASTUnitPtrs& ASTs,
                                         const std::vector<PtrToUintMap>& Hashes,
                                         const Node2NodeMap& MatchedNodes)
            : ASTs(ASTs)
            , MatchedNodes(MatchedNodes)
            , Hashes(Hashes)
            , Similarity(0.0) {
        assert(ASTs.size() == Hashes.size());
    }

    double GenerateEditScript();
    double GetLastSimilarity() const;
    ScriptStruct GetEditScript() const;

private:
    ASTUnitPtrs& ASTs;
    const Node2NodeMap& MatchedNodes;
    const std::vector<PtrToUintMap>& Hashes;
    double Similarity;
    ScriptStruct EditScript;
    static constexpr double EPS = 1e-12;
private:
    // Add, update, move
    void ProcessFirstTree();

    // Delete
    void ProcessSecondTree();

    void CaclSimilarity();
};

#endif //PLAGIARISMPROGRAMS_ASTFRONTENDACTIONEDITSCRIPT_H
