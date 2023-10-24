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

          NodeType _type { NodeType::LeafNode };
          bool _is_root { false };

          Node(): _type(NodeType::LeafNode) {
                    if (m <= 0) exit(1); 
                    list.reserve(m);
                    if (_type == NodeType::LeafNode) {
                              value.reserve(m);
                    } else {
                              next.reserve(m);
                    }
          }

          /**
           * Constructor for a new Node, default type is LeafNode
           * @param new_type Represent type for the constructed node
           */
          Node(NodeType new_type): _type(new_type) { 
                    if (m <= 0) exit(1); 
                    list.reserve(m);
                    if (_type == NodeType::LeafNode) {
                              value.reserve(m);
                    } else {
                              next.reserve(m);
                    }
          }


          virtual ~Node();

          explicit Node(const Node*);
          explicit Node(const Node&);
          explicit Node(const Node&&);
          void operator=(const Node&) = delete;
          void operator=(const Node&&) = delete;

          struct RET { 
                    struct two4one {
                              V _v;
                              bpNode<T, V, m> _n;

                              two4one() { }
                              ~two4one() { }
                    };

                    T x; 
                    two4one y;
                    bool _leaf;
                    RET(const T& x, const V& v) {
                              this->x = x;
                              y._v = v;
                              _leaf = true;
                    }
                    RET(const T& x, const bpNode<T, V, m>& v) {
                              
                              this->x = x;
                              // printf("In Constructor1\n");
                              y._n = v;
                              _leaf = false;
                              // printf("In Constructor2\n");
                    }
                    ~RET() { }
          };
          const RET operator[](uint index) const {
                    if (index < 0 || index >= crt_size) {
                              printf("Outof bound of current node. operator[%d]\n", index);
                              exit(1);
                    }
                    if (_type == NodeType::LeafNode)
                    return { list[index], value[index] };
                    else {
                              return { list[index], next[index] };
                    }
          }

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
          const bpNode<T, V, m> insert(const T& elem, const V& val);

          const bpNode<T, V, m> insert(const T& elem, const bpNode<T, V, m>& node);

          void change_key(const uint pos, const T& key) {
                    list[pos] = key;
          }

          /**
           * Variable count of insert value, can be used when insert more than one elements
           * @param elem The current element be inserted, with type T
           * @param _args The other elements will be inserted, just have the same type with T, or trigger an error
           * @return Pointer to a new node if there is new node be constructed, or nullptr
           */
          template <typename... _Args>
          const bpNode<T, V, m> insert(const T& elem , const V& val, const _Args&... _args) {
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
          virtual const bpNode<T, V, m> split() /*override*/;       //***

          uint find_key(const T& elem) const noexcept;

          virtual const T get_key() const noexcept /*override*/
          { return list[crt_size - 1]; }

          /**
           * Erase a element in index.
           * @details
           *        Need to merge some other elements when necessary.
           *        Merge process as described in @cite{find_neighbour}
           * @param index Index for the element which will be erased.
           */
          size_t erase(const uint32_t index);       //***

          virtual void merge ();

          virtual size_t size() const noexcept /*override*/ { return crt_size; }
          constexpr uint capacity() const noexcept { return m; }
private:
          size_t crt_size = 0;
          // T* list = nullptr;                      // Key list
          // V* value = nullptr;                     // Value Eitity
          std::vector<T> list;
          std::vector<V> value;
          std::vector<bpNode<T, V, m>> next;      // Next layer Nodes

          uint32_t limit = (m + 1) / 2;

          /**
           * Delete element in index without other process
           * @param index The index will be force deleted in current node
           */
          virtual void _force_del(const uint32_t index) = delete /*override*/; //***

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
                    list = node.list;
                    value = node.value;
                    next = node.next;
                    crt_size = node.crt_size;
          }

          inline void copy_list(const Node&& node) {
                    list = std::move(node.list);
                    value = std::move(node.value);
                    next = std::move(node.next);
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
          { return bpNode<T, V, m>(const_cast<Node*>(create_node_(type))); }

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
 * B+ Tree Entity
 */
template <typename T, typename V, uint m>
class BPTree {
public:

          BPTree();

private:
          bpNode<T, V, m> root;
public:

};
