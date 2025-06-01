#ifndef BTree_H
#define BTree_H
#include <iostream>
#include <vector>
#include "node.h"

using namespace std;

template<typename TK>
class BTree {
    Node<TK> *root;
    int M; // grado u orden del arbol
    int n; // total de elementos en el arbol
    int total_size = 0;

    void _inorder(Node<TK> *n, void func(Node<TK> n)) {
        for (int i = 0; i < n->count; i++) {
            if (!n->leaf) _inorder(n->children[i]);
            func(n);
        }
        if (!n->leaf) _inorder(n->children[n->count]);
    }

    bool _insert_leaf(Node<TK>* node, TK key) {
        int i;
        // Move all left keys +1 space
        // to create an empty slot for key param
        // [1, 10, 12, 13, nullptr, nullptr], key = 9
        // => [1, duplicate, 10, 12, 13, nullptr]
        for (i = node->values - 1; i >= 0 && key < node->keys[i]; --i) {
            node->keys[i + 1] = node->keys[i];
        }

        // => [1, 9, 10, 12, 13, nullptr]
        node->keys[i + 1] = key;
        ++(node->values);
        return true;
    }

    bool _insert_pivot(Node<TK>* parent, TK key) {
        int i;
        // Move i to the minor key (key > keys[i])
        for (i = parent->values - 1; i >= 0 && key < parent->keys[i]; --i) {}
        // Moving to last greater key to split
        ++i;

        auto child = parent->children[i];
        if (child->values == M - 1) { // Full node? # keys == M - 1?
            // Node example to split: [10, 12, 13, 14, 15], M = 6
            // Result:
            // left -> [10, 12, nullptr, nullptr, nullptr],
            // promote -> [13, nullptr ,nullptr, nullptr, nullptr],
            // right -> [14, 15, nullptr, nullptr, nullptr]
            auto [new_parent, left_right] = child->split(M);

            // Move keys/children to insert new_parent
            for (int j = parent->values - 1; j > i; --j) {
                parent->keys[j + 1] = parent->keys[j];
                parent->children[j + 2] = parent->children[j];
            }

            // Inserting/merge new_parent
            parent->keys[i] = new_parent->keys[0];
            parent->children[i] = left_right.first;
            parent->children[i + 1] = left_right.second;
            parent->values = parent->values + 1;
            parent->count = parent->count + 1;

            // After promoting, checking where node to go (left or right)
            // Index i is already to the left
            if (key > parent->keys[i]) {
                ++i; // Right
            }
        }

        return _insert_leaf(parent->children[i], key);
    }

    auto _insert_full(Node<TK>* node, TK key) {
        auto [parent, children] = node->split(M);
        // Parent always has 1 element by split algorithm
        if (key < parent->keys[0]) { // Check parent's element to pivot
            _insert(children.first, key);
        } else {
            _insert(children.second, key);
        }
        return {parent, children};
    }

    bool _insert(Node<TK>* node, TK key) {
        if (node->values == M - 1) { // insert_full
            auto [parent, left_right] = _insert_full(node, key);
            root = parent;
            return true;
        }

        return node->leaf ? _insert_leaf(node, key) : _insert_pivot(node, key);
    }

    void remove(Node<TK>* node, TK key) {

    }

public:
    explicit BTree(int M) : root(nullptr), M(M), n(0) {}

    bool search(TK key) {
        auto n = root;
        while (n) {
            int i = 0;
            for (; i < n->count; ++i) {
                auto ckey = n->keys[i];
                if (key == ckey) {
                    return true;
                }
                if (key < ckey) {
                    break;
                }
            }

            n = n->leaf ? nullptr : n->children[i];
        }
        return false;
    }

    bool insert(TK key) {
        bool inserted = false;
        if (!root) {
            root = new Node<TK>(M);
            root->add_key(key);
            inserted = true;
        } else {
            inserted = _insert(root, key);
        }

        if (inserted) total_size++;
        return inserted;
    }

    void remove(TK key) {
        remove(root, key);
    }

    int height() {
        return root->height;
    }

    string toString(const string &sep) {
        string base;
        auto fun = [&base](Node<TK> *node) {
            if (!base.empty()) base.push_back(*" ");
            base.push_back(to_string(node->keys));
        };
        _inorder(root, fun);
        return base;
    }

    vector<TK> rangeSearch(TK begin, TK end) {
        return rangeSearch(root, begin, end, {});
    }

    vector<TK> rangeSearch(Node<TK> *n, TK &begin, TK &end, vector<TK> &results) {
        int i = 0;
        while (i < n->count) {
            if (!n->leaf) rangeSearch(n->children[i], begin, end, results);

            auto k = n->keys[i++];
            if (k > end) {
                break;
            }

            if (k >= begin) {
                results.push_back(k);
            }
        }

        if (!n->leaf) rangeSearch(n->children[i], begin, end, results);
        return results;
    }


    TK minKey() {
        return minKey(root);
    }

    TK minKey(Node<TK>* n) {
        if (!n) throw invalid_argument("No min / empty node at minKey call");
        return !n->leaf ? minKey(n->children[0]) : n->keys[0];
    }

    TK maxKey() {
        return maxKey(root);
    }

    TK maxKey(Node<TK>* n) {
        if (!n) throw invalid_argument("No max / empty node at maxKey call");
        return !n->leaf ? maxKey(n->children[n->count]) : n->keys[n->count - 1];
    }

    void clear() {
        root->killSelf(); // Cascade call of killSelf
    }

    int size() {
        return total_size;
    }

    ~BTree() {
        root->killSelf();
    }
};

#endif
