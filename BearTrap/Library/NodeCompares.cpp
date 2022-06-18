//
// Created by mrboogie on 06.06.22.
//

#include "NodeCompares.h"

namespace BearTrap {
    double Compare(VarDecl *first, VarDecl *second) {
        return 1.0;
    }

    double Compare(FunctionDecl *first, FunctionDecl *second) {
        return 1.0;
    }

    double Compare(Decl *first, Decl *second) {
#define COMPAREDECL(CLASS)                                 \
  case Decl::CLASS:                                        \
        return Compare(static_cast<CLASS##Decl *>(first), static_cast<CLASS##Decl *>(second));

        switch (first->getKind()) {
            COMPAREDECL(Var)
            COMPAREDECL(Function)
            COMPAREDECL(CXXRecord)
            default:
                return 1.0;
        }
#undef COMPAREDECL
    }

    double Compare(Stmt *first, Stmt *second) {
#define COMPARESTMT(CLASS)                                 \
  case Stmt::CLASS##Class:                                        \
        return Compare(static_cast<CLASS *>(first), static_cast<CLASS *>(second));
        switch (first->getStmtClass()) {
            COMPARESTMT(IntegerLiteral)
            COMPARESTMT(UnaryOperator)
            COMPARESTMT(CXXConstructExpr)
            COMPARESTMT(CallExpr)
            COMPARESTMT(WhileStmt)
            COMPARESTMT(ForStmt)
            COMPARESTMT(IfStmt)
            default:
                return 1.0;
        }
#undef COMPARESTMT
    }

    double Compare(CXXRecordDecl *first, CXXRecordDecl *second) {
        return 1.0;
    }

    double Compare(IntegerLiteral *first, IntegerLiteral *second) {
        return 1.0;
    }

    double Compare(UnaryOperator *first, UnaryOperator *second) {
        return 1.0;
    }

    double Compare(CXXConstructExpr *first, CXXConstructExpr *second) {
        return 1.0;
    }

    double Compare(CallExpr *first, CallExpr *second) {
        return 1.0;
    }

    double Compare(WhileStmt *first, WhileStmt *second) {
        return 1.0;
    }

    double Compare(ForStmt *first, ForStmt *second) {
        return 1.0;
    }

    double Compare(IfStmt *first, IfStmt *second) {
        return 1.0;
    }
}
