#ifndef BTree_H
#define BTree_H
#include <iostream>
#include <vector>
#include "node.h"
#include <functional>

using namespace std;

template<typename TK>
class BTree {
    Node<TK> *root;
    int M; // grado u orden del arbol
    int n; // total de elementos en el arbol
    int total_size = 0;

    void _inorder(Node<TK>* node, const std::function<void(TK)>& func) {
        if (!node) return;
        for (int i = 0; i < node->values; ++i) {
            if (!node->leaf) _inorder(node->children[i], func);
            func(node->keys[i]);
        }
        if (!node->leaf) _inorder(node->children[node->values], func);
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
            for (int j = parent->values - 1; j >= i; --j) {
                parent->keys[j + 1] = parent->keys[j];
                parent->children[j + 2] = parent->children[j + 1];
            }

            // Inserting/merge new_parent
            parent->keys[i] = new_parent->keys[0];
            parent->children[i] = left_right.first;
            parent->children[i + 1] = left_right.second;
            parent->values = parent->values + 1;

            // After promoting, checking where node to go (left or right)
            // Index i is already to the left
            if (key > parent->keys[i]) {
                ++i; // Right
            }
        }

        return _insert_leaf(parent->children[i], key);
    }

    std::pair<Node<TK>*, std::pair<Node<TK>*, Node<TK>*>> _insert_full(Node<TK>* node, TK key) {
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
    int idx = 0;
    while (idx < node->values && key > node->keys[idx]) idx++;

    if (idx < node->count && node->keys[idx] == key) {
        if (node->leaf) {
            for (int i = idx; i < node->count - 1; i++)
                node->keys[i] = node->keys[i + 1];
            node->count--;
        } else {
            Node<TK>* left = node->children[idx];
            Node<TK>* right = node->children[idx + 1];

            if (left->count >= M / 2) {
                Node<TK>* current = left;
                while (!current->leaf)
                    current = current->children[current->count];
                TK pred = current->keys[current->count - 1];
                node->keys[idx] = pred;
                remove(left, pred);
            } else if (right->count >= M / 2) {
                Node<TK>* current = right;
                while (!current->leaf)
                    current = current->children[0];
                TK succ = current->keys[0];
                node->keys[idx] = succ;
                remove(right, succ);
            } else {
                left->keys[left->count] = node->keys[idx];
                for (int i = 0; i < right->count; i++)
                    left->keys[left->count + 1 + i] = right->keys[i];
                if (!left->leaf) {
                    for (int i = 0; i <= right->count; i++)
                        left->children[left->count + 1 + i] = right->children[i];
                }
                left->count += right->count + 1;

                for (int i = idx; i < node->count - 1; i++)
                    node->keys[i] = node->keys[i + 1];
                for (int i = idx + 1; i < node->count; i++)
                    node->children[i] = node->children[i + 1];

                node->count--;
                delete right;

                remove(left, key);
            }
        }
    } else {
        if (node->leaf) return;

        bool last = (idx == node->count);
        Node<TK>* child = node->children[idx];

        if (child->count < M/2){
            Node<TK>* leftSibling = (idx > 0) ? node->children[idx - 1] : nullptr;
            Node<TK>* rightSibling = (idx < node->count) ? node->children[idx + 1] : nullptr;

            if (leftSibling && leftSibling->count >= M / 2) {
                for (int i = child->count; i > 0; i--)
                    child->keys[i] = child->keys[i - 1];
                if (!child->leaf) {
                    for (int i = child->count + 1; i > 0; i--)
                        child->children[i] = child->children[i - 1];
                    child->children[0] = leftSibling->children[leftSibling->count];
                }
                child->keys[0] = node->keys[idx - 1];
                node->keys[idx - 1] = leftSibling->keys[leftSibling->count - 1];
                child->count++;
                leftSibling->count--;
            }
            else if (rightSibling && rightSibling->count >= M / 2) {
                child->keys[child->count] = node->keys[idx];
                if (!child->leaf)
                    child->children[child->count + 1] = rightSibling->children[0];

                node->keys[idx] = rightSibling->keys[0];
                for (int i = 1; i < rightSibling->count; i++)
                    rightSibling->keys[i - 1] = rightSibling->keys[i];
                if (!rightSibling->leaf) {
                    for (int i = 1; i <= rightSibling->count; i++)
                        rightSibling->children[i - 1] = rightSibling->children[i];
                }
                child->count++;
                rightSibling->count--;
            }
            else {
                // Merge con hermano
                if (leftSibling) {
                    leftSibling->keys[leftSibling->count] = node->keys[idx - 1];
                    for (int i = 0; i < child->count; i++)
                        leftSibling->keys[leftSibling->count + 1 + i] = child->keys[i];
                    if (!child->leaf) {
                        for (int i = 0; i <= child->count; i++)
                            leftSibling->children[leftSibling->count + 1 + i] = child->children[i];
                    }
                    leftSibling->count += child->count + 1;

                    for (int i = idx - 1; i < node->count - 1; i++)
                        node->keys[i] = node->keys[i + 1];
                    for (int i = idx; i < node->count; i++)
                        node->children[i] = node->children[i + 1];
                    node->count--;

                    delete child;
                    child = leftSibling;
                    idx--;
                }
                else if (rightSibling) {
                    child->keys[child->count] = node->keys[idx];
                    for (int i = 0; i < rightSibling->count; i++)
                        child->keys[child->count + 1 + i] = rightSibling->keys[i];
                    if (!rightSibling->leaf) {
                        for (int i = 0; i <= rightSibling->count; i++)
                            child->children[child->count + 1 + i] = rightSibling->children[i];
                    }
                    child->count += rightSibling->count + 1;

                    for (int i = idx; i < node->count - 1; i++)
                        node->keys[i] = node->keys[i + 1];
                    for (int i = idx + 1; i < node->count; i++)
                        node->children[i] = node->children[i + 1];
                    node->count--;

                    delete rightSibling;
                }
            }
        }

        if (last && idx > node->count)
            remove(node->children[idx - 1], key);
        else
            remove(node->children[idx], key);
    }
}


public:
    explicit BTree(int M) : root(nullptr), M(M), n(0) {}

    bool search(TK key) {
        auto n = root;
        while (n) {
            int i = 0;
            for (; i < n->values; ++i) {
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
        if (!root) return;
        remove(root, key);
    }

    int height() {
        return root->height;
    }

    string toString(const string &sep) {
        string base;
        auto fun = [&base, &sep](TK val) {
            if (!base.empty()) base += sep;
            base += to_string(val);
        };
        _inorder(root, fun);
        cout << base << endl;
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
