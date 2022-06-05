//
// Created by mrboogie on 31.05.22.
//

#include "Library/HeightCalc.h"
#include "Library/TopDownMatcher.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "../CommonLib/CalcHashes.h"

using namespace clang;
using namespace clang::tooling;

static llvm::cl::OptionCategory MyToolCategory("Additional options");

int main(int argc, const char **argv) {
    CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
    ClangTool Tool(OptionsParser.getCompilations(),
                   OptionsParser.getSourcePathList());

    int result = Tool.run(newFrontendActionFactory<ASTFrontendCalcHeights>().get());
    if (result) {
        return result;
    }

    auto frontendHashes = newFrontendActionFactory<ASTFrontendCalcHashes>();
    result = Tool.run(frontendHashes.get());
    if (result) {
        return result;
    }

    result = Tool.run(newTopDownFrontendActionFactory(ASTConsumerCalcHashes::allHashes).get());
//    std::vector<std::unique_ptr<ASTUnit>> ASTs;
//    Tool.buildASTs(ASTs);

    return result;
}
