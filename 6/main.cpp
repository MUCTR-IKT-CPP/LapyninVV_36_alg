#include <vector>
#include <random>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <fstream>

// Binary Search Tree Node
template<typename T>
struct BSTNode {
    T data;
    BSTNode* left;
    BSTNode* right;
    BSTNode(T value) : data(value), left(nullptr), right(nullptr) {}
};

// AVL Tree Node
template<typename T>
struct AVLNode {
    T data;
    AVLNode* left;
    AVLNode* right;
    int height;
    AVLNode(T value) : data(value), left(nullptr), right(nullptr), height(1) {}
};

// Binary Search Tree class
template<typename T>
class BST {
private:
    BSTNode<T>* root;

    BSTNode<T>* insert(BSTNode<T>* node, T value) {
        if (!node) return new BSTNode<T>(value);
        if (value < node->data) node->left = insert(node->left, value);
        else if (value > node->data) node->right = insert(node->right, value);
        return node;
    }

    BSTNode<T>* search(BSTNode<T>* node, T value) {
        if (!node || node->data == value) return node;
        if (value < node->data) return search(node->left, value);
        return search(node->right, value);
    }

    BSTNode<T>* findMin(BSTNode<T>* node) {
        while (node->left) node = node->left;
        return node;
    }

    BSTNode<T>* remove(BSTNode<T>* node, T value) {
        if (!node) return nullptr;
        
        if (value < node->data) 
            node->left = remove(node->left, value);
        else if (value > node->data) 
            node->right = remove(node->right, value);
        else {
            if (!node->left) {
                BSTNode<T>* temp = node->right;
                delete node;
                return temp;
            } else if (!node->right) {
                BSTNode<T>* temp = node->left;
                delete node;
                return temp;
            }
            BSTNode<T>* temp = findMin(node->right);
            node->data = temp->data;
            node->right = remove(node->right, temp->data);
        }
        return node;
    }

public:
    BST() : root(nullptr) {}

    void insert(T value) {
        root = insert(root, value);
    }

    bool search(T value) {
        return search(root, value) != nullptr;
    }

    void remove(T value) {
        root = remove(root, value);
    }
};

// AVL Tree class
template<typename T>
class AVLTree {
private:
    AVLNode<T>* root;

    int height(AVLNode<T>* node) {
        return node ? node->height : 0;
    }

    int getBalance(AVLNode<T>* node) {
        return node ? height(node->left) - height(node->right) : 0;
    }

    AVLNode<T>* rightRotate(AVLNode<T>* y) {
        AVLNode<T>* x = y->left;
        AVLNode<T>* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = std::max(height(y->left), height(y->right)) + 1;
        x->height = std::max(height(x->left), height(x->right)) + 1;
        return x;
    }

    AVLNode<T>* leftRotate(AVLNode<T>* x) {
        AVLNode<T>* y = x->right;
        AVLNode<T>* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = std::max(height(x->left), height(x->right)) + 1;
        y->height = std::max(height(y->left), height(y->right)) + 1;
        return y;
    }

    AVLNode<T>* insert(AVLNode<T>* node, T value) {
        if (!node) return new AVLNode<T>(value);

        if (value < node->data)
            node->left = insert(node->left, value);
        else if (value > node->data)
            node->right = insert(node->right, value);
        else
            return node;

        node->height = std::max(height(node->left), height(node->right)) + 1;
        int balance = getBalance(node);

        // Left Left
        if (balance > 1 && value < node->left->data)
            return rightRotate(node);

        // Right Right
        if (balance < -1 && value > node->right->data)
            return leftRotate(node);

        // Left Right
        if (balance > 1 && value > node->left->data) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        // Right Left
        if (balance < -1 && value < node->right->data) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    AVLNode<T>* findMin(AVLNode<T>* node) {
        while (node->left) node = node->left;
        return node;
    }

    AVLNode<T>* remove(AVLNode<T>* node, T value) {
        if (!node) return nullptr;

        if (value < node->data)
            node->left = remove(node->left, value);
        else if (value > node->data)
            node->right = remove(node->right, value);
        else {
            if (!node->left || !node->right) {
                AVLNode<T>* temp = node->left ? node->left : node->right;
                if (!temp) {
                    temp = node;
                    node = nullptr;
                } else
                    *node = *temp;
                delete temp;
            } else {
                AVLNode<T>* temp = findMin(node->right);
                node->data = temp->data;
                node->right = remove(node->right, temp->data);
            }
        }

        if (!node) return nullptr;

        node->height = std::max(height(node->left), height(node->right)) + 1;
        int balance = getBalance(node);

        // Left Left
        if (balance > 1 && getBalance(node->left) >= 0)
            return rightRotate(node);

        // Left Right
        if (balance > 1 && getBalance(node->left) < 0) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        // Right Right
        if (balance < -1 && getBalance(node->right) <= 0)
            return leftRotate(node);

        // Right Left
        if (balance < -1 && getBalance(node->right) > 0) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    AVLNode<T>* search(AVLNode<T>* node, T value) {
        if (!node || node->data == value) return node;
        if (value < node->data) return search(node->left, value);
        return search(node->right, value);
    }

public:
    AVLTree() : root(nullptr) {}

    void insert(T value) {
        root = insert(root, value);
    }

    bool search(T value) {
        return search(root, value) != nullptr;
    }

    void remove(T value) {
        root = remove(root, value);
    }
};

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::ofstream outFile("results.txt");
    if (!outFile) {
        std::cerr << "Failed to open results.txt\n";
        return 1;
    }

    outFile << "Size\tType\tBST Insert\tAVL Insert\tBST Search\tAVL Search\tArray Search\tBST Delete\tAVL Delete\n";

    // 10 test series
    for (int series = 0; series < 10; series++) {
        int size = std::pow(2, 10 + series);
        
        // 20 cycles (10 random + 10 sorted)
        for (int cycle = 0; cycle < 20; cycle++) {
            std::vector<int> data(size);
            
            // Generate data
            if (cycle < 10) {
                // Random data
                std::uniform_int_distribution<> dis(1, size * 10);
                for (int& val : data) {
                    val = dis(gen);
                }
            } else {
                // Sorted data
                for (int i = 0; i < size; i++) {
                    data[i] = i;
                }
            }

            BST<int> bst;
            AVLTree<int> avl;

            // Measure insertion time
            auto start = std::chrono::high_resolution_clock::now();
            for (int val : data) {
                bst.insert(val);
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto bst_insert_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

            start = std::chrono::high_resolution_clock::now();
            for (int val : data) {
                avl.insert(val);
            }
            end = std::chrono::high_resolution_clock::now();
            auto avl_insert_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

            // Generate 1000 random search values
            std::vector<int> search_values;
            std::uniform_int_distribution<> search_dis(1, size * 10);
            for (int i = 0; i < 1000; i++) {
                search_values.push_back(search_dis(gen));
            }

            // Measure search time
            start = std::chrono::high_resolution_clock::now();
            for (int val : search_values) {
                bst.search(val);
            }
            end = std::chrono::high_resolution_clock::now();
            auto bst_search_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0;

            start = std::chrono::high_resolution_clock::now();
            for (int val : search_values) {
                avl.search(val);
            }
            end = std::chrono::high_resolution_clock::now();
            auto avl_search_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0;

            // Array search time
            start = std::chrono::high_resolution_clock::now();
            for (int val : search_values) {
                std::find(data.begin(), data.end(), val);
            }
            end = std::chrono::high_resolution_clock::now();
            auto array_search_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0;

            // Measure deletion time
            start = std::chrono::high_resolution_clock::now();
            for (int val : search_values) {
                bst.remove(val);
            }
            end = std::chrono::high_resolution_clock::now();
            auto bst_delete_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0;

            start = std::chrono::high_resolution_clock::now();
            for (int val : search_values) {
                avl.remove(val);
            }
            end = std::chrono::high_resolution_clock::now();
            auto avl_delete_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0;

            outFile << size << "\t" 
                   << (cycle < 10 ? "Random" : "Sorted") << "\t"
                   << bst_insert_time << "\t\t"
                   << avl_insert_time << "\t\t"
                   << bst_search_time << "\t\t"
                   << avl_search_time << "\t\t"
                   << array_search_time << "\t\t"
                   << bst_delete_time << "\t\t"
                   << avl_delete_time << "\n";
        }
    }

    outFile.close();
    return 0;
}
