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


    void split(Node<TK> n) {
        auto splitted = n.split();

    }

    void _inorder(Node<TK> *n, void func(Node<TK> n)) {
        for (int i = 0; i < n->count; i++) {
            if (!n->leaf) _inorder(n->children[i]);
            func(n);
        }
        if (!n->leaf) _inorder(n->children[n->count]);
    }

public:
    explicit BTree(int M) : root(nullptr), M(M), n(0) {
    }

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

    void insert(TK key) {

    }

    void remove(TK key) {

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
