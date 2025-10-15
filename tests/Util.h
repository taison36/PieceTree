//
// Created by Tykhon Korol on 13.10.25.
//

#ifndef UTIL_H
#define UTIL_H
#include "PieceTree.h"

#include <vector>

class Util {
public:
    static std::vector<PieceTree::Piece> collectPieces(PieceTree &tree);
    static void printPieces(const std::vector<PieceTree::Piece>& pieces);
};



#endif //UTIL_H
