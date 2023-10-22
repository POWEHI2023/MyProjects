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
 * @param _is_root If `true`, m and limit have no force to current node,
 *                when current is InnerNode, at least 2 elements and 2 links,
 *                when current is LeafNode, at least 0 element.
 * @param type The type for current node
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
           * @return Pointer to a new node if there is new node be constructed, or nullptr
           */
          const Node* insert(const T& elem);

          /**
           * Variable count of insert value, can be used when insert more than one elements
           * @param elem The current element be inserted, with type T
           * @param _args The other elements will be inserted, just have the same type with T, or trigger an error
           * @return Pointer to a new node if there is new node be constructed, or nullptr
           */
          template <typename... _Args>
          const Node* insert(const T& elem, const _Args&... _args);

          /**
           * Find the index for elem in current node, return -1 when no matched element
           * @param elem Target element will be located
           */
          uint32_t find_key(const T& elem) const;

          /**
           * Called when erase elements which cause need to merge sibling nodes, find the fittable node and return the pointer to that node
           * @details
           *        Visit from left sibling to right sibling, from the closest to the farest.
           *        When the origin node is 3 and array is 1, 2, 3, 4, 5, the order is 2 -> 4 -> 1 -> 5 until find a fittable node
           *        If cannot find a fittable node, then process the true merge which merge two node into one.
           *        (Fake merge): Find a node has more than limit number elements, split one closest element into current node.
           * @param index Stored key which represent to a next layer node.
           * @return Pointer to the fittable neighbour node to merge.
           */
          const Node* find_neighbour(const uint32_t index) const;

          /**
           * Erase a element in index.
           * @details
           *        Need to merge some other elements when necessary.
           *        Merge process as described in @cite{find_neighbour}
           * @param index Index for the element which will be erased.
           */
          void erase(const uint32_t index);
private:
          T* keys = nullptr;
          // Used when is InnerNode
          Node** next = nullptr;
          // Used when is LeafNode
          uint32_t* addr = nullptr;
          Node *next_leaf = nullptr, *parent = nullptr;
          uint32_t limit = (m + 1) / 2;

          /**
           * Split current node into two nodes with limit and m - limit + 1 elements respectively
           * @details
           *        Used when insert a new element, but have involve m elements, 
           *        then split into two node, with floor(m / 2) and ceil(m / 2)
           *        Insert into new element into the node with floor number of elements, 
           *        then the element number for two nodes is limie and m - limit + 1
           */
          const Deliver split();

          /**
           * Delete element in index without other process
           * @param index The index will be force deleted in current node
           */
          void _force_del(const uint32_t index);

          /**
           * Used when delete multiple elements one time
           * @param first First index of multiple indexes
           * @param others Indexes beside first in totle indexes
           */
          template <typename... _Args>
          void _force_del(const uint32_t first, const _Args&... others) {
                    _force_del(first);
                    _force_del(others...);
          }
public:
          unsigned char _type { LeafNode };
          bool _is_root { false };
};