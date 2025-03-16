#include <iostream>
#include <random>
#include <string>
#include <chrono>
#include <array>

struct Person {
    std::string lastName;
    std::string firstName;
    std::string middleName;
    std::chrono::system_clock::time_point birthDate;
    
    Person() = default;
    Person(const std::string& last, const std::string& first, const std::string& middle, 
           const std::chrono::system_clock::time_point& birth)
        : lastName(last), firstName(first), middleName(middle), birthDate(birth) {}
};

// Stack implementation using linked list
template<typename T>
class LinkedListStack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& value) : data(value), next(nullptr) {}
    };
    
    Node* top;
    size_t size;

public:
    class Iterator {
    private:
        Node* current;
    public:
        Iterator(Node* node) : current(node) {}
        
        T& operator*() { return current->data; }
        
        Iterator& operator++() {
            current = current->next;
            return *this;
        }
        
        bool operator!=(const Iterator& other) {
            return current != other.current;
        }
    };

    LinkedListStack() : top(nullptr), size(0) {}
    
    ~LinkedListStack() {
        while (!isEmpty()) {
            pop();
        }
    }

    void push(const T& value) {
        Node* newNode = new Node(value);
        newNode->next = top;
        top = newNode;
        size++;
    }

    T pop() {
        if (isEmpty()) {
            throw std::runtime_error("Stack is empty");
        }
        Node* temp = top;
        T value = temp->data;
        top = top->next;
        delete temp;
        size--;
        return value;
    }

    bool isEmpty() const {
        return top == nullptr;
    }

    size_t getSize() const {
        return size;
    }

    Iterator begin() { return Iterator(top); }
    Iterator end() { return Iterator(nullptr); }
};

// Stack implementation using array
template<typename T>
class ArrayStack {
private:
    static const size_t INITIAL_CAPACITY = 10;
    T* array;
    size_t capacity;
    size_t size;

public:
    class Iterator {
    private:
        T* current;
        size_t index;
        size_t size;
    public:
        Iterator(T* arr, size_t idx, size_t s) : current(arr), index(idx), size(s) {}
        
        T& operator*() { return current[index]; }
        
        Iterator& operator++() {
            index++;
            return *this;
        }
        
        bool operator!=(const Iterator& other) {
            return index != other.index;
        }
    };

    ArrayStack() : capacity(INITIAL_CAPACITY), size(0) {
        array = new T[capacity];
    }
    
    ~ArrayStack() {
        delete[] array;
    }

    void push(const T& value) {
        if (size == capacity) {
            resize(capacity * 2);
        }
        array[size++] = value;
    }

    T pop() {
        if (isEmpty()) {
            throw std::runtime_error("Stack is empty");
        }
        T value = array[--size];
        if (size > 0 && size < capacity / 4) {
            resize(capacity / 2);
        }
        return value;
    }

    bool isEmpty() const {
        return size == 0;
    }

    size_t getSize() const {
        return size;
    }

private:
    void resize(size_t newCapacity) {
        T* newArray = new T[newCapacity];
        for (size_t i = 0; i < size; i++) {
            newArray[i] = array[i];
        }
        delete[] array;
        array = newArray;
        capacity = newCapacity;
    }

public:
    Iterator begin() { return Iterator(array, 0, size); }
    Iterator end() { return Iterator(array, size, size); }
};

// Test data generation
const std::array<std::string, 10> LAST_NAMES = {
    "Smith", "Johnson", "Williams", "Brown", "Jones",
    "Garcia", "Miller", "Davis", "Rodriguez", "Martinez"
};

const std::array<std::string, 10> FIRST_NAMES = {
    "James", "John", "Robert", "Michael", "William",
    "David", "Richard", "Joseph", "Thomas", "Charles"
};

const std::array<std::string, 10> MIDDLE_NAMES = {
    "Alexander", "Benjamin", "Christopher", "Daniel", "Edward",
    "Francis", "George", "Henry", "Isaac", "Jacob"
};

Person generateRandomPerson(std::mt19937& gen) {
    std::uniform_int_distribution<> namesDist(0, 9);
    
    // Generate random date between 1980-01-01 and 2020-01-01
    std::uniform_int_distribution<> dateDist(0, 40*365);
    auto start = std::chrono::system_clock::from_time_t(315576000); // 1980-01-01
    auto birthDate = start + std::chrono::hours(24 * dateDist(gen));
    
    return Person(
        LAST_NAMES[namesDist(gen)],
        FIRST_NAMES[namesDist(gen)],
        MIDDLE_NAMES[namesDist(gen)],
        birthDate
    );
}

void testStackPerformance() {
    const size_t TEST_SIZE = 10000;
    LinkedListStack<Person> listStack;
    ArrayStack<Person> arrayStack;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Test insertion performance
    auto listStart = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < TEST_SIZE; i++) {
        listStack.push(generateRandomPerson(gen));
    }
    auto listEnd = std::chrono::high_resolution_clock::now();
    auto listDuration = std::chrono::duration_cast<std::chrono::microseconds>(listEnd - listStart);
    
    auto arrayStart = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < TEST_SIZE; i++) {
        arrayStack.push(generateRandomPerson(gen));
    }
    auto arrayEnd = std::chrono::high_resolution_clock::now();
    auto arrayDuration = std::chrono::duration_cast<std::chrono::microseconds>(arrayEnd - arrayStart);
    
    std::cout << "Performance Test Results (10000 elements):\n";
    std::cout << "LinkedListStack insertion time: " << listDuration.count() << " microseconds\n";
    std::cout << "ArrayStack insertion time: " << arrayDuration.count() << " microseconds\n\n";
    
    // Test inversion using only push/pop operations
    LinkedListStack<int> sortedListStack;
    ArrayStack<int> sortedArrayStack;
    
    // Fill with sorted numbers
    for (int i = 0; i < 100; i++) {
        sortedListStack.push(i);
        sortedArrayStack.push(i);
    }
    
    // Invert LinkedListStack
    LinkedListStack<int> tempList;
    while (!sortedListStack.isEmpty()) {
        tempList.push(sortedListStack.pop());
    }
    
    // Invert ArrayStack
    ArrayStack<int> tempArray;
    while (!sortedArrayStack.isEmpty()) {
        tempArray.push(sortedArrayStack.pop());
    }
    
    std::cout << "Stack inversion test:\n";
    std::cout << "First 5 elements after inversion (LinkedListStack):\n";
    for (int i = 0; i < 5 && !tempList.isEmpty(); i++) {
        std::cout << tempList.pop() << " ";
    }
    std::cout << "\n\nFirst 5 elements after inversion (ArrayStack):\n";
    for (int i = 0; i < 5 && !tempArray.isEmpty(); i++) {
        std::cout << tempArray.pop() << " ";
    }
    std::cout << "\n";
}

int main() {
    testStackPerformance();
    return 0;
}
