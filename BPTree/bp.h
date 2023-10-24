#include <iostream>
#include <stdlib.h>
#include <memory.h>


using uint = uint32_t;

// class AbstractNode;
template <typename T, typename V, uint m = 5>
class Node;
template <typename KeyType, typename ValueType>
class Element;
template <typename T, typename V, uint m = 5>
class BPTree;
template <typename T, typename V, uint m>
using bpNode = std::shared_ptr<Node<T, V, m>>;
template <typename T, typename Y>
using bpElement = std::shared_ptr<Element<T, Y>>;

enum class NodeType: uint8_t { LeafNode, InnerNode };

/*class AbstractNode {
public:
          AbstractNode();
          virtual ~AbstractNode();

          // Deliver is used for split operator
          typedef struct { const AbstractNode *left, *right; } Deliver;

          virtual const void* get_key() const = 0;
          // virtual const AbstractNode* find_neighbour(const uint index) const = 0;
          virtual const Deliver split() = 0;
          virtual size_t size() const = 0;

          virtual void _force_del(const uint index) {
                    printf("Can not call _force_del in AbstractNode.\n");
                    exit(1);
          }
          virtual bool set_tombstone(const uint index) {
                    printf("Can not call set_tombstone in AbstractNode.\n");
                    exit(1);
          }
};*/

template <typename T, typename V, uint m>
inline bool operator==(const Node<T, V, m>& x, const Node<T, V, m>& y) { return x.operator==(y); }
template <typename T, typename V, uint m>
inline bool operator!=(const Node<T, V, m>& x, const Node<T, V, m>& y) { return x.operator!=(y); }
template <typename T, typename V, uint m>
inline bool operator>(const Node<T, V, m>& x, const Node<T, V, m>& y) { return x.operator>(y); }
template <typename T, typename V, uint m>
inline bool operator>=(const Node<T, V, m>& x, const Node<T, V, m>& y) { return x.operator>=(y); }
template <typename T, typename V, uint m>
inline bool operator<(const Node<T, V, m>& x, const Node<T, V, m>& y) { return x.operator<(y); }
template <typename T, typename V, uint m>
inline bool operator<=(const Node<T, V, m>& x, const Node<T, V, m>& y) { return x.operator<=(y); }

/**
 * Node for B+ tree, involve InnerNode and LeafNode
 * @details
 * InnerNode: 
 *        Key value & pointer to next layer nodes
 * LeafNode: 
 *        Key value and Real value (A block address or offset for a block)
 * @param list Elements list, each element involve one Key and one Value
 * @param next_leaf (Just used in LeafNode) point to next LeafNode
 * @param limit Avoid reptating caculate, represent underlimit of key number for each node
 * @param parent Point to upper layer InnerNode, the value is nullptr for root node
 * @param _is_root If `true`, m and limit have no force to current node,
 *                when current is InnerNode, at least 2 elements and 2 links,
 *                when current is LeafNode, at least 0 element.
 * @param _type The type for current node
 * @param _destroyed Used for iterator or as a tombstone mark
 */
template <typename T, typename V, uint m>
class Node /*: public AbstractNode*/ {
public:
          typedef struct { const Node *left, *right; } Deliver;
          
          Node *next_leaf = nullptr, *before_leaf = nullptr, *parent = nullptr;
          T* parend_element = nullptr;

          NodeType _type { NodeType::LeafNode };
          // bool _is_root { false };

          /**
           * Constructor for a new Node, default type is LeafNode
           * @param new_type Represent type for the constructed node
           */
          Node(NodeType new_type = NodeType::LeafNode): _type(new_type),
          list((T*)malloc(sizeof(T) * m)), value((T*)malloc(sizeof(V) * m))
          { if (m <= 0) exit(1); }

          virtual ~Node();

          explicit Node(const Node*);
          explicit Node(const Node&);
          explicit Node(const Node&&);
          void operator=(const Node&) = delete;
          void operator=(const Node&&) = delete;

          decltype(auto) operator[](uint index) const;

          bool operator==(const Node& node) 
          { return comp(node, [](const T& _x, const T& _y){ return _x == _y; }); }
          bool operator!=(const Node& node) { return !operator==(node); }
          bool operator>=(const Node& node)
          { return comp(node, [](const T& _x, const T& _y){ return _x >= _y; }); }
          bool operator<=(const Node& node)
          { return comp(node, [](const T& _x, const T& _y){ return _x <= _y; }); }
          bool operator>(const Node& node)
          { return comp(node, [](const T& _x, const T& _y){ return _x > _y; }); }
          bool operator<(const Node& node)
          { return comp(node, [](const T& _x, const T& _y){ return _x < _y; }); }

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
          const void insert(const T& elem, const V& val);

          /**
           * Variable count of insert value, can be used when insert more than one elements
           * @param elem The current element be inserted, with type T
           * @param _args The other elements will be inserted, just have the same type with T, or trigger an error
           * @return Pointer to a new node if there is new node be constructed, or nullptr
           */
          template <typename... _Args>
          const void insert(const T& elem , const V& val, const _Args&... _args) {
                    insert(elem, val);
                    return insert(_args...);
          }

          /**
           * Split current node into two nodes with limit and m - limit + 1 elements respectively
           * @details
           *        Used when insert a new element, but have involve m elements, 
           *        then split into two node, with floor(m / 2) and ceil(m / 2)
           *        Insert into new element into the node with floor number of elements, 
           *        then the element number for two nodes is limie and m - limit + 1
           */
          virtual const Deliver split() /*override*/;       //***

          uint find_key(const T& elem) const;

          virtual const T get_key() const /*override*/
          { return list[crt_size - 1]; }

          /**
           * Erase a element in index.
           * @details
           *        Need to merge some other elements when necessary.
           *        Merge process as described in @cite{find_neighbour}
           * @param index Index for the element which will be erased.
           */
          size_t erase(const uint32_t index);       //***

          virtual const Deliver merge ();

          virtual size_t size() const /*override*/ { return crt_size; }
          constexpr uint capacity() const noexcept { return m; }
private:
          size_t crt_size = 0;
          T* list = nullptr;
          V* value = nullptr;

          uint32_t limit = (m + 1) / 2;

          /**
           * Delete element in index without other process
           * @param index The index will be force deleted in current node
           */
          virtual void _force_del(const uint32_t index) /*override*/; //***

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

          inline bool comp(const Node& node, const std::function<bool(const T& _x, const T& _y)>& fn) {
                    bool ret = crt_size == node.crt_size;
                    if (!ret) {
                              printf("Two compared Nodes have different size.\n");
                              exit(1);
                    }
                    for (int i = 0; ret && i < crt_size; ++i)
                    ret &= fn(list[i], node.list[i]);
                    return std::move(ret);
          }

          inline void copy_list(const Node& node) {
                    if (list == nullptr) 
                    list = static_cast<T*>(malloc(m * sizeof(T)));

                    for (int i = 0; i < node.crt_size; ++i)
                    list[i] = node.list[i];

                    if (value == nullptr) 
                    value = static_cast<T*>(malloc(m * sizeof(V)));

                    for (int i = 0; i < node.crt_size; ++i)
                    value[i] = node.value[i];

                    crt_size = node.crt_size;
          }

          inline void copy_list(const Node&& node) {
                    if (list != nullptr) free(list);
                    list = std::move(node.list);

                    if (value != nullptr) free(value);
                    value = std::move(node.value);

                    crt_size = std::move(node.crt_size);
          }
public:
          friend class BPTree<T, V, m>;

          /**
           * Create functions, it is advisable to use these function to create bpNode than manully construct Node.
           * @details
           *        User can deliver series of key
           */ 

          static const Node* create_node_ (NodeType type = NodeType::LeafNode)
          { return new Node(type); }

          template <typename... _Args>
          static const Node* create_node_ (NodeType type, const _Args&... _args) {
                    size_t size = sizeof...(_args) / 2;
                    if (size > m) {
                              printf("Too many arguments given(%ld) than template size m(%d)\n", size, m);
                              exit(1);
                    }

                    Node* ret = new Node(type);
                    ret->insert(_args...);
                    return std::move(ret);
          }

          static const bpNode<T, V, m> create_node (NodeType type = NodeType::LeafNode) 
          { return bpNode<T, V, m>(create_node_(type)); }

          template <typename... _Args>
          static const bpNode<T, V, m> create_node (NodeType type, const _Args&... _args) {
                    size_t size = sizeof...(_args) / 2;
                    if (size > m) {
                              printf("Too many arguments given(%ld) than template size m(%d)\n", size, m);
                              exit(1);
                    }
                    auto ret = bpNode<T, V, m>(const_cast<Node*>(create_node_(type))); 
                    ret->insert(_args...);
                    return std::move(ret);
          }
};

template <typename KeyType, typename ValueType>
struct Element {
          typedef KeyType key_type;
          typedef ValueType value_type;

          KeyType key;
          ValueType value;
          bool is_leaf { true } , tombstone { false };


          Element(KeyType&& key, ValueType&& value, bool type);
          ~Element() { }

          Element(const Element*);
          Element(const Element&);
          Element(const Element&&);

          void operator=(const Element&);
          void operator=(const Element&&);

          std::pair<KeyType, ValueType> get_content () const
          { return {key, value}; }

          void set_key(const KeyType& key) { this->key = key; }
          void set_value(const ValueType& val) { this->value = val; }

          static const Element* create_element_ (const KeyType& key, const ValueType& val);
          static const Element* create_element_ (const KeyType&& key, const ValueType&& val);
          static const bpElement<KeyType, ValueType> create_element (const KeyType& key, const ValueType& val);
          static const bpElement<KeyType, ValueType> create_element (const KeyType&& key, const ValueType&& val);
};

/**
 * Compare operations for 
 *        Key2Key (Original involved) & 
 *        Value2Value (If Value is pointer to a node, transfer pointer to uint32 and compare)
 */

template <typename KeyType, typename ValueType>
inline bool operator==(const Element<KeyType, ValueType>& x, const Element<KeyType, ValueType>& y) {
          return x.key == y.key;
}
template <typename KeyType, typename ValueType>
inline bool operator!=(const Element<KeyType, ValueType>& x, const Element<KeyType, ValueType>& y) {
          return x.key != y.key;
}
template <typename KeyType, typename ValueType>
inline bool operator>(const Element<KeyType, ValueType>& x, const Element<KeyType, ValueType>& y) {
          return x.key > y.key;
}
template <typename KeyType, typename ValueType>
inline bool operator>=(const Element<KeyType, ValueType>& x, const Element<KeyType, ValueType>& y) {
          return x.key >= y.key;
}
template <typename KeyType, typename ValueType>
inline bool operator<(const Element<KeyType, ValueType>& x, const Element<KeyType, ValueType>& y) {
          return x.key < y.key;
}
template <typename KeyType, typename ValueType>
inline bool operator<=(const Element<KeyType, ValueType>& x, const Element<KeyType, ValueType>& y) {
          return x.key <= y.key;
}




/**
 * B+ Tree Entity
 */
template <typename T, typename V, uint m>
class BPTree {
public:

private:
          bpNode<T, V, m> root;
public:

};
