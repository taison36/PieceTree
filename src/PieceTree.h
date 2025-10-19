#ifndef PieceTree_H
#define PieceTree_H

#pragma once

#include <stack>
#include <exception>
#include <optional>
#include <string>
#include <utility>
#include <vector>

enum class NodeType { Original, Added };

class PieceTreeException : public std::exception {
    std::string message;

  public:
    PieceTreeException(const std::string &msg) : message(msg) {}

    PieceTreeException(const char *msg) : message(msg) {}

    const char *what() const noexcept { return message.c_str(); }
};

class PieceTree {
    public:
    struct Piece {
        NodeType type;
        int offset; // 0-based
        int length;
        std::vector<int> line_breaks; // 0-based

        Piece splitAt(int split_offset);
        void cutRightSide(int cut_offset);
        void cutLeftSide(int cut_offset);
        int getLine(int piece_offset);
    };
    private:
    class Node {
      public:
        Piece piece;
        int left_line_count = 0;
        int height = 0;
        Node *parent = nullptr;
        Node *left = nullptr;
        Node *right = nullptr;

        explicit Node(const Piece &p_);
        explicit Node(Piece &&p_);

        int recalcMetadata();
        int bf();
        Node *rightRotation();
        Node *leftRotation();
        Node splitAt(int split_offset);
        Node *next();
        Node *prev();
        Node *leftest();
        Node *rightest();
        Node *remove(Node *&root);
        Node *balanceAndUpdate();
    };
    class Iterator {
      public:
        explicit Iterator(Node *root) { pushLeft(root); }

        Piece &operator*() { return stack.top()->piece; }
        Piece *operator->() { return &stack.top()->piece; }

        Iterator &operator++() {
            Node *n = stack.top();
            stack.pop();
            if (n && n->right)
                pushLeft(n->right);
            return *this;
        }

        bool operator==(const Iterator &other) const { return stack == other.stack; }
        bool operator!=(const Iterator &other) const { return !(*this == other); }

      private:
        std::stack<Node *> stack;

        void pushLeft(Node *n) {
            while (n) {
                stack.push(n);
                n = n->left;
            }
        }
    };

    struct Position {
      public:
        Node *node;
        int piece_offset;

        Position(Node *n, int o) : node(n), piece_offset(o) {}
    };

    Node *root = nullptr;

    std::optional<Position> findVisualLine(int line, Node *node);
    Position findVisualColumn(Node *node, int offset_line_begin, int visual_column);
    std::pair<Node *, Node *> splitAt(Position &pos);
    void insertNodeAtPosition(const Position &insert, Node *new_node);
    void removeStartingFromPosition(const Position &start, int length);
    std::vector<Piece> getLinePiecesFromPosition(Position &start);

  public:
    void insert(const Piece &new_piece, int insertion_line, int insertion_column);
    void remove(int line, int column, int length);
    std::vector<Piece> getLinePieces(int line);
    Iterator begin() { return Iterator(root); }
    Iterator end() { return Iterator(nullptr); }
};

#endif  // PieceTree_H
