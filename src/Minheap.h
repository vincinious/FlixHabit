
/* Completed by Vinicius Intravartola */

#include <vector>
#include <stdexcept>

using namespace std;

/* Min Heap implementation: Use the root (smallest distance from different users' profile metrics) to get the nearest profiles (users with the most similar interests)*/

/* templated MinHeap(class) for any data element type */
template <typename T>
class MinHeap 
{
    private:

        vector <T> heap;

        int getParent(int i);
        int getLChild(int i);
        int getRChild(int i);
        void heapifyUp(int i);
        void heapifyDown(int i);

    public:

        void insert(const T& element);
        T getMin() const;
        void removeMin();

};


