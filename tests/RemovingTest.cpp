#include "../src/PieceTree.h"
#include "Util.h"
#include <gtest/gtest.h>
#include <ostream>

TEST(PieceTreeRemoving, RemoveStart) {
    PieceTree tree;

    PieceTree::Piece p1{NodeType::Added, 0, 7, {3}};     // "abc\ndef"

    /*
            "abc\n
             def"
    */
    tree.insert(p1, 0, 0);

    tree.remove(0,0,3);
    /*
            "\n
             def"
    */

    std::vector<PieceTree::Piece> pieces = Util::collectPieces(tree);

    ASSERT_EQ(pieces.size(), 1);

    ASSERT_EQ(pieces[0].length, 4);
    ASSERT_EQ(pieces[0].line_breaks[0], 0);
}

TEST(PieceTreeRemoving, RemoveBeetwen) {
    PieceTree tree;

    PieceTree::Piece p1{NodeType::Added, 0, 7, {3}};     // "abc\ndef"

    /*
            "abc\n
             def"
    */
    tree.insert(p1, 0, 0);

    tree.remove(1,1,1); // e is deleted
    /*
            "abc\n
             d" "f"
    */

    std::vector<PieceTree::Piece> pieces = Util::collectPieces(tree);

    ASSERT_EQ(pieces.size(), 2);

    ASSERT_EQ(pieces[0].length, 5);
    ASSERT_EQ(pieces[0].line_breaks[0], 3);

    ASSERT_EQ(pieces[1].length, 1);
    ASSERT_EQ(pieces[1].line_breaks.size(), 0);
}

TEST(PieceTreeRemoving, RemovingEnd) {
    PieceTree tree;

    PieceTree::Piece p1{NodeType::Added, 0, 7, {3}};     // "abc\ndef"

    /*
            "abc\n
             def"
    */
    tree.insert(p1, 0, 0);

    tree.remove(1,0,3); // e is deleted
    /*
            "abc\n"
    */

    std::vector<PieceTree::Piece> pieces = Util::collectPieces(tree);

    ASSERT_EQ(pieces.size(), 1);

    ASSERT_EQ(pieces[0].length, 4);
    ASSERT_EQ(pieces[0].line_breaks[0], 3);

}

TEST(PieceTreeRemoving, RemovingWithinTwoNodes) {
    PieceTree tree;

    PieceTree::Piece p1{NodeType::Added, 0, 4, {2}}; // "ab\nc"
    PieceTree::Piece p2{NodeType::Added, 4, 3, {}};  // "def"
    tree.insert(p1, 0, 0);
    tree.insert(p2, 0, 1);
    /*
        "a" "def" "b\n
         c"
    */

    tree.remove(0,2,3);

    /*
        "a" "d" "\n
         c"
    */

    std::vector<PieceTree::Piece> pieces = Util::collectPieces(tree);

    ASSERT_EQ(pieces.size(), 3);

    ASSERT_EQ(pieces[0].length, 1);
    ASSERT_EQ(pieces[0].line_breaks.size(), 0);

    ASSERT_EQ(pieces[1].length, 1);
    ASSERT_EQ(pieces[1].line_breaks.size(), 0);

    ASSERT_EQ(pieces[2].length, 2);
    ASSERT_EQ(pieces[2].line_breaks[0], 0);
}

TEST(PieceTreeRemoving, RemovingMoreThanPossible) {
    PieceTree tree;

    PieceTree::Piece p1{NodeType::Added, 0, 4, {2}}; // "ab\nc"
    PieceTree::Piece p2{NodeType::Added, 4, 3, {}};  // "def"
    tree.insert(p1, 0, 0);
    tree.insert(p2, 0, 1);
    /*
        "a" "def" "b\n
         c"
    */

    ;
    EXPECT_THROW(tree.remove(0,1,7), PieceTreeException);
}

TEST(PieceTreeRemoving, RemovingWholeNode) {
    PieceTree tree;

    PieceTree::Piece p1{NodeType::Added, 0, 4, {2}}; // "ab\nc"
    PieceTree::Piece p2{NodeType::Added, 4, 3, {}};  // "def"
    tree.insert(p1, 0, 0);
    tree.insert(p2, 0, 1);
    /*
        "a" "def" "b\n
         c"
    */

    tree.remove(0,1,3);

    /*
        "a" "b\n
         c"
    */

    std::vector<PieceTree::Piece> pieces = Util::collectPieces(tree);

    ASSERT_EQ(pieces.size(), 2);

    ASSERT_EQ(pieces[0].length, 1);
    ASSERT_EQ(pieces[0].line_breaks.size(), 0);

    ASSERT_EQ(pieces[1].length, 3);
    ASSERT_EQ(pieces[1].line_breaks.size(), 1);
}
