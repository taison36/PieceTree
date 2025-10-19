#include "../src/PieceTree.h"
#include "Util.h"
#include <gtest/gtest.h>
#include <ostream>

TEST(PieceTreeGettingPieces, CutRightSide) {

    PieceTree tree;

    PieceTree::Piece p1{NodeType::Added, 0, 7, {3}}; // "abc\ndef"

    /*
            "abc\n
             def"
    */
    tree.insert(p1, 0, 0);

    std::vector<PieceTree::Piece> pieces = tree.getLinePieces(0);

    ASSERT_EQ(pieces.size(), 1);

    ASSERT_EQ(pieces[0].length, 4);
    ASSERT_EQ(pieces[0].line_breaks[0], 3);
}

TEST(PieceTreeGettingPieces, CutNothing) {

    PieceTree tree;

    PieceTree::Piece p1{NodeType::Added, 0, 4, {3}}; // "abc\n"

    /*
            "abc\n"
    */
    tree.insert(p1, 0, 0);

    std::vector<PieceTree::Piece> pieces = tree.getLinePieces(0);

    ASSERT_EQ(pieces.size(), 1);

    ASSERT_EQ(pieces[0].length, 4);
    ASSERT_EQ(pieces[0].line_breaks[0], 3);
}

TEST(PieceTreeGettingPieces, LineInFewNodes) {

    PieceTree tree;

    PieceTree::Piece p1{NodeType::Added, 0, 10, {3}}; // "abc\ndef"
    PieceTree::Piece p2{NodeType::Added, 0, 8, {4}};  // "abc\ndef"

    /*
            "abc\n
             tykhon" "best\n
             def"
    */
    tree.insert(p1, 0, 0);
    tree.insert(p2, 1, 6);

    std::vector<PieceTree::Piece> pieces = tree.getLinePieces(1);

    ASSERT_EQ(pieces.size(), 2);
    ASSERT_EQ(pieces[0].length, 6);
    ASSERT_TRUE(pieces[0].line_breaks.empty());

    ASSERT_EQ(pieces[1].length, 5);
    ASSERT_EQ(pieces[1].line_breaks[0], 4);
}

TEST(PieceTreeGettingPieces, CutLeftSide) {

    PieceTree tree;

    PieceTree::Piece p1{NodeType::Added, 0, 10, {3}}; // "abc\ndef"

    /*
            "abc\n
             tykhon"
    */
    tree.insert(p1, 0, 0);

    std::vector<PieceTree::Piece> pieces = tree.getLinePieces(1);

    ASSERT_EQ(pieces.size(), 1);
    ASSERT_EQ(pieces[0].length, 6);
    ASSERT_TRUE(pieces[0].line_breaks.empty());
}
