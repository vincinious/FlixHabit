/* Completed by Vinicius Intravartola */

#include "MinHeap.h"
#include "User.h"
#include <algorithm>  


using namespace std;

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
    /* If no root, throw err messsage */
    if (heap.empty() == true)   { throw runtime_error("Heap is empty!"); }

    heap[0] = heap[heap.size() - 1];
    heap.pop_back();

    if (heap.empty() == false)  { heapifyDown(0); }

}

template class MinHeap<UserSimilarity>;
