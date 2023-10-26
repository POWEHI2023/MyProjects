#ifndef BPLUSTREE_
#define BPLUSTREE_

#include <iostream>
#include <stdlib.h>
#include <memory>
#include <shared_mutex>
#include <cassert>
#include <vector>
#include <functional>


template <typename T, typename V, uint m = 5>
class Node;
template <typename T, typename V, uint m = 5>
class BPTree;

using uint = uint32_t;
template <typename T, typename V, uint m>
using bpNode = std::shared_ptr<Node<T, V, m>>;

enum class NodeType: uint8_t { LeafNode, InnerNode };

void crash (const std::string& msg) {
          std::cerr << "Error: " << msg << "\n";
          exit(1);
}

template <typename T>
inline void moveV2V(std::vector<T>& v1, std::vector<T>& v2, size_t start) noexcept;

template <typename T, typename V, uint m>
class Node {
public:
          typedef struct { const Node *left, *right; } Deliver;

          /**
           * Basic arguments
           */

          Node *next_leaf = nullptr, *before_leaf = nullptr, *parent = nullptr;
          NodeType _type { NodeType::LeafNode };
          bool _is_root { false };


          /**
           * Basic functions
           */

          Node(): _type(NodeType::LeafNode) {
                    if (m <= 0) exit(1); 
                    list.reserve(m);
                    if (_type == NodeType::LeafNode) value.reserve(m);
                    else next.reserve(m);
          }
          explicit Node(NodeType new_type): _type(new_type) { 
                    if (m <= 0) exit(1); 
                    list.reserve(m);
                    if (_type == NodeType::LeafNode) value.reserve(m);
                    else next.reserve(m);
          }
          ~Node();
          explicit Node(const Node*);
          explicit Node(const Node&);
          explicit Node(const Node&&);
          void operator=(const Node&) = delete;
          void operator=(const Node&&) = delete;

          struct RET { 
                    T x; 
                    struct two4one { V _v; bpNode<T, V, m> _n; } y;
                    bool _leaf;
                    RET(const T& x, const V& v): x(x) {y._v = v; _leaf = true; }
                    RET(const T& x, const bpNode<T, V, m>& n): x(x) { y._n = n; _leaf = false; }
          };
          const RET operator[](uint index) const {
                    if (index < 0 || index >= size()) {
                              printf("Outof bound of current node. operator[%d]\n", index);
                              exit(1);
                    }
                    if (_type == NodeType::LeafNode) return { list[index], value[index] };
                    else return { list[index], next[index] };
          }

          bool operator==(const Node& node) noexcept { return comp(node, [](const T& _x, const T& _y){ return _x == _y; }); }
          bool operator!=(const Node& node) { return !operator==(node); }
          bool operator>=(const Node& node) noexcept { return comp(node, [](const T& _x, const T& _y){ return _x >= _y; }); }
          bool operator<=(const Node& node) noexcept { return comp(node, [](const T& _x, const T& _y){ return _x <= _y; }); }
          bool operator>(const Node& node) noexcept { return comp(node, [](const T& _x, const T& _y){ return _x > _y; }); }
          bool operator<(const Node& node) noexcept { return comp(node, [](const T& _x, const T& _y){ return _x < _y; }); }

          /**
           * Some main functions
           */

          const bpNode<T, V, m> insert(const T& elem, const V& val) noexcept;
          const bpNode<T, V, m> insert(const T& elem, const bpNode<T, V, m>& node) noexcept;
          const bpNode<T, V, m> insert(const bpNode<T, V, m>& node) noexcept;
          template <typename... _Args>
          const bpNode<T, V, m> insert(const T& elem , const V& val, _Args&&... _args) noexcept {
                    insert(elem, val);
                    return insert(_args...);
          }
          template <typename... _Args>
          // const bpNode<T, V, m> insert(_Args&&... _args) noexcept { return insert(_args...); }
          
          const bpNode<T, V, m> insert(const RET& ret) noexcept { return ret._leaf ? insert(ret.x, ret.y._v) : insert(ret.x, ret.y._n); }
          void erase(const uint32_t index) noexcept;

          /**
           * Sample tool functions
           */

          inline constexpr int find_key(const T& elem) const noexcept { for (int i = 0; i < size(); ++i) if (list[i] == elem) return i; return -1; }
          inline constexpr const T get_key() const noexcept { return list[size() - 1]; }
          inline constexpr size_t size() const noexcept { return list.size(); }
          inline constexpr uint capacity() const noexcept { return m; }
private:
          
          /**
           * Node contents
           */

          std::vector<T> list;                    // Key
          std::vector<V> value;                   // Values which are stored
          std::vector<bpNode<T, V, m>> next;      // Next node if current node is InnerNode

          uint32_t limit = (m + 1) / 2;           // limit, the minmum number content stored in current node
                                        // when size() < limit, current node need to be merged

          // std::shared_mutex mtx;                         // RWLock

          /**
           * Inner tool functions
           */

          void _force_del(const uint32_t index) noexcept = delete;
          template <typename... _Args>
          void _force_del(const uint32_t first, const _Args&... others) {
                    _force_del(first);
                    _force_del(others...);
          }
          inline bool comp(const Node& node, const std::function<bool(const T& _x, const T& _y)>& fn) noexcept {
                    bool ret = size() == node.size();
                    if (!ret) {
                              printf("Two compared Nodes have different size.\n");
                              exit(1);
                    }
                    for (int i = 0; ret && i < size(); ++i)
                    ret &= fn(list[i], node.list[i]);
                    return std::move(ret);
          }
          inline void copy_list(const Node& node) noexcept {
                    list = node.list;
                    value = node.value;
                    next = node.next;
          }
          inline void copy_list(const Node&& node) noexcept {
                    list = std::move(node.list);
                    value = std::move(node.value);
                    next = std::move(node.next);
                    // crt_size = std::move(node.crt_size);
          }

          const bpNode<T, V, m> split() noexcept;
          void merge () noexcept;

          template <typename ValueType>
          const bpNode<T, V, m> insert_(const T& key, ValueType&& value, bool type) noexcept;
          void change_key(const T& old_key, const T& new_key) noexcept;
          void change_key(const int pos, const T& new_key, int) noexcept;

          inline int32_t pfind(const T& key) const noexcept;
public:
          friend class BPTree<T, V, m>;

          /**
           * It is advisable to use these functions to create a new node
           */

          static const Node* create_node_ (NodeType type = NodeType::LeafNode) noexcept { return new Node(type); }
          template <typename... _Args>
          static const Node* create_node_ (NodeType type, const _Args&... _args) noexcept {
                    size_t size = sizeof...(_args) / 2;
                    if (size > m) {
                              printf("Too many arguments given(%ld) than template size m(%d)\n", size, m);
                              exit(1);
                    }

                    Node* ret = new Node(type);
                    ret->insert(_args...);
                    return std::move(ret);
          }
          static const bpNode<T, V, m> create_node (NodeType type = NodeType::LeafNode) noexcept { return bpNode<T, V, m>(const_cast<Node*>(create_node_(type))); }
          template <typename... _Args>
          static const bpNode<T, V, m> create_node (NodeType type, const _Args&... _args) noexcept {
                    size_t size = sizeof...(_args) / 2;
                    if (size > m) {
                              printf("Too many arguments given(%ld) than template size m(%d)\n", size, m);
                              exit(1);
                    }
                    auto ret = bpNode<T, V, m>(const_cast<Node*>(create_node_(type))); 
                    ret->insert(_args...);
                    return std::move(ret);
          }

          void check();
};

template <typename T>
inline void moveV2V(std::vector<T>& v1, std::vector<T>& v2, size_t start) noexcept {
          for (
                    typename std::vector<T>::iterator iter = v1.begin() + start;
                    iter != v1.end();
                    iter++
          ) v2.push_back(*iter);
          for (int i = start, len = v1.size(); i < len; ++i) v1.pop_back();
}

template <typename T, typename V, uint m>
inline bool operator==(const Node<T, V, m>& x, const Node<T, V, m>& y) noexcept { return x.operator==(y); }
template <typename T, typename V, uint m>
inline bool operator!=(const Node<T, V, m>& x, const Node<T, V, m>& y) noexcept { return x.operator!=(y); }
template <typename T, typename V, uint m>
inline bool operator>(const Node<T, V, m>& x, const Node<T, V, m>& y) noexcept { return x.operator>(y); }
template <typename T, typename V, uint m>
inline bool operator>=(const Node<T, V, m>& x, const Node<T, V, m>& y) noexcept { return x.operator>=(y); }
template <typename T, typename V, uint m>
inline bool operator<(const Node<T, V, m>& x, const Node<T, V, m>& y) noexcept { return x.operator<(y); }
template <typename T, typename V, uint m>
inline bool operator<=(const Node<T, V, m>& x, const Node<T, V, m>& y) noexcept { return x.operator<=(y); }

/**
 * B+ Tree Entity
 * B+ Tree Template arguments
 *        T: Key type
 *        V: Value type
 *        m: Maxmun number stored, limit will be [(m + 1) / 2, m]
 */

template <typename T, typename V, uint m>
class BPTree {
public:
          BPTree();
          ~BPTree();

          class iterator {
          public:
                    iterator(const Node<T, V, m>* node);
                    ~iterator();

                    const iterator operator++();
                    iterator& operator++(int);
                    const iterator operator--();
                    iterator& operator--(int);
          private:
                    Node<T, V, m>* node;
                    uint ptr;
          public:
          };

          bool is_empty() const noexcept;
          void insert(const T& key, const V& val);

          const V* find(const T& key) const noexcept ;
          V& operator[](const T& key) const noexcept ; 

          bool tombstone(const T& key);
          bool erase(const T& key);
          bool exist(const T& key);

          std::vector<V> serialize() const;

          void check();
          void check_serialize(std::vector<V>& arr);

          void check_serialize_(std::vector<V>& arr, const bpNode<T, V, m>& node);
private:
          bpNode<T, V, m> root;
public:

};

#endif