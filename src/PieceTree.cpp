#include "PieceTree.h"

#include <optional>
#include <utility>
#include <vector>

PieceTree::Piece PieceTree::Piece::splitAt(int split_offset) {
    PieceTree::Piece r;
    r.type = type;

    if (split_offset <= 0) {
        // Right gets everything, left becomes empty
        r.offset = offset;
        r.length = length;
        r.line_breaks = line_breaks;
        length = 0;
        line_breaks.clear();
        return r;
    }

    if (split_offset >= length) {
        // Nothing to move to the right
        r.offset = offset + length;
        r.length = 0;
        return r;
    }

    // Normal case
    r.offset = offset + split_offset;
    r.length = length - split_offset;
    length = split_offset;

    int resizeIndex = -1;
    for (int i = 0; i < line_breaks.size(); i++) {
        if (line_breaks[i] >= split_offset) {
            if (resizeIndex == -1)
                resizeIndex = i;
            r.line_breaks.push_back(line_breaks[i] - split_offset);
        }
    }
    if (resizeIndex != -1)
        line_breaks.resize(resizeIndex);

    return r;
}

int PieceTree::Piece::getLine(int piece_offset) {
    if (line_breaks.empty())
        return 0;

    int line = 0;
    for (int i = 0; i < line_breaks.size(); i++) {
        if (line_breaks[i] >= piece_offset) {
            return i;
        }
    }
    return line_breaks.size();
}

void PieceTree::Piece::cutRightSide(int cut_offset) {
    // including cut_offset
    length = cut_offset != 0 ? cut_offset : 1;
    for (int i = 0; i < line_breaks.size(); i++) {
        if (line_breaks[i] > length) {
            int new_size = i;
            line_breaks.resize(new_size);
            break;
        }
    }
}

void PieceTree::Piece::cutLeftSide(int cut_offset) {
    // including offset
    offset += cut_offset;
    length -= cut_offset;

    line_breaks.erase(std::remove_if(line_breaks.begin(), line_breaks.end(), [&](int br) { return br < cut_offset; }),
                      line_breaks.end());

    for (int &br : line_breaks) {
        br -= cut_offset;
    }
}

PieceTree::Node::Node(const Piece &p_) : piece(p_) {}
PieceTree::Node::Node(Piece &&p_) : piece(std::move(p_)) {}

int PieceTree::Node::recalcMetadata() {
    int lh = left ? left->height : 0;
    int rh = right ? right->height : 0;
    height = 1 + std::max(lh, rh);

    left_line_count = left ? left->left_line_count + left->piece.line_breaks.size() : 0;

    return height;
}

int PieceTree::Node::bf() {
    int lh = left ? left->height : 0;
    int rh = right ? right->height : 0;
    return lh - rh;
}

PieceTree::Node *PieceTree::Node::rightRotation() {
    Node *current = this;
    Node *new_node = left;

    current->left = new_node->right;
    if (current->left) {
        current->left->parent = current;
    }

    if (current->parent) {
        if (current->parent->right == current) {
            current->parent->right = new_node;
        } else {
            current->parent->left = new_node;
        }
    }
    new_node->parent = current->parent;

    new_node->right = current;
    current->parent = new_node;

    current->recalcMetadata();
    new_node->recalcMetadata();
    return new_node;
}

PieceTree::Node *PieceTree::Node::leftRotation() {
    Node *current = this;
    Node *new_node = right;

    current->right = new_node->left;
    if (current->right) {
        current->right->parent = current;
    }

    if (current->parent) {
        if (current->parent->right == current) {
            current->parent->right = new_node;
        } else {
            current->parent->left = new_node;
        }
    }
    new_node->parent = current->parent;

    new_node->left = current;
    current->parent = new_node;

    current->recalcMetadata();
    new_node->recalcMetadata();
    return new_node;
}

PieceTree::Node PieceTree::Node::splitAt(int split_offset) {
    Node r(piece.splitAt(split_offset));
    r.height = height;
    r.parent = parent;
    r.right = right;
    right = nullptr;

    this->recalcMetadata();
    r.recalcMetadata();

    return r;
}

PieceTree::Node *PieceTree::Node::next() {
    if (right != nullptr) {
        return right->leftest();
    }

    Node *cur = this;
    while (cur->parent != nullptr) {
        if (cur->parent->left == cur) {
            return cur->parent;
        }

        cur = cur->parent;
    }

    return nullptr;
}

PieceTree::Node *PieceTree::Node::prev() {
    if (left != nullptr) {
        return left->rightest();
    }

    Node *cur = this;
    while (cur->parent != nullptr) {
        if (cur->parent->right == cur) {
            return cur->parent;
        }
        cur = cur->parent;
    }

    return nullptr;
}

PieceTree::Node *PieceTree::Node::leftest() {
    Node *node = this;
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}
PieceTree::Node *PieceTree::Node::rightest() {
    Node *node = this;
    while (node->right != nullptr) {
        node = node->right;
    }
    return node;
}

PieceTree::Node *PieceTree::Node::remove(PieceTree::Node *&root) {
    PieceTree::Node *head = nullptr;
    if (right) {
        head = right;
        if (parent && parent->left == this) {
            parent->left = head;
            head->parent = parent;
        } else if (parent && parent->right == this) {
            parent->right = head;
            head->parent = parent;
        } else {
            head->parent = nullptr;
        }
        if (left) {
            auto *head_leftest = head->leftest();
            head_leftest->left = left;
            left->parent = head_leftest;
            root = head_leftest->balanceAndUpdate();
        }
    } else if (left) {
        head = left;
        if (parent && parent->left == this) {
            parent->left = head;
            head->parent = parent;
            root = head->parent->balanceAndUpdate();
        } else if (parent && parent->right == this) {
            parent->right = head;
            head->parent = parent;
            root = head->parent->balanceAndUpdate();
        } else {
            head->parent = nullptr;
        }
    } else {
        if (parent && parent->left == this) {
            parent->left = nullptr;
            root = parent->balanceAndUpdate();
        } else if (parent && parent->right == this) {
            parent->right = nullptr;
            root = parent->balanceAndUpdate();
        }
    }

    delete this;

    return head;
}

std::optional<PieceTree::Position> PieceTree::findVisualLine(int line, Node *node) {
    if (!node) {
        return std::nullopt;
    }

    int line_sum_subtree = node->piece.line_breaks.size() + node->left_line_count;
    while (true) {
        if (node->left_line_count <= line && line <= line_sum_subtree) {
            int line_in_node = line - node->left_line_count;
            if (line_in_node == 0) {
                // searching the start of the line
                do {
                    Node *temp = node->prev();
                    if (!temp)
                        break;
                    node = temp;
                    line_in_node = node->piece.line_breaks.size();
                } while (node->piece.line_breaks.empty());
            }
            int piece_offset = line_in_node != 0 ? node->piece.line_breaks[line_in_node - 1] + 1 : 0;
            // edge case, when node has '\n' as the last character, so the beginning of the line is in the next node;
            if (piece_offset >= node->piece.length) {
                node = node->next();
                piece_offset = 0;
            }
            return Position{node, piece_offset};
        }
        if (line_sum_subtree < line) {
            line -= line_sum_subtree;
            node = node->right;
            if (!node)
                break;
            line_sum_subtree = node->piece.line_breaks.size() + node->left_line_count;
        } else {
            node = node->left;
            if (!node)
                break;
            line_sum_subtree = node->piece.line_breaks.size() + node->left_line_count;
        }
    }

    return std::nullopt;
}

PieceTree::Position PieceTree::findVisualColumn(Node *node, int offset_line_begin, int visual_column) {
    int piece_line = node->piece.getLine(offset_line_begin);
    int piece_offset = offset_line_begin + visual_column;

    if (piece_line < node->piece.line_breaks.size() && piece_offset >= node->piece.line_breaks[piece_line]) {
        throw PieceTreeException("insertion column " + std::to_string(visual_column) + " is out of line bounds");
    }

    while (piece_offset > node->piece.length) {
        piece_offset -= node->piece.length;

        node = node->next();
        if (!node) {
            throw PieceTreeException("insertion column " + std::to_string(visual_column) + " beyond document length");
        }

        if (!node->piece.line_breaks.empty() && piece_offset >= node->piece.line_breaks[0]) {
            throw PieceTreeException("insertion column " + std::to_string(visual_column) + " is out of line bounds");
        }
    }

    return {node, piece_offset};
}

void PieceTree::insertNodeAtPosition(const Position &insert, Node *new_node) {
    Node *insert_node = insert.node;
    Node *insert_node_parent = insert_node->parent;

    if (insert.piece_offset == 0) {
        // Insert before the target node
        if (insert_node_parent) {
            if (insert_node_parent->left == insert_node)
                insert_node_parent->left = new_node;
            else
                insert_node_parent->right = new_node;
        } else {
            root = new_node;
        }

        new_node->parent = insert_node_parent;
        new_node->right = insert_node;
        new_node->left = insert_node->left;
        if (new_node->left)
            new_node->left->parent = new_node;

        insert_node->left = nullptr;
        insert_node->parent = new_node;
    } else if (insert.piece_offset >= insert_node->piece.length) {
        // Insert after the target
        if (insert_node_parent) {
            if (insert_node_parent->left == insert_node)
                insert_node_parent->left = new_node;
            else
                insert_node_parent->right = new_node;
        } else {
            root = new_node;
        }

        new_node->parent = insert_node_parent;
        new_node->left = insert_node;
        new_node->right = insert_node->right;
        if (new_node->right)
            new_node->right->parent = new_node;

        insert_node->right = nullptr;
        insert_node->parent = new_node;
    } else {
        // split target
        Piece right_piece = insert_node->piece.splitAt(insert.piece_offset);
        Node *right_node = new Node(right_piece);

        if (insert_node_parent) {
            if (insert_node_parent->left == insert_node)
                insert_node_parent->left = new_node;
            else
                insert_node_parent->right = new_node;
        } else {
            root = new_node;
        }

        new_node->parent = insert_node_parent;
        new_node->left = insert_node;
        new_node->right = right_node;
        insert_node->parent = new_node;
        right_node->parent = new_node;
    }
    new_node->recalcMetadata();
    if (new_node->parent)
        new_node->parent->recalcMetadata();
}

PieceTree::Node *PieceTree::Node::balanceAndUpdate() {
    auto node = this;
    node->recalcMetadata();

    if (node->bf() < -1) {
        if (node->right->bf() == 1) {
            node = node->rightRotation();
        }
        node = node->leftRotation();
    } else if (node->bf() > 1) {
        if (node->left->bf() == -1) {
            node->left = node->leftRotation();
        }
        node = node->rightRotation();
    }

    node->recalcMetadata();

    if (node->parent != nullptr) {
        return node->parent->balanceAndUpdate();
    }

    // new root
    return node;
};

std::vector<PieceTree::Piece> PieceTree::getLinePiecesFromPosition(Position &start) {
    std::vector<Piece> pieces;
    // clear the beginning of the piece
    // find the end and clear it also
    pieces.push_back(start.node->piece);
    if (start.piece_offset > 0) {
        // cut the lines before
        pieces.back().cutLeftSide(start.piece_offset);
    }

    bool end_line_in_piece = false;
    for (int i : pieces.back().line_breaks) {
        if (i > start.piece_offset) {
            if (i + 1 < pieces.back().length) {
                pieces.back().cutRightSide(i + 1);
            }
            end_line_in_piece = true;
        }
    }

    while (!end_line_in_piece) {
        auto *temp = start.node->next();
        if (!temp) {
            // the last line in the tree
            end_line_in_piece = true;
            break;
        }
        start.node = temp;
        pieces.push_back(start.node->piece);
        if (!start.node->piece.line_breaks.empty()) {
            int i = start.node->piece.line_breaks[0];
            if (i + 1 < pieces.back().length) {
                pieces.back().cutRightSide(i + 1);
            }
            end_line_in_piece = true;
        }
    }

    return pieces;
}

void PieceTree::removeStartingFromPosition(const Position &start, int cut_length) {
    if (!start.node) {
        throw PieceTreeException("The node of the starting removal position is nullptr");
    }
    Node *r_node = start.node;
    int r_piece_offset = start.piece_offset;

    while (cut_length > 0) {
        if (r_piece_offset > 0) {
            if (r_piece_offset + cut_length < r_node->piece.length) {
                // inside the node, need to split
                Piece right_piece = r_node->piece.splitAt(r_piece_offset + cut_length);
                Node *right_node = new Node(right_piece);

                if (r_node->right) {
                    right_node->right = r_node->right;
                    r_node->right->parent = right_node;
                }
                r_node->right = right_node;
                right_node->parent = r_node;
                right_node->balanceAndUpdate();

                // cutting removal_node
                r_node->piece.length = r_piece_offset;
                cut_length = 0;
            } else if (r_piece_offset + cut_length >= r_node->piece.length) {
                // cut the whole right side
                int old_piece_length = r_node->piece.length;
                r_node->piece.cutRightSide(r_piece_offset);
                cut_length -= old_piece_length - r_node->piece.length;

                if (cut_length > 0) {
                    auto *temp = r_node->next();
                    if (!temp) {
                        throw PieceTreeException(
                            "The length of the cutout is greater than the length of the WHOLE text");
                    }
                    r_node = temp;
                    r_piece_offset = 0;
                }
            }
        } else if (r_piece_offset == 0) {
            if (r_piece_offset + cut_length < r_node->piece.length) {
                // cut left side
                int old_piece_length = r_node->piece.length;
                r_node->piece.cutLeftSide(r_piece_offset + cut_length);
                cut_length -= old_piece_length - r_node->piece.length;
            } else if (r_piece_offset + cut_length >= r_node->piece.length) {
                // remove the hole node
                cut_length -= r_node->piece.length;
                if (cut_length > 0) {
                    auto *temp = r_node->next();
                    if (!temp) {
                        throw PieceTreeException(
                            "The length of the cutout is greater than the length of the WHOLE text");
                    }
                    r_node = temp;
                    r_piece_offset = 0;
                } else {
                    r_node->remove(root);
                }
            }
        }
    }
}

// insertionLine and insertionColumn are 0-based
// insertionColumn including goes to the right node
void PieceTree::insert(const Piece &new_piece, int insertion_line, int insertion_column) {
    Node *new_node = new Node(new_piece);

    if (root == nullptr) {
        root = new_node;
        return;
    }

    std::optional<Position> result = findVisualLine(insertion_line, root);
    if (!result) {
        throw PieceTreeException("Insertion: line " + std::to_string(insertion_line) + " not found in piece table");
    }
    Position insert = *result;
    insert = findVisualColumn(insert.node, insert.piece_offset, insertion_column);

    insertNodeAtPosition(insert, new_node);

    // only the path from the inserted node to the root need recalculating and rebalancing
    root = new_node->balanceAndUpdate();
}

// line and column are 0-based
// column is not automatically included, so the min length is 1;
void PieceTree::remove(int line, int column, int length) {
    if (length < 1)
        throw PieceTreeException("Cut length must be greater than 0");
    std::optional<Position> result = findVisualLine(line, root);
    if (!result) {
        throw PieceTreeException("Removing: line " + std::to_string(line) + " not found in piece table");
    }
    Position start_pos = *result;
    start_pos = findVisualColumn(start_pos.node, start_pos.piece_offset, column);

    removeStartingFromPosition(start_pos, length);
}

std::vector<PieceTree::Piece> PieceTree::getLinePieces(int line) {
    std::optional<Position> result = findVisualLine(line, root);
    if (!result) {
        throw PieceTreeException("Getting: line " + std::to_string(line) + " not found in piece table");
    }
    Position line_pos = *result;

    return getLinePiecesFromPosition(line_pos);
}