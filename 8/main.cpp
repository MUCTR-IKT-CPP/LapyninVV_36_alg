#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <fstream>
#include <queue>
#include <cmath>

// Binary Max Heap implementation
template<typename T>
class BinaryHeap {
private:
    std::vector<T> heap;
    
    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[parent] < heap[index]) {
                std::swap(heap[parent], heap[index]);
                index = parent;
            } else {
                break;
            }
        }
    }
    
    void heapifyDown(int index) {
        int size = heap.size();
        while (true) {
            int largest = index;
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            
            if (left < size && heap[left] > heap[largest]) {
                largest = left;
            }
            if (right < size && heap[right] > heap[largest]) {
                largest = right;
            }
            
            if (largest != index) {
                std::swap(heap[index], heap[largest]);
                index = largest;
            } else {
                break;
            }
        }
    }
    
public:
    void insert(T value) {
        heap.push_back(value);
        heapifyUp(heap.size() - 1);
    }
    
    T getMax() {
        if (heap.empty()) throw std::runtime_error("Heap is empty");
        return heap[0];
    }
    
    T extractMax() {
        if (heap.empty()) throw std::runtime_error("Heap is empty");
        T max = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) {
            heapifyDown(0);
        }
        return max;
    }
    
    bool empty() {
        return heap.empty();
    }
    
    void clear() {
        heap.clear();
    }
};

// Binomial Tree Node
template<typename T>
struct BinomialNode {
    T key;
    int degree;
    BinomialNode* child;
    BinomialNode* sibling;
    
    BinomialNode(T k) : key(k), degree(0), child(nullptr), sibling(nullptr) {}
};

// Binomial Heap implementation
template<typename T>
class BinomialHeap {
private:
    BinomialNode<T>* head;
    
    BinomialNode<T>* merge(BinomialNode<T>* b1, BinomialNode<T>* b2) {
        if (b1->key < b2->key) {
            std::swap(b1, b2);
        }
        b2->sibling = b1->child;
        b1->child = b2;
        b1->degree++;
        return b1;
    }
    
    void consolidate() {
        if (!head) return;
        
        std::vector<BinomialNode<T>*> degree_table(32, nullptr);
        BinomialNode<T>* current = head;
        BinomialNode<T>* next = nullptr;
        
        while (current) {
            next = current->sibling;
            current->sibling = nullptr;
            
            while (degree_table[current->degree]) {
                BinomialNode<T>* same_degree = degree_table[current->degree];
                degree_table[current->degree] = nullptr;
                current = merge(current, same_degree);
            }
            
            degree_table[current->degree] = current;
            current = next;
        }
        
        head = nullptr;
        for (auto node : degree_table) {
            if (node) {
                node->sibling = head;
                head = node;
            }
        }
    }
    
public:
    BinomialHeap() : head(nullptr) {}
    
    void insert(T key) {
        BinomialNode<T>* node = new BinomialNode<T>(key);
        if (!head) {
            head = node;
        } else {
            node->sibling = head;
            head = node;
            consolidate();
        }
    }
    
    T getMax() {
        if (!head) throw std::runtime_error("Heap is empty");
        
        BinomialNode<T>* max_node = head;
        BinomialNode<T>* current = head->sibling;
        
        while (current) {
            if (current->key > max_node->key) {
                max_node = current;
            }
            current = current->sibling;
        }
        
        return max_node->key;
    }
    
    T extractMax() {
        if (!head) throw std::runtime_error("Heap is empty");
        
        BinomialNode<T>* max_node = head;
        BinomialNode<T>* max_prev = nullptr;
        BinomialNode<T>* current = head->sibling;
        BinomialNode<T>* prev = head;
        
        while (current) {
            if (current->key > max_node->key) {
                max_node = current;
                max_prev = prev;
            }
            prev = current;
            current = current->sibling;
        }
        
        T max_value = max_node->key;
        
        if (max_prev) {
            max_prev->sibling = max_node->sibling;
        } else {
            head = max_node->sibling;
        }
        
        BinomialNode<T>* child = max_node->child;
        delete max_node;
        
        BinomialHeap<T> temp;
        while (child) {
            BinomialNode<T>* next = child->sibling;
            child->sibling = temp.head;
            temp.head = child;
            child = next;
        }
        
        if (temp.head) {
            BinomialNode<T>* temp_head = temp.head;
            while (temp_head->sibling) {
                temp_head = temp_head->sibling;
            }
            temp_head->sibling = head;
            head = temp.head;
            consolidate();
        }
        
        return max_value;
    }
    
    bool empty() {
        return head == nullptr;
    }
};

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::ofstream outFile("heap_analysis.txt");
    
    const int NUM_OPERATIONS = 1000;
    
    for (int i = 3; i <= 7; i++) {
        int N = static_cast<int>(std::pow(10, i));
        outFile << "\nTesting for N = " << N << " elements\n";
        outFile << "====================================\n\n";
        
        std::vector<int> values(N);
        for (int j = 0; j < N; j++) {
            values[j] = j;
        }
        std::shuffle(values.begin(), values.end(), gen);
        
        // Test Binary Heap
        {
            BinaryHeap<int> binary_heap;
            
            // Fill heap
            auto start = std::chrono::high_resolution_clock::now();
            for (int value : values) {
                binary_heap.insert(value);
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto fill_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            
            outFile << "Binary Heap:\n";
            outFile << "Fill time: " << fill_time << " microseconds\n";
            
            // Test getMax
            start = std::chrono::high_resolution_clock::now();
            for (int j = 0; j < NUM_OPERATIONS; j++) {
                binary_heap.getMax();
            }
            end = std::chrono::high_resolution_clock::now();
            auto get_max_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            
            outFile << "GetMax time (1000 operations): " << get_max_time << " microseconds\n";
            outFile << "Average time per getMax: " << (double)get_max_time / NUM_OPERATIONS << " microseconds\n";
            
            // Test extractMax
            start = std::chrono::high_resolution_clock::now();
            for (int j = 0; j < NUM_OPERATIONS; j++) {
                binary_heap.extractMax();
            }
            end = std::chrono::high_resolution_clock::now();
            auto extract_max_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            
            outFile << "ExtractMax time (1000 operations): " << extract_max_time << " microseconds\n";
            outFile << "Average time per extractMax: " << (double)extract_max_time / NUM_OPERATIONS << " microseconds\n";
            
            // Test insert
            std::vector<int> new_values(NUM_OPERATIONS);
            for (int& val : new_values) {
                val = std::uniform_int_distribution<>(0, N * 2)(gen);
            }
            
            start = std::chrono::high_resolution_clock::now();
            for (int val : new_values) {
                binary_heap.insert(val);
            }
            end = std::chrono::high_resolution_clock::now();
            auto insert_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            
            outFile << "Insert time (1000 operations): " << insert_time << " microseconds\n";
            outFile << "Average time per insert: " << (double)insert_time / NUM_OPERATIONS << " microseconds\n\n";
        }
        
        // Test Binomial Heap
        {
            BinomialHeap<int> binomial_heap;
            
            // Fill heap
            auto start = std::chrono::high_resolution_clock::now();
            for (int value : values) {
                binomial_heap.insert(value);
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto fill_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            
            outFile << "Binomial Heap:\n";
            outFile << "Fill time: " << fill_time << " microseconds\n";
            
            // Test getMax
            start = std::chrono::high_resolution_clock::now();
            for (int j = 0; j < NUM_OPERATIONS; j++) {
                binomial_heap.getMax();
            }
            end = std::chrono::high_resolution_clock::now();
            auto get_max_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            
            outFile << "GetMax time (1000 operations): " << get_max_time << " microseconds\n";
            outFile << "Average time per getMax: " << (double)get_max_time / NUM_OPERATIONS << " microseconds\n";
            
            // Test extractMax
            start = std::chrono::high_resolution_clock::now();
            for (int j = 0; j < NUM_OPERATIONS; j++) {
                binomial_heap.extractMax();
            }
            end = std::chrono::high_resolution_clock::now();
            auto extract_max_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            
            outFile << "ExtractMax time (1000 operations): " << extract_max_time << " microseconds\n";
            outFile << "Average time per extractMax: " << (double)extract_max_time / NUM_OPERATIONS << " microseconds\n";
            
            // Test insert
            std::vector<int> new_values(NUM_OPERATIONS);
            for (int& val : new_values) {
                val = std::uniform_int_distribution<>(0, N * 2)(gen);
            }
            
            start = std::chrono::high_resolution_clock::now();
            for (int val : new_values) {
                binomial_heap.insert(val);
            }
            end = std::chrono::high_resolution_clock::now();
            auto insert_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            
            outFile << "Insert time (1000 operations): " << insert_time << " microseconds\n";
            outFile << "Average time per insert: " << (double)insert_time / NUM_OPERATIONS << " microseconds\n";
        }
    }
    
    outFile.close();
    return 0;
}
