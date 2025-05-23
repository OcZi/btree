#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

const int M = 5;

template<typename TK>
struct Node {
    TK *keys;
    Node **children;
    int count; //contador de llaves
    bool leaf;

    Node(initializer_list<TK> elements) {
        this->leaf = true;
        this->keys = new TK[M - 1];
        this->children = new Node<TK> *[M];
        this->count = 0;
        for (auto it = elements.begin(); it != elements.end(); ++it) {
            this->keys[this->count] = *it;
            this->children[this->count] = nullptr;
            this->count++;
        }
        this->children[this->count] = nullptr;
    }

    void link_child(int index_child, initializer_list<TK> elements) {
        this->leaf = false;
        this->children[index_child] = new Node(elements);
    }

    void killSelf() {
        for (int i = 0; i < count; ++i) {
            if (!leaf) delete children[i];
        }

        if (!leaf) delete children[count];
        delete [] children;
        delete [] keys;
    }

    ~Node() {
        // TODO: liberar memoria de keys y children
        killSelf();
    }
};

template<typename T>
class BTree {
private:
    Node<T> *root;

public:
    explicit BTree(Node<T> *root = nullptr) {
        this->root = root;
    }

    ~BTree() {
        root->killSelf();
    }

    // Mostrar el árbol en orden
    string toString() {
        string result = "";
        toString(root, result);
        return result;
    }

    /**
     * Search key parameter in the BTree.
     * It has the strategy of an AVL;
     *  goes down by conditions (left min, right max). But in this case, there is 'n' children.
     * Still having a good worst case.
     * @param key Key to find
     * @return If key is present
     */
    bool search(T key) {
        auto n = root;
        while (n) {
            // Mientras se trabaje sobre un nodo, ejecutar bucle
            int i = 0; // Node index
            for (; i < n->count; ++i) {
                // Iterar sobre el rango de nodes actuales
                auto ckey = n->keys[i]; // Node's key
                if (key == ckey) {
                    // Si son iguales, key está en el BTree
                    return true;
                }
                if (key < ckey) {
                    //  Si el key es menor que los keys, no habrán nodos importantes por revisar
                    break;
                }
            }

            n = n->leaf ? nullptr : n->children[i]; // Si es hoja, no tendrá hijos. Si es padre, continuar bucle
        }
        return false;
    }

    /**
     * Search a range of keys in the BTree.
     * Using similar strategy of normal search, with the difference
     * of
     * @param begin Start range
     * @param end End range
     * @return All element matches in start - end range
     */
    vector<int> range_search(T begin, T end) {
        vector<int> v;
        return range_search(root, begin, end, v);
    }

    vector<int> range_search(Node<T> *n, T &begin, T &end, vector<int> &results) {
        int i = 0; // Node index
        while (i < n->count) {
            // Verificar solo keys disponibles
            if (!n->leaf) range_search(n->children[i], begin, end, results); // Si no es hoja, bajar y buscar

            auto k = n->keys[i++]; // Obtener siguiente key
            if (k > end) {
                break;
            }

            if (k >= begin) {
                results.push_back(k); // Agregar key dentro del rango
            }
        }

        // Bajar y buscar sobre el último nodo, pueden estar dentro del rango
        if (!n->leaf) range_search(n->children[i], begin, end, results);
        return results;
    }

    // Verifique las propiedades de un árbol B
    bool check_properties(Node<T> *node, int level, int &leaf_level, bool isRoot) {
        cout << "doing start " << endl;
        for (int i = 0; i < node->count - 1; ++i) {
            if (node->keys[i] > node->keys[i + 1])
                return false;
        }


        if (!node->leaf) {
            for (int i = 0; i <= node->count; ++i) {
                if (!check_properties(node->children[i], level + 1, leaf_level, false))
                    return false;
            }
        }


        if (node->leaf) {
            if (leaf_level == -1) {
                leaf_level = level;
            } else if (leaf_level != level) {
                return false;
            }
        }

        return !isRoot && (M / 2 - 1) > node->count;
    }


    bool check_properties() {
        if (!root) return true;
        int leaf_level = -1;
        cout << "leaf level " + leaf_level << endl;
        return check_properties(root, 0, leaf_level, true);
    }


    // Construya un árbol B a partir de un vector de elementos ordenados
    static BTree *build_from_ordered_vector(vector<T> elements) {
        vector<Node*> nodes;
        int total_sections = elements.size() / (M - 1);
        int z = 0;
        for (int i = 0; i < total_sections; i++) {
            for (int j = 0; j < M; j++) {
                auto list = initializer_list<T>(elements.begin(), elements.end());
                nodes.emplace_back(list);
            }
        }

        return {};
    }

private:
    void toString(Node<T> *nodo, string &result) {
        int i = 0;
        for (; i < nodo->count; i++) {
            if (!nodo->leaf) toString(nodo->children[i], result);
            result += std::to_string(nodo->keys[i]) + ",";
        }
        if (!nodo->leaf) toString(nodo->children[i], result);
    }
};

void test1() {
    Node<int> *root = new Node<int>({11, 16, 20, 31});
    root->link_child(0, {3, 5});
    root->link_child(1, {12, 15});
    root->link_child(2, {17, 19});
    root->link_child(3, {22, 25, 27, 28});
    root->link_child(4, {32, 36, 37, 38});
    BTree<int> *btree = new BTree<int>(root);

    cout << "25:" << btree->search(25) << endl; //25:1
    cout << "16:" << btree->search(16) << endl; //16:1
    cout << "35:" << btree->search(35) << endl; //35:0

    cout << btree->toString() << endl; //3,5,11,12,15,16,17,19,20,22,25,27,28,31,32,36,37,38

    vector<int> result = btree->range_search(14, 20);
    for (auto elem: result) cout << elem << ", "; //15,16,17,19,20
    cout << endl;
    delete btree;
}

void test2() {
    auto *root = new Node<int>({20});
    root->link_child(0, {11, 16});
    root->link_child(1, {31, 35});
    root->children[0]->link_child(0, {3, 5});
    root->children[0]->link_child(1, {12, 15});
    root->children[0]->link_child(2, {17, 19});
    root->children[1]->link_child(0, {22, 25, 27, 28});
    root->children[1]->link_child(1, {32, 33});
    root->children[1]->link_child(2, {36, 38});
    auto *btree = new BTree<int>(root);

    cout << "25:" << btree->search(25) << endl; //25:1
    cout << "16:" << btree->search(16) << endl; //16:1
    cout << "37:" << btree->search(37) << endl; //37:0

    cout << btree->toString() << endl; //3,5,11,12,15,16,17,19,20,22,25,27,28,31,32,33,35,36,38
    vector<int> result = btree->range_search(14, 21);
    for (auto elem: result) cout << elem << ", "; //15,16,17,19,20
    cout << endl;
    delete btree;
}

void test3() {
    std::vector<int> elements = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    BTree<int> *btree = BTree<int>::build_from_ordered_vector(elements);

    if (btree->check_properties()) {
        cout << "El árbol cumple con las propiedades de un árbol B." << endl;
        cout << btree->toString() << endl; //1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
    } else {
        cout << "El árbol no cumple con las propiedades de un árbol B." << endl;
    }
    delete btree;
}

int main() {
    cout << "Test 1" << endl;
    test1();
    cout << "Test 2" << endl;
    test2();
    cout << "Test 3" << endl;
    test3();
    return 0;
}
