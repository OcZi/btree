#ifndef NODE_H
#define NODE_H
#include <cmath>

using namespace std;

template<typename TK>
struct Node {
    // array de keys
    TK *keys;
    // array de punteros a hijos
    Node **children;
    // cantidad de keys
    int count, height;
    // indicador de nodo hoja
    bool leaf;

    Node() : keys(nullptr), children(nullptr), count(0), height(0), leaf(false) {
    }

    explicit Node(int M) {
        keys = new TK[M - 1];
        children = new Node*[M];
        count = 0;
        height = 0;
        leaf = true;
    }

    template<int M>
    explicit Node(Node* (&arr)[M]) : Node(M) {
        for (auto &t : arr)  add_node(t);
    }

    void init(int M) {
        keys = new TK[M - 1];
        children = new Node*[M];
        count = 0;
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

    void add_node(Node *node) {
        if (!node) return;

        children[count++] = node;
    }

    pair<Node*, Node*> split() {
        // FIXME: implementation of Node#split
        auto mid = static_cast<double>(count) / 2;
        Node *left = new Node[ceil(mid)], *right = new Node[floor(mid)];
        for (int i = 0; i < count; ++i) {
            if (i < mid) {
                left[i] = children[i];
            } else {
                right[i] ;
            }

        }
        return {left, right};
    }


};

#endif
