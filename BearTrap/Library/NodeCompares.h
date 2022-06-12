//
// Created by mrboogie on 06.06.22.
//

#ifndef PLAGIARISMPROGRAMS_NODECOMPARES_H
#define PLAGIARISMPROGRAMS_NODECOMPARES_H

#include "CommonInclude.h"

namespace BearTrap {
    using namespace clang;

    double Compare(VarDecl *first, VarDecl *second);

    double Compare(FunctionDecl *first, FunctionDecl *second);

    double Compare(Decl *first, Decl *second);

    double Compare(Stmt *first, Stmt *second);

    double Compare(CXXRecordDecl *first, CXXRecordDecl *second);
}

#endif //PLAGIARISMPROGRAMS_NODECOMPARES_H
