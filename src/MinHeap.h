
/* Completed by Vinicius Intravartola */
#pragma once

#include <vector>
#include <stdexcept>
#include <filesystem>


using namespace std;

/* Min Heap implementation: Use the root (smallest distance from different users' profile metrics) to get the nearest profiles (users with the most similar interests)*/

/* templated MinHeap(class) for any data element type */
template <typename T>

/* ---------------- Unbounded MinHeap (option 6) ---------------- */
class MinHeap 
{
    private:

        vector <T> heap;
        unsigned int capacity;

        int getParent(int i);
        int getLChild(int i);
        int getRChild(int i);
        void heapifyUp(int i);
        void heapifyDown(int i);

    public:

        MinHeap();

        unsigned int getSize() const;
        void insert(const T& element);
        T getMin() const;
        void removeMin();


};

/* ---------------- Fixed-Size MinHeap ---------------- */
template<typename T>
class FixedMinHeap 
{
   public:

        FixedMinHeap(unsigned int capacity = SIZE_MAX);
        bool empty() const;

        void insert(const T& val);
        T getMin() const;
        void removeMin();
        unsigned int size()  const;

    private:

        vector<T> heap;
        unsigned int capacity;

        int getParent(int i) const;
        int getLChild(int i) const;
        int getRChild(int i) const;
        void heapifyUp(int i);
        void heapifyDown(int i);
};
