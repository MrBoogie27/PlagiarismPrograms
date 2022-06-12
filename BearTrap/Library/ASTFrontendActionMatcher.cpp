//
// Created by mrboogie on 05.06.22.
//

#include "ASTFrontendActionMatcher.h"

using namespace BearTrap;

Node2NodeMap ASTFrontendActionMatcher::Match() {
    Precalc();
    MatchByHash();
    MatchByDice();
    return MatchedNodes;
}

void ASTFrontendActionMatcher::Precalc() {
    CalcHeights();
    CalcHashes();
    CalcAdjacency();
}

Node2NodeMap ASTFrontendActionMatcher::MatchByHash() {
    std::vector<std::uintptr_t> first_queue;
    std::vector<std::uintptr_t> second_queue;
    FillQueue(first_queue, 0);
    FillQueue(second_queue, 1);

    while(!first_queue.empty() && !second_queue.empty()) {
        uint32_t first_height = Attributes[0].Heights[first_queue[0]];
        uint32_t second_height = Attributes[1].Heights[second_queue[0]];

        // std::cout << "HEIGHTS " << first_height << " with " << second_height << std::endl;

        if (second_height > first_height) {
            first_queue = GetChilds(first_queue, 0);
            continue;
        }
        if (first_height > second_height) {
            second_queue = GetChilds(second_queue, 1);
            continue;
        }
        std::vector<std::uintptr_t> next_first_queue;
        std::vector<std::uintptr_t> next_second_queue;

        sort_by_hash(first_queue, 0);
        sort_by_hash(second_queue, 1);

        size_t fst_index = 0;
        size_t snd_index = 0;

        while(fst_index < first_queue.size() && snd_index < second_queue.size()) {
            std::uintptr_t fst_el = first_queue[fst_index];
            uint32_t fst_hash = Attributes[0].Hashes[fst_el];

            std::uintptr_t snd_el = second_queue[snd_index];
            uint32_t snd_hash = Attributes[1].Hashes[snd_el];

            // std::cout << "Compare " << std::hex << fst_el << " with " << snd_el << std::dec << std::endl;

            if (fst_hash == snd_hash) {
                // std::cout << "RecusiveMatch " << std::hex << fst_el << " with " << snd_el << std::dec << std::endl;
                RecusiveMatch(fst_el, snd_el);
                fst_index++;
                snd_index++;
            }
            if (fst_hash < snd_hash) {
                InsertChild(next_first_queue, fst_el, 0);
                fst_index++;
            }
            if (fst_hash > snd_hash) {
                InsertChild(next_second_queue, snd_el, 1);
                snd_index++;
            }
        }
        for (; fst_index < first_queue.size(); fst_index++) {
            InsertChild(next_first_queue, first_queue[fst_index], 0);
        }
        for (; snd_index < second_queue.size(); snd_index++) {
            InsertChild(next_second_queue, second_queue[snd_index], 1);
        }
        std::swap(first_queue, next_first_queue);
        std::swap(second_queue, next_second_queue);
    }

    // std::cout << "End Match by hash" << std::endl;

    return MatchedNodes;
}

Node2NodeMap ASTFrontendActionMatcher::MatchByDice() {
    ASTContext& first_context = ASTs[0]->getASTContext();
    ASTContext& second_context = ASTs[1]->getASTContext();
    ASTConsumerButtomUpFirstMatcher consumerButtomUp(&first_context, &second_context, MatchedNodes);
    consumerButtomUp.HandleTranslationUnit(first_context);

    return MatchedNodes;
}

void ASTFrontendActionMatcher::CalcHeights() {
    for (size_t index = 0; index < Count; index++) {
        auto consumer = CalcSome<ASTConsumerCalcHeights>(index);
        Attributes[index].Heights = consumer->GetHeights();
    }
}

void ASTFrontendActionMatcher::CalcHashes() {
    for (size_t index = 0; index < Count; index++) {
        auto consumer = CalcSome<ASTConsumerCalcHashes>(index);
        Attributes[index].Hashes = consumer->GetHashes();
    }
}

void ASTFrontendActionMatcher::CalcAdjacency() {
    for (size_t index = 0; index < Count; index++) {
        auto consumer = CalcSome<ASTConsumerCalcGraph>(index);
        Attributes[index].AdjacencyGraph = consumer->GetGraph();
    }
}

void ASTFrontendActionMatcher::RecusiveMatch(std::uintptr_t left, std::uintptr_t right) {
    MatchedNodes[left] = right;
    MatchedNodes[right] = left;

    if (!Attributes[0].Hashes.count(left) || !Attributes[1].Hashes.count(right)) {
        if (Attributes[0].Hashes.count(left) || Attributes[1].Hashes.count(right)) {
            std::cerr << "Only one empty in: " << std::hex << left << " " << right << std::dec << "\n";
        }
        return;
    }
    assert(Attributes[0].Hashes[left] == Attributes[1].Hashes[right]);
    auto& left_child = Attributes[0].AdjacencyGraph[left];
    auto& right_child = Attributes[1].AdjacencyGraph[right];

    sort_by_hash(left_child, 0);
    sort_by_hash(right_child, 1);

    if (!VectorChildCompare(left_child, right_child)) {
        std::cerr << "Found collision:\nleft:\n";
        for (auto el: left_child) {
            std::cerr << std::hex << el << " with " << std::dec << Attributes[0].Hashes[el] << std::endl;
        }
        std::cerr << "right:\n";
        for (auto el: right_child) {
            std::cerr << std::hex << el << " with " << std::dec << Attributes[1].Hashes[el] << std::endl;
        }
        return;
    }

    for (size_t index = 0; index < left_child.size(); index++) {
        auto lci = left_child[index];
        auto rci = right_child[index];

        RecusiveMatch(lci, rci);
    }
}

void ASTFrontendActionMatcher::sort_by_hash(std::vector<std::uintptr_t>& vec, size_t index) {
    auto hash_comparison = [this, &index](std::uintptr_t first, std::uintptr_t second) {
        return Attributes[index].Hashes[first] < Attributes[index].Hashes[second];
    };
    std::sort(vec.begin(), vec.end(), hash_comparison);
};

bool ASTFrontendActionMatcher::VectorChildCompare(const std::vector<std::uintptr_t>& first, const std::vector<std::uintptr_t>& second) {
    if (first.size() != second.size()) {
        return false;
    }

    for (size_t index = 0; index < first.size(); index++) {
        if (Attributes[0].Hashes[first[index]] != Attributes[1].Hashes[second[index]]) {
            return false;
        }
    }

    return true;
}

void ASTFrontendActionMatcher::FillQueue(std::vector<std::uintptr_t>& queue, size_t index) {
    uint32_t max_height = 0;
    for (auto& [_, height]: Attributes[index].Heights) {
        max_height = std::max(max_height, height);
    }
    for (auto& [key, height]: Attributes[index].Heights) {
        if (height == max_height) {
            queue.push_back(key);
        }
    }
}

void ASTFrontendActionMatcher::InsertChild(std::vector<std::uintptr_t>& vec, std::uintptr_t key, size_t index) {
    const auto& child = Attributes[index].AdjacencyGraph[key];
    vec.insert(vec.end(), child.begin(), child.end());
}

std::vector<std::uintptr_t> ASTFrontendActionMatcher::GetChilds(const std::vector<std::uintptr_t>& queue, size_t index) {
    std::vector<std::uintptr_t> answer;

    for (auto el: queue) {
        InsertChild(answer, el, index);
    }

    return answer;
}