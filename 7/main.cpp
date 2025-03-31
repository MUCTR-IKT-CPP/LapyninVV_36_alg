#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <memory>
#include <chrono>
#include <fstream>
#include <cmath>
#include <queue>

// AVL Tree Node
template<typename T>
struct AVLNode {
    T data;
    AVLNode* left;
    AVLNode* right;
    int height;
    AVLNode(T value) : data(value), left(nullptr), right(nullptr), height(1) {}
};

// Treap (Cartesian Tree) Node 
template<typename T>
struct TreapNode {
    T data;
    int priority;
    TreapNode* left;
    TreapNode* right;
    TreapNode(T value, int p) : data(value), priority(p), left(nullptr), right(nullptr) {}
};

// AVL Tree implementation
template<typename T>
class AVLTree {
private:
    AVLNode<T>* root;
    
    int height(AVLNode<T>* node) {
        return node ? node->height : 0;
    }
    
    int balanceFactor(AVLNode<T>* node) {
        return height(node->right) - height(node->left);
    }
    
    void updateHeight(AVLNode<T>* node) {
        node->height = std::max(height(node->left), height(node->right)) + 1;
    }
    
    AVLNode<T>* rotateRight(AVLNode<T>* y) {
        AVLNode<T>* x = y->left;
        AVLNode<T>* T2 = x->right;
        x->right = y;
        y->left = T2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }
    
    AVLNode<T>* rotateLeft(AVLNode<T>* x) {
        AVLNode<T>* y = x->right;
        AVLNode<T>* T2 = y->left;
        y->left = x;
        x->right = T2;
        updateHeight(x);
        updateHeight(y);
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
            
        updateHeight(node);
        
        int balance = balanceFactor(node);
        
        // Left Left Case
        if (balance < -1 && value < node->left->data)
            return rotateRight(node);
            
        // Right Right Case    
        if (balance > 1 && value > node->right->data)
            return rotateLeft(node);
            
        // Left Right Case
        if (balance < -1 && value > node->left->data) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        
        // Right Left Case
        if (balance > 1 && value < node->right->data) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        
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

        updateHeight(node);
        int balance = balanceFactor(node);

        // Left Left Case
        if (balance < -1 && balanceFactor(node->left) <= 0)
            return rotateRight(node);

        // Left Right Case
        if (balance < -1 && balanceFactor(node->left) > 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        // Right Right Case
        if (balance > 1 && balanceFactor(node->right) >= 0)
            return rotateLeft(node);

        // Right Left Case
        if (balance > 1 && balanceFactor(node->right) < 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    AVLNode<T>* search(AVLNode<T>* node, T value) {
        if (!node || node->data == value) return node;
        if (value < node->data)
            return search(node->left, value);
        return search(node->right, value);
    }

    AVLNode<T>* findMin(AVLNode<T>* node) {
        while (node->left) node = node->left;
        return node;
    }

    int getMaxDepth(AVLNode<T>* node) {
        if (!node) return 0;
        return 1 + std::max(getMaxDepth(node->left), getMaxDepth(node->right));
    }

    void getAllDepths(AVLNode<T>* node, int currentDepth, std::vector<int>& depths) {
        if (!node) return;
        if (!node->left && !node->right) {
            depths.push_back(currentDepth);
            return;
        }
        getAllDepths(node->left, currentDepth + 1, depths);
        getAllDepths(node->right, currentDepth + 1, depths);
    }

public:
    AVLTree() : root(nullptr) {}
    
    void insert(T value) {
        root = insert(root, value);
    }

    void remove(T value) {
        root = remove(root, value);
    }

    bool search(T value) {
        return search(root, value) != nullptr;
    }

    int getMaxDepth() {
        return getMaxDepth(root);
    }

    std::vector<int> getAllDepths() {
        std::vector<int> depths;
        getAllDepths(root, 0, depths);
        return depths;
    }
};

// Treap (Cartesian Tree) implementation
template<typename T>
class Treap {
private:
    TreapNode<T>* root;
    std::mt19937 rng;
    
    std::pair<TreapNode<T>*, TreapNode<T>*> split(TreapNode<T>* root, T value) {
        if (!root) return {nullptr, nullptr};
        
        if (root->data <= value) {
            auto [left, right] = split(root->right, value);
            root->right = left;
            return {root, right};
        } else {
            auto [left, right] = split(root->left, value);
            root->left = right;
            return {left, root};
        }
    }
    
    TreapNode<T>* merge(TreapNode<T>* left, TreapNode<T>* right) {
        if (!left || !right) return left ? left : right;
        
        if (left->priority > right->priority) {
            left->right = merge(left->right, right);
            return left;
        } else {
            right->left = merge(left, right->left);
            return right;
        }
    }
    
    TreapNode<T>* insert(TreapNode<T>* root, T value) {
        int priority = std::uniform_int_distribution<>()(rng);
        auto [left, right] = split(root, value);
        TreapNode<T>* new_node = new TreapNode<T>(value, priority);
        return merge(merge(left, new_node), right);
    }

    TreapNode<T>* remove(TreapNode<T>* root, T value) {
        if (!root) return nullptr;
        
        if (value < root->data)
            root->left = remove(root->left, value);
        else if (value > root->data)
            root->right = remove(root->right, value);
        else
            return merge(root->left, root->right);
        
        return root;
    }

    TreapNode<T>* search(TreapNode<T>* node, T value) {
        if (!node || node->data == value) return node;
        if (value < node->data)
            return search(node->left, value);
        return search(node->right, value);
    }

    int getMaxDepth(TreapNode<T>* node) {
        if (!node) return 0;
        return 1 + std::max(getMaxDepth(node->left), getMaxDepth(node->right));
    }

    void getAllDepths(TreapNode<T>* node, int currentDepth, std::vector<int>& depths) {
        if (!node) return;
        if (!node->left && !node->right) {
            depths.push_back(currentDepth);
            return;
        }
        getAllDepths(node->left, currentDepth + 1, depths);
        getAllDepths(node->right, currentDepth + 1, depths);
    }

public:
    Treap() : root(nullptr) {
        std::random_device rd;
        rng.seed(rd());
    }
    
    void insert(T value) {
        root = insert(root, value);
    }

    void remove(T value) {
        root = remove(root, value);
    }

    bool search(T value) {
        return search(root, value) != nullptr;
    }

    int getMaxDepth() {
        return getMaxDepth(root);
    }

    std::vector<int> getAllDepths() {
        std::vector<int> depths;
        getAllDepths(root, 0, depths);
        return depths;
    }
};

int main() {
    std::ofstream outFile("tree_analysis.txt");
    std::random_device rd;
    std::mt19937 gen(rd());

    const int NUM_REPEATS = 50;
    const int NUM_OPERATIONS = 1000;

    for (int i = 10; i <= 18; i++) {
        int N = 1 << i;
        outFile << "\nTesting for N = " << N << " elements\n";
        outFile << "====================================\n";

        for (int repeat = 0; repeat < NUM_REPEATS; repeat++) {
            outFile << "\nRepeat " << repeat + 1 << ":\n";

            // Generate random values
            std::vector<int> values(N);
            for (int j = 0; j < N; j++) {
                values[j] = j;
            }
            std::shuffle(values.begin(), values.end(), gen);

            AVLTree<int> avl;
            Treap<int> treap;

            // Fill both trees
            for (int value : values) {
                avl.insert(value);
                treap.insert(value);
            }

            // Measure max depths
            outFile << "Max depths after initial filling:\n";
            outFile << "AVL: " << avl.getMaxDepth() << "\n";
            outFile << "Treap: " << treap.getMaxDepth() << "\n";

            // Measure insertion time
            std::vector<int> insertValues(NUM_OPERATIONS);
            for (int& val : insertValues) {
                val = std::uniform_int_distribution<>(N, N * 2)(gen);
            }

            auto start = std::chrono::high_resolution_clock::now();
            for (int val : insertValues) {
                avl.insert(val);
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto avl_insert_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

            start = std::chrono::high_resolution_clock::now();
            for (int val : insertValues) {
                treap.insert(val);
            }
            end = std::chrono::high_resolution_clock::now();
            auto treap_insert_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

            outFile << "Insertion time (microseconds):\n";
            outFile << "AVL: " << avl_insert_time << "\n";
            outFile << "Treap: " << treap_insert_time << "\n";

            // Measure deletion time
            std::vector<int> deleteValues(NUM_OPERATIONS);
            for (int& val : deleteValues) {
                val = std::uniform_int_distribution<>(0, N - 1)(gen);
            }

            start = std::chrono::high_resolution_clock::now();
            for (int val : deleteValues) {
                avl.remove(val);
            }
            end = std::chrono::high_resolution_clock::now();
            auto avl_delete_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

            start = std::chrono::high_resolution_clock::now();
            for (int val : deleteValues) {
                treap.remove(val);
            }
            end = std::chrono::high_resolution_clock::now();
            auto treap_delete_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

            outFile << "Deletion time (microseconds):\n";
            outFile << "AVL: " << avl_delete_time << "\n";
            outFile << "Treap: " << treap_delete_time << "\n";

            // Measure search time
            std::vector<int> searchValues(NUM_OPERATIONS);
            for (int& val : searchValues) {
                val = std::uniform_int_distribution<>(0, N * 2)(gen);
            }

            start = std::chrono::high_resolution_clock::now();
            for (int val : searchValues) {
                avl.search(val);
            }
            end = std::chrono::high_resolution_clock::now();
            auto avl_search_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

            start = std::chrono::high_resolution_clock::now();
            for (int val : searchValues) {
                treap.search(val);
            }
            end = std::chrono::high_resolution_clock::now();
            auto treap_search_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

            outFile << "Search time (microseconds):\n";
            outFile << "AVL: " << avl_search_time << "\n";
            outFile << "Treap: " << treap_search_time << "\n";

            // Get all depths
            auto avl_depths = avl.getAllDepths();
            auto treap_depths = treap.getAllDepths();

            double avl_avg_depth = 0;
            double treap_avg_depth = 0;

            for (int depth : avl_depths) avl_avg_depth += depth;
            for (int depth : treap_depths) treap_avg_depth += depth;

            avl_avg_depth /= avl_depths.size();
            treap_avg_depth /= treap_depths.size();

            outFile << "Average depths:\n";
            outFile << "AVL: " << avl_avg_depth << "\n";
            outFile << "Treap: " << treap_avg_depth << "\n";
        }
    }

    outFile.close();
    return 0;
}
