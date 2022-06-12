//
// Created by mrboogie on 09.06.22.
//

#ifndef PLAGIARISMPROGRAMS_ASTFRONTENDACTIONEDITSCRIPT_H
#define PLAGIARISMPROGRAMS_ASTFRONTENDACTIONEDITSCRIPT_H

#include "CommonInclude.h"

namespace BearTrap {
    using namespace clang;

    class ASTFrontendActionEditScript {
    public:
        explicit ASTFrontendActionEditScript(ASTUnitPtrs &ASTs,
                                             const std::vector<PtrToUintMap> &Hashes,
                                             const Node2NodeMap &MatchedNodes)
                : ASTs(ASTs), MatchedNodes(MatchedNodes), Hashes(Hashes), Similarity(0.0) {
            assert(ASTs.size() == Hashes.size());
        }

        double GenerateEditScript();

        double GetLastSimilarity() const;

        ScriptStruct GetEditScript() const;

    private:
        ASTUnitPtrs &ASTs;
        const Node2NodeMap &MatchedNodes;
        const std::vector<PtrToUintMap> &Hashes;
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
}

#endif //PLAGIARISMPROGRAMS_ASTFRONTENDACTIONEDITSCRIPT_H
