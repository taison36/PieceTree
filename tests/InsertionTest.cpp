#include "../include/PieceTree.h"
#include "Util.h"
#include <gtest/gtest.h>
#include <iostream>
#include <ostream>

TEST(PieceTreeTest, LineInsertion) {
    PieceTree tree;

    PieceTree::Piece p1{NodeType::Added, 0, 7, {3}};     // "abc\ndef"
    PieceTree::Piece p2{NodeType::Added, 7, 5, {2}};     // "gh\njk"

/*
        "abc\n"
        "gh\n
        jk" "def"
*/
    tree.insert(p1, 0, 0);
    tree.insert(p2, 1, 0);

    // Collect pieces via iterator (in-order traversal)
    std::vector<PieceTree::Piece> pieces = Util::collectPieces(tree);

    ASSERT_EQ(pieces.size(), 3);

    ASSERT_EQ(pieces[0].length, 4);
    ASSERT_EQ(pieces[0].line_breaks[0], 3);

    ASSERT_EQ(pieces[1].length, 5);
    ASSERT_EQ(pieces[1].line_breaks[0], 2);

    ASSERT_EQ(pieces[2].length, 3);
}
TEST(PieceTreeTest, InsertAtBeginning) {
    PieceTree tree;

    PieceTree::Piece p1{NodeType::Added, 0, 5, {2}};  // "ab\ncd"
    /*
        "ab\n"
        "cd"
    */
    tree.insert(p1, 0, 0);

    std::vector<PieceTree::Piece> pieces = Util::collectPieces(tree);

    ASSERT_EQ(pieces.size(), 1);
    ASSERT_EQ(pieces[0].length, 5);
    ASSERT_EQ(pieces[0].line_breaks[0], 2);
}

TEST(PieceTreeTest, InsertAtEnd) {
    PieceTree tree;

    PieceTree::Piece p1{NodeType::Added, 0, 5, {2}};  // "ab\ncd"
    /*
        "ab\n
         cd"
    */

    PieceTree::Piece p2{NodeType::Added, 5, 3, {}};  // "efg" appended
    /*
        Insert "efg" at the end:
        "ab\n
         cd" "efg"
    */

    tree.insert(p1, 0, 0);
    tree.insert(p2, 1, 2);

    std::vector<PieceTree::Piece> pieces = Util::collectPieces(tree);

    ASSERT_EQ(pieces.size(), 2);

    ASSERT_EQ(pieces[0].length, 5);
    ASSERT_EQ(pieces[0].line_breaks[0], 2);

    ASSERT_EQ(pieces[1].length, 3);
    ASSERT_TRUE(pieces[1].line_breaks.empty());
}

TEST(PieceTreeTest, InsertEmptyPiece) {
    PieceTree tree;

    PieceTree::Piece p1{NodeType::Added, 0, 0, {}};  // empty piece
    /*
        ""
    */
    tree.insert(p1, 0, 0);

    std::vector<PieceTree::Piece> pieces = Util::collectPieces(tree);

    ASSERT_EQ(pieces.size(), 1);
    ASSERT_EQ(pieces[0].length, 0);
    ASSERT_TRUE(pieces[0].line_breaks.empty());
}

TEST(PieceTreeTest, SplitAtLineBreak) {
    PieceTree tree;

    PieceTree::Piece p1{NodeType::Added, 0, 7, {3}};  // "abc\ndef"
    /*
        "abc\n"
        "def"
    */

    PieceTree::Piece p2{NodeType::Added, 7, 2, {}};  // "gh"
    /*
        Insert "gh" at line break:
        "abc\n"
        "gh""def"
    */

    tree.insert(p1, 0, 0);
    tree.insert(p2, 1, 0);

    std::vector<PieceTree::Piece> pieces = Util::collectPieces(tree);

    ASSERT_EQ(pieces.size(), 3);
    ASSERT_EQ(pieces[0].length, 4);
    ASSERT_EQ(pieces[0].line_breaks.size(), 1);
    ASSERT_EQ(pieces[0].line_breaks[0], 3);

    ASSERT_EQ(pieces[1].length, 2);
    ASSERT_TRUE(pieces[1].line_breaks.empty());

    ASSERT_EQ(pieces[2].length, 3);
    ASSERT_TRUE(pieces[2].line_breaks.empty());
}

TEST(PieceTreeTest, ConsecutiveLineBreaks) {
    PieceTree tree;

    PieceTree::Piece p1{NodeType::Added, 0, 5, {1, 2}};  // "\n\nabc"
    /*
        "\n
         \n
         abc"
    */
    tree.insert(p1, 0, 0);

    std::vector<PieceTree::Piece> pieces = Util::collectPieces(tree);

    ASSERT_EQ(pieces.size(), 1);
    ASSERT_EQ(pieces[0].length, 5);
    ASSERT_EQ(pieces[0].line_breaks.size(), 2);
    ASSERT_EQ(pieces[0].line_breaks[0], 1);
    ASSERT_EQ(pieces[0].line_breaks[1], 2);
}

TEST(PieceTreeTest, InsertFirstLine) {
    PieceTree tree;

    PieceTree::Piece p1{NodeType::Added, 0, 4, {2}}; // "ab\nc"
    /*
        "ab\n
         c"
    */

    PieceTree::Piece p2{NodeType::Added, 4, 3, {}};  // "def"
    /*
        Insert "def" on first line at offset 1:
        "a" "def" "b\n
         c"
    */

    tree.insert(p1, 0, 0);
    tree.insert(p2, 0, 1);

    std::vector<PieceTree::Piece> pieces = Util::collectPieces(tree);

    ASSERT_EQ(pieces.size(), 3);
    ASSERT_EQ(pieces[0].length, 1);  // left of insertion
    ASSERT_EQ(pieces[1].length, 3);  // inserted + right
    ASSERT_EQ(pieces[2].length, 3);  // inserted + right
    ASSERT_EQ(pieces[2].line_breaks.size(), 1);
    ASSERT_EQ(pieces[2].line_breaks[0], 1);
}

TEST(PieceTreeTest, MultipleInsertsSameLine) {
    PieceTree tree;

    PieceTree::Piece p1{NodeType::Added, 0, 4, {2}}; // "ab\nc"
    /*
        "ab\n"
        "c"
    */

    PieceTree::Piece p2{NodeType::Added, 4, 3, {}};  // "def"
    PieceTree::Piece p3{NodeType::Added, 7, 3, {}};  // "def"


    tree.insert(p1, 0, 0);
    tree.insert(p2, 0, 1);
    /*
        Insert "def" on first line at offset 1:
        "a" "def" "b\n
         c"
    */
    tree.insert(p3, 0, 0);
    /*
        Insert "def" one more time on first line at offset 0:
        "def" "a" "def" "b\n
         c"
    */

    std::vector<PieceTree::Piece> pieces = Util::collectPieces(tree);

    ASSERT_EQ(pieces.size(), 4);
    ASSERT_EQ(pieces[0].length, 3);  // left of insertion
    ASSERT_EQ(pieces[1].length, 1);  // inserted + right
    ASSERT_EQ(pieces[2].length, 3);  // inserted + right
    ASSERT_EQ(pieces[3].length, 3);  // inserted + right
}