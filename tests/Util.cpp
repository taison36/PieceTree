#include "Util.h"

#include <iostream>
#include <vector>

std::vector<PieceTree::Piece> Util::collectPieces(PieceTree &tree) {
    std::vector<PieceTree::Piece> result;
    for (auto &piece : tree) {
        result.push_back(piece);
    }
    return result;
}

void Util::printPieces(const std::vector<PieceTree::Piece>& pieces) {
    int i = 1;
    for (auto &piece : pieces) {
        std::cout << " piece " << i << " length: " << piece.length << " " << std::endl;
        for (const int &x : piece.line_breaks) {
            std::cout << " piece "<< i << " linebreak: " << x << " ";
        }
        std::cout << std::endl;
        i++;
    }
}
