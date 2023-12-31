# Data structure and system prototype
>Some cpp practice projects.

### Use & Test

How to use these template structures ?
```C++
// An example for In-Memory BPlusTree
#include "bp.h"

int main(int args, char ** argv) {
    /** 
     * BPTree<KeyType, ValueType, NodeSize> bptree;
     * During this declaration
     *      KeyType: Key type which must can be compared, 
     *          operation< & operation> implemented in this type
     *      ValuType: Value type, any type
     *      NodeSize: unsigned 32bite integer, 
     *          The maximum Key number inserted into one node
     *          Default is 5
     */

    BPTree<uint32_t, float, 10> bptree;
    bptree.insert(1, 0.1);      // Insert key and value once time
    // bptree.insert(1, 0.1, 2, 0.2, 3, 0.3);      // several pairs of key & value
    float* val = bptree.find(1);
    std::cout << "The value is " << *val << "\n";
    bptree[1] = 0.2;
    std::cout << "The changed value is " << *val << "\n";

    std::vector<float> arr = bptree.serialize();
    for (float v : arr) std::cout << v  << " ";
    std::cout << std::endl;

    return 0;
}
```

How to test these templates ?
```Shell
# enter project root directory
pwd

# compile
mkdir build && cd build
cmake ..
make

# execute test
cd output/bin
./ProjectMain
```

### Current data structure :
1. Binary Search Tree template (2 editions, source / develop)

    **Note**: BST has not been cearfully inspected in large data scenario.

2. In-Memory B+ Tree template (no parallel support, no perfect erase implementation)

    Tested more than 10 million data.

***More :*** 

    1. AVL implementation (Done)

    2. Jump Table (TODO)

    3. Log Structure Tree

    4. File System B+ Tree implementation
    
    5. Storage system prototype
   
    6. CMakeListx.txt
   
    7. Red Black Tree (TODO)

    Tech: lock-free, mmap