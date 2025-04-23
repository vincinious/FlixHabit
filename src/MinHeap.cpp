/* Completed by Vinicius Intravartola */

#include "MinHeap.h"
#include "User.h"
#include <algorithm>  
#include <filesystem>
#include <limits>


using namespace std;


/* ---------------- Unbounded MinHeap ---------------- */

template<typename T>
MinHeap<T>::MinHeap() { }

template<typename T>
unsigned int MinHeap<T>::getSize() const  { return heap.size(); }

/* Return the parent index */
template <typename T>
int MinHeap<T>::getParent(int i)   { return (i - 1) / 2; }

/* Return the index of the left child */
template <typename T>
int MinHeap<T>::getLChild(int i)    { return 2 * i + 1; }

/* Return the index of the right child */
template <typename T>
int MinHeap<T>::getRChild(int i)   { return 2 * i + 2; }

/* Percolate the new element up to its appropriate place in the min heap */
template <typename T>
void MinHeap<T>::heapifyUp(int i) 
{
    /* As long as the parent is larger, swap the parent and child */
    while (i > 0 
           && heap[i] < heap[getParent(i)])
    {
        T temp = heap[i];
        heap[i] = heap[getParent(i)];
        heap[getParent(i)] = temp;
        i = getParent(i);
    }

}

/* Push the new root down after the min is removed from the heap */
template <typename T>
void MinHeap<T>::heapifyDown(int i) 
{
    int left = getLChild(i);
    int right = getRChild(i);
    int smallest = i;

    if (left < heap.size() && heap[left] < heap[smallest])  { smallest = left; }

    if (right < heap.size() && heap[right] < heap[smallest])    { smallest = right; }
    
    /* If the child is smaller than the current element in the heap, swap them both */
    if (smallest != i) 
    {
        T temp = heap[i];
        heap[i] = heap[smallest];
        heap[smallest] = temp;
        heapifyDown(smallest);
    }

}

/* Insert a node at the end of the heap containing a difference attribute */
template <typename T>
void MinHeap<T>::insert(const T& element) 
{
    heap.push_back(element);
    heapifyUp(heap.size() - 1);

    if (heap.size() > capacity)   { removeMin(); }

}

/* Return the root of the heap */
template <typename T>
T MinHeap<T>::getMin() const 
{
    /* If no root, throw err messsage */
    if (heap.empty())   { throw runtime_error("Heap is empty!"); }

    return heap[0];

}

/* Remove the root of the heap and replace it with the last element; heapify down if necessary */
template <typename T>
void MinHeap<T>::removeMin()
{
    /* If no root, throw error messsage */
    if (heap.empty() == true)   { throw runtime_error("Heap is empty!"); }

    heap[0] = heap[heap.size() - 1];
    heap.pop_back();

    if (heap.empty() == false)  { heapifyDown(0); }

}

template class MinHeap<UserSimilarity>;


/* ---------------- Fixed-Size MinHeap ---------------- */

template<typename T>
int FixedMinHeap<T>::getParent(int i) const   { return (i - 1) / 2; }

template<typename T>
int FixedMinHeap<T>::getLChild(int i) const   { return 2 * i + 1; }

template<typename T>
int FixedMinHeap<T>::getRChild(int i) const   { return 2 * i + 2; }

template<typename T>
void FixedMinHeap<T>::heapifyUp(int i) 
{
    while (i > 0 && heap[i] < heap[getParent(i)]) 
    {
        swap(heap[i], heap[getParent(i)]);
        i = getParent(i);
    }
}

template<typename T>
void FixedMinHeap<T>::heapifyDown(int i) 
{
    int smallest = i;
    int l = getLChild(i), r = getRChild(i);
    if (l < (int)heap.size() && heap[l] < heap[smallest]) smallest = l;
    if (r < (int)heap.size() && heap[r] < heap[smallest]) smallest = r;
    if (smallest != i)
    {
        swap(heap[i], heap[smallest]);
        heapifyDown(smallest);
    }
}

template<typename T>
void FixedMinHeap<T>::insert(const T& val) 
{
    if (capacity == 0) return;
    heap.push_back(val);
    heapifyUp((int)heap.size() - 1);
    if (heap.size() > capacity) removeMin();
}

template<typename T>
T FixedMinHeap<T>::getMin() const 
{
    if (heap.empty()) throw runtime_error("Heap is empty");
    return heap[0];
}

template<typename T>
void FixedMinHeap<T>::removeMin()
{
    if (heap.empty()) throw runtime_error("Heap is empty");
    heap[0] = heap.back();
    heap.pop_back();
    if (!heap.empty()) heapifyDown(0);
}

template<typename T>
unsigned int FixedMinHeap<T>::size() const   { return heap.size(); }

template<typename T>
FixedMinHeap<T>::FixedMinHeap(unsigned int capacity)
    : capacity(capacity)
{}

template<typename T>
bool FixedMinHeap<T>::empty() const  { return heap.empty(); }

template class FixedMinHeap<UserWatch>;
