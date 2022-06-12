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
        switch (first->getKind()) {
            case Decl::Var:
                return Compare(static_cast<VarDecl *>(first), static_cast<VarDecl *>(second));
            case Decl::Function:
                return Compare(static_cast<FunctionDecl *>(first), static_cast<FunctionDecl *>(second));
            case Decl::CXXRecord:
                return Compare(static_cast<CXXRecordDecl *>(first), static_cast<CXXRecordDecl *>(second));
            default:
                return 1.0;
        }
    }

    double Compare(Stmt *first, Stmt *second) {
        return 1.0;
    }

    double Compare(CXXRecordDecl *first, CXXRecordDecl *second) {
        return 1.0;
    }
}
