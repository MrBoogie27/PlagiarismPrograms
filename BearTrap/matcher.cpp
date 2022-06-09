//
// Created by mrboogie on 31.05.22.
//

#include "Library/HeightCalc.h"
#include "Library/TopDownMatcher.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "../CommonLib/CalcHashes.h"
#include "Library/ASTFrontendActionMatcher.h"
#include "Library/ASTFrontendActionEditScript.h"

using namespace clang;
using namespace clang::tooling;

static llvm::cl::OptionCategory MyToolCategory("Additional options");

int main(int argc, const char **argv) {
    CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
    ClangTool Tool(OptionsParser.getCompilations(),
                   OptionsParser.getSourcePathList());

    std::vector<std::unique_ptr<ASTUnit>> ASTs;
    Tool.buildASTs(ASTs);

    ASTFrontendActionMatcher matcher(ASTs);
    auto matches = matcher.Match();

    std::cout << "MATCH:\n";
    std::cout << std::hex;
    for(auto& [left, right]: matches) {
        std::cout << left << " : " << right << std::endl;
    }
    std::cout << std::dec;

    auto attributes = matcher.GetASTAttribute();
    std::vector<PtrToUintMap> hashes = {attributes[0].Hashes, attributes[1].Hashes};
    ASTFrontendActionEditScript editScriptBuilder(ASTs, hashes, matches);
    double similarity = editScriptBuilder.GenerateEditScript();

    std::cout << "SIMILARITY: " << similarity << std::endl;

    return 0;
}
