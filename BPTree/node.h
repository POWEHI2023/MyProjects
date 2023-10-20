#include <iostream>
#include <stdlib.h>
#include <memory.h>

#define InnerNode 0x01
#define LeafNode 0x02

template <typename T, uint32_t m>
class Node;

/**
 * Node for B+ tree, involve InnerNode and LeafNode
 * @details
 * InnerNode: 
 *        Key value & pointer to next layer nodes
 * LeafNode: 
 *        Key value and Real value (A block address or offset for a block)
 * @param keys Represent intervals for next layer node
 * @param next (Just used in InnerNode) point to next layer nodes
 * @param addr (Just used in LeafNode) point to a 32bit address or something else
 * @param next_leaf (Just used in LeafNode) point to next LeafNode
 * @param limit Avoid reptating caculate, represent underlimit of key number for each node
 * @param parent Point to upper layer InnerNode, the value is nullptr for root node
 */
template <typename T, uint32_t m>
class Node {
public:
          // Deliver is used for split operator
          typedef struct { T elem; Node *left, *right; } Deliver;

          /**
           * Constructor for a new Node, default type is LeafNode
           * @param new_type Represent type for the constructed node
           */
          Node(unsigned char new_type = LeafNode): _type(new_type) { }
          virtual ~Node();

          /**
           * Insert element into current node
           * @details
           *        1. If the number of elements in current node greater than `m`
           *        2. Then split current current node into two nodes, one have limit number of elements, one have m - limit + 1 elements
           *        3. Construct a Deliver struct, which have the last key in the first node, and two pointer point to the two nodes
           *        4. Deliver the constructed to `parent` node if the parent pinter is not nullptr, or construct a new root
           *        5. When insert complished, return the new root if a new root has been constructed or nullptr
           * @param elem The element which will be inserted into current node
           */
          void insert(const T& elem);

          /**
           * Variable count of insert value, can be used when insert more than one elements
           * @param elem The current element be inserted, with type T
           * @param _args The other elements will be inserted, just have the same type with T, or trigger an error
           */
          template <typename... _Args>
          void insert(const T& elem, const _Args&... _args);

          uint32_t find_key(const T& elem) const;

          const Node* find_neighbour(const uint32_t index) const;

          void erase(const uint32_t index);
private:
          T* keys = nullptr;
          // Used when is InnerNode
          Node** next = nullptr;
          // Used when is LeafNode
          uint32_t* addr = nullptr;
          Node *next_leaf = nullptr, *parent = nullptr;
          uint32_t limit = (m + 1) / 2;

          const Deliver split();
          void _force_del(const uint32_t index);
public:
          unsigned char _type = LeafNode;
};