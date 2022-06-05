//
// Created by mrboogie on 04.06.22.
//

#include "CalcHashes.h"

std::vector<std::vector<uint32_t> > VisitorCalcHashes::ps_pows;
std::map<std::string, PtrToUintMap> ASTConsumerCalcHashes::allHashes;

void VisitorCalcHashes::ExpandPowPrimes(size_t length) {
    if (ps_pows.empty()) {
        ps_pows.resize(Ps.size());
    }
    size_t max_size = ps_pows.begin()->size();
    if (max_size >= length) {
        return;
    }

    for (size_t index_p = 0u; index_p < ps_pows.size(); index_p++) {
        ps_pows[index_p].resize(length);
        for (size_t index_element = max_size; index_element < length; index_element++) {
            if (index_element) {
                uint64_t new_value = ps_pows[index_p][index_element - 1u] * Ps[index_element];
                ps_pows[index_p][index_element] = new_value % MOD;
            } else {
                ps_pows[index_p][index_element] = 1u;
            }
        }
    }
}

uint32_t VisitorCalcHashes::GetPPower(size_t num_p, size_t power) {
    if (ps_pows[num_p].size() < power) {
        ExpandPowPrimes(2 * power);
    }
    return ps_pows[num_p][power];
}

uint32_t VisitorCalcHashes::AddByModule(uint32_t left, uint32_t right) {
    uint64_t answer = left + right;
    return answer % MOD;
}
uint32_t VisitorCalcHashes::MultiplyByModule(uint32_t left, uint32_t right) {
    uint64_t answer = left * right;
    return answer % MOD;
}

uint32_t VisitorCalcHashes::HashFunction(std::vector<uint32_t> elems) {
    std::sort(elems.begin(), elems.end());
    uint32_t hash = 0u;
    for (size_t index = 0; index < elems.size(); index++) {
        hash = AddByModule(hash, MultiplyByModule(elems[index], elems[index]));
        hash = AddByModule(hash, MultiplyByModule(elems[index], GetPPower(0, index + 1)));
        hash = AddByModule(hash, GetPPower(1, 0));
    }
    return hash;
}

bool VisitorCalcHashes::VisitVarDecl(VarDecl *var) {
    if (!Context->getSourceManager().isInMainFile(var->getLocation())) {
        return true;
    }

    std::vector<uint32_t> children_id;

    if (Stmt* S = var->getInit()) {
        std::uintptr_t temp_id = reinterpret_cast<std::uintptr_t>(S);
        if (Hashes.count(temp_id)) {
            children_id.push_back(Hashes[temp_id]);
        }
    }

    children_id.push_back(START_WEIGHT_DECLARATION + var->getKind());

    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(var);
    Hashes[id] = HashFunction(children_id);
    return true;
}

bool VisitorCalcHashes::VisitFunctionDecl(FunctionDecl *func) {
    if (!Context->getSourceManager().isInMainFile(func->getLocation())) {
        return true;
    }
    std::vector<uint32_t> children_id;

    if (Stmt* S = func->getBody()) {
        std::uintptr_t temp_id = reinterpret_cast<std::uintptr_t>(S);
        if (Hashes.count(temp_id)) {
            children_id.push_back(Hashes[temp_id]);
        }
    }
    children_id.push_back(START_WEIGHT_DECLARATION + func->getKind());

    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(func);
    Hashes[id] = HashFunction(children_id);

    return true;
}

bool VisitorCalcHashes::VisitStmt(Stmt *st) {
    if (!Context->getSourceManager().isInMainFile(st->getBeginLoc())) {
        return true;
    }

    std::vector<uint32_t> children_id;

    uint32_t cur_height = 0u;
    if (st->getStmtClass() == Stmt::DeclStmtClass) {
        DeclStmt *newD = static_cast<DeclStmt*>(st);
        for (auto *I : newD->decls()) {
            std::uintptr_t temp_id = reinterpret_cast<std::uintptr_t>(I);
            if (Hashes.count(temp_id)) {
                children_id.push_back(Hashes[temp_id]);
            }
        }
    }
    for (auto D = st->child_begin(); D != st->child_end(); D++) {
        std::uintptr_t temp_id = reinterpret_cast<std::uintptr_t>(*D);
        if (Hashes.count(temp_id)) {
            children_id.push_back(Hashes[temp_id]);
        }
    }
    children_id.push_back(START_WEIGHT_SOMETHING + st->getStmtClass());

    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(st);
    Hashes[id] = HashFunction(children_id);

    return true;
}

bool VisitorCalcHashes::VisitCXXRecordDecl(CXXRecordDecl *Declaration) {
    if (!Context->getSourceManager().isInMainFile(Declaration->getLocation())) {
        return true;
    }

    std::vector<uint32_t> children_id;

    children_id.push_back(START_WEIGHT_DECLARATION + Declaration->getKind());

    return true;
}

void ASTConsumerCalcHashes::HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());

    allHashes[currentFile] = Visitor.GetHashes();

    Context.getTranslationUnitDecl()->dump();
    llvm::outs().flush();
    for (auto& [key, value]: Visitor.GetHashes()) {
        std::cout << std::hex << key << " " << std::dec << value << "\n";
    }
    std::cout.flush();
}
