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

    double Compare(IntegerLiteral *first, IntegerLiteral *second);

    double Compare(UnaryOperator *first, UnaryOperator *second);

    double Compare(CXXConstructExpr *first, CXXConstructExpr *second);

    double Compare(CallExpr *first, CallExpr *second);

    double Compare(WhileStmt *first, WhileStmt *second);

    double Compare(ForStmt *first, ForStmt *second);

    double Compare(IfStmt *first, IfStmt *second);
}

#endif //PLAGIARISMPROGRAMS_NODECOMPARES_H
