#ifndef NODE_H
#define NODE_H
#include <cmath>

using namespace std;

template<typename TK>
struct Node {
    int M;
    // array de keys
    TK *keys;
    // array de punteros a hijos
    Node **children;
    // cantidad de keys
    int values, count, height;
    // indicador de nodo hoja
    bool leaf;

    Node() : keys(nullptr), children(nullptr), values(0), count(0), height(0), leaf(false) {}

    explicit Node(int M) {
        keys = new TK[M - 1];
        children = new Node*[M];
        for (int i = 0; i < M; ++i) children[i] = nullptr;
        count = 0;
        values = 0;
        height = 0;
        leaf = true;
        this->M = M;
    }

    template<int M>
    explicit Node(Node* (&arr)[M]) : Node(M) { // Delegates Node(int) constructor
        for (auto &t : arr)  add_node(t);
    }

    void init(int M) {
        keys = new TK[M - 1];
        children = new Node*[M];
        count = 0;
        height = 0;
        values = 0;
        leaf = true;
    }

    void updateHeight(int h) {
        height = h;
    }

    void killSelf() {
        for (int i = 0; i < count; i++) {
            children[i]->killSelf();
        }

        delete[] keys;
        delete[] children;
    }

    void add_key(TK key) {
        if (values >= M - 1) return;
        keys[values++] = key;
    }

    void add_node(Node *node) {
        if (!node || count >= M) return;
        children[count++] = node;
    }

    pair<Node*, pair<Node*, Node*>> split(int M) {
        // FIXME: implementation of Node#split
        int mid = values / 2;
        Node *left = new Node(M), *right = new Node(M), *parent = new Node(M);

        for (int i = 0; i < values; ++i) {
            if (i < mid) left->add_key(keys[i]);
            else if (i > mid) right->add_key(keys[i]);
        }

        if (!leaf) {
            for (int i = 0; i <= values; ++i) {
                if (i <= mid) left->add_node(children[i]);
                else if (i > mid) right->add_node(children[i]);
            }

            left->leaf = false;
            right->leaf = false;
        }

        parent->add_key(keys[mid]);
        parent->leaf = false;
        parent->add_node(left);
        parent->add_node(right);

        return {parent, {left, right}};
    }


};

#endif
