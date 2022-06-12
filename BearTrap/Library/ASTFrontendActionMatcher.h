//
// Created by mrboogie on 05.06.22.
//

#ifndef PLAGIARISMPROGRAMS_ASTFRONTENDACTIONMATCHER_H
#define PLAGIARISMPROGRAMS_ASTFRONTENDACTIONMATCHER_H

#include <queue>
#include "CommonInclude.h"
#include "HeightCalc.h"
#include "../../CommonLib/CalcHashes.h"
#include "GraphCalc.h"
#include "BottomUpMatcher.h"

namespace BearTrap {
    using namespace clang;

    class ASTFrontendActionMatcher {
    public:
        struct ASTAttribute {
            PtrToUintMap Heights;
            PtrToUintMap Hashes;
            Node2Nodes AdjacencyGraph;
        };
    public:
        explicit ASTFrontendActionMatcher(ASTUnitPtrs &ASTs)
                : ASTs(ASTs), Count(ASTs.size()) {
            Attributes.resize(ASTs.size());
        }

        Node2NodeMap GetMatchedNodes() {
            return MatchedNodes;
        }

        std::vector<ASTAttribute> GetASTAttribute() {
            return Attributes;
        }

        Node2NodeMap Match();

        void Precalc();

        Node2NodeMap MatchByHash();

        Node2NodeMap MatchByDice();

        void CalcHeights();

        void CalcHashes();

        void CalcAdjacency();

        template<class SomeASTConsumer>
        std::shared_ptr<SomeASTConsumer> CalcSome(size_t index) {
            ASTContext &current_context = ASTs[index]->getASTContext();
            auto consumer = std::make_shared<SomeASTConsumer>(&current_context);
            consumer->HandleTranslationUnit(current_context);
            return consumer;
        }

    private:
        ASTUnitPtrs &ASTs;
        size_t Count;
        Node2NodeMap MatchedNodes;
        std::vector<ASTAttribute> Attributes;
    private:
        void RecusiveMatch(std::uintptr_t left, std::uintptr_t right);

        void sort_by_hash(std::vector<std::uintptr_t> &vec, size_t index);

        bool VectorChildCompare(const std::vector<std::uintptr_t> &first, const std::vector<std::uintptr_t> &second);

        void FillQueue(std::vector<std::uintptr_t> &queue, size_t index);

        void InsertChild(std::vector<std::uintptr_t> &vec, std::uintptr_t key, size_t index);

        std::vector<std::uintptr_t> GetChilds(const std::vector<std::uintptr_t> &queue, size_t index);
    };
}

#endif //PLAGIARISMPROGRAMS_ASTFRONTENDACTIONMATCHER_H
