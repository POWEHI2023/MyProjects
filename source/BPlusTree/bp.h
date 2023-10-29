#ifndef BPLUSTREE_
#define BPLUSTREE_

#include <iostream>
#include <stdlib.h>
#include <memory>
#include <shared_mutex>
#include <cassert>
#include <vector>
#include <functional>
#include <unordered_map>
#include <stack>

#include "../locker.h"

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





template <typename T, typename V, uint m>
Node<T, V, m>::Node(const Node* node): _type(node->_type), next_leaf(node->next_leaf), before_leaf(node->before_leaf), parent(node->parent)  { copy_list(*node); }
template <typename T, typename V, uint m>
Node<T, V, m>::Node(const Node& node): _type(node._type), next_leaf(node.next_leaf), before_leaf(node.before_leaf), parent(node.parent) { copy_list(node); }
template <typename T, typename V, uint m>
Node<T, V, m>::Node(const Node&& node): _type(std::move(node._type)), next_leaf(std::move(node.next_leaf)), before_leaf(std::move(node.before_leaf)), parent(std::move(node.parent)) { copy_list(std::move(node)); }
template <typename T, typename V, uint m>
Node<T, V, m>::~Node() { 
          list.clear(); value.clear(); next.clear();

          if (next_leaf != nullptr && before_leaf != nullptr) {
                    before_leaf->next_leaf = next_leaf; 
                    next_leaf->before_leaf = before_leaf;
          } else if (next_leaf != nullptr) {
                    next_leaf->before_leaf = nullptr;
          } else if (before_leaf != nullptr) {
                    before_leaf->next_leaf = nullptr;
          }
}

/**
 * Basic function implementation
 *        insert / erase
 *        split / merge
 */

template <typename T, typename V, uint m>
const bpNode<T, V, m> Node<T, V, m>::insert(const T& elem, const V& val) noexcept {
          if (std::find(list.begin(), list.end(), elem) != list.end()) {
                    printf("Key has inserted into B+ tree.\n");
                    return nullptr;
          }
          if (size() == m) { 
                    if (before_leaf != nullptr && before_leaf->size() < m) {
                                        // can be rotated
                              T k = list.front(); V v = value.front();
                              list.erase(list.begin()); value.erase(value.begin());
                              before_leaf->insert(k, v);
                              return insert(elem, val);
                    }

                    auto ret = split();                                         // split current node into two nodes，get a bpNode represent right half part
                    if (ret == nullptr) crash("Split get an empty node.");      // no next part, trigger an error
                    if (elem < get_key()) {
                              insert(elem, val);
                              if (ret->size() < limit) {
                                        int pos = -1; 
                                        if (parent != nullptr) pos = parent->find_key(get_key());
                                        ret->list.insert(ret->list.begin(), list.back()); list.pop_back();
                                        ret->value.insert(ret->value.begin(), value.back()); value.pop_back();
                                        if (pos != -1) parent->change_key(pos, get_key(), 0);
                              }
                    }
                    else ret->insert(elem, val);
                    return parent != nullptr ? parent->insert(ret->get_key(), ret) : ret;
          }

          int32_t position = std::max(pfind(elem), 0);
          list.insert(list.begin() + position, elem);
          value.insert(value.begin() + position, val);
          // last key is changed, parent also changes old key
          if (position == size() - 1 && parent != nullptr) parent->change_key(list[size() - 2], list[size() - 1]);
          return nullptr;
}
template <typename T, typename V, uint m>
const bpNode<T, V, m> Node<T, V, m>::insert(const T& elem, const bpNode<T, V, m>& node) noexcept {
          node->_is_root = false; node->parent = this;
          if (size() == m) { 
                    if (before_leaf != nullptr && before_leaf->size() < m) {
                              T k = list.front(); bpNode<T, V, m> v = next.front();
                              list.erase(list.begin()); next.erase(next.begin());
                              
                              before_leaf->parent->change_key(before_leaf->get_key(), k);
                              before_leaf->list.push_back(k);

                              v->parent = before_leaf;
                              before_leaf->next.push_back(v);

                              return insert(elem, node);
                    }

                              // 不可以旋转
                    auto ret = split();                                         
                    if (ret == nullptr) crash("Split get an empty node.");     
                    if (elem < get_key()) {
                              insert(elem, node);
                              if (ret->size() < limit) {
                                        int pos = -1;
                                        if (parent != nullptr) pos = parent->find_key(get_key());
                                        ret->list.insert(ret->list.begin(), list.back()); list.pop_back();
                                        next.back()->parent = ret.get();
                                        ret->next.insert(ret->next.begin(), next.back()); 
                                        next.pop_back(); 
                                        if (pos != -1) parent->change_key(pos, get_key(), 0);
                              }
                    }
                    else ret->insert(elem, node);
                    return parent != nullptr ? parent->insert(ret->get_key(), ret) : ret;
          }

          int32_t position = std::max(pfind(elem), 0);
          list.insert(list.begin() + position, elem);
          next.insert(next.begin() + position, node);
          if (position > 0) {                               // Before <-> Next & Node
                    Node* before_ = next[position - 1].get();
                    node->before_leaf = before_;
                    node->next_leaf = before_->next_leaf;
                    if (node->next_leaf != nullptr) node->next_leaf->before_leaf = node.get();
                    before_->next_leaf = node.get();
          } else if (position + 1 < size()) {
                    Node* next_ = next[position + 1].get();
                    node->next_leaf = next_;
                    node->before_leaf = next_->before_leaf;
                    if (node->before_leaf != nullptr) node->before_leaf->next_leaf = node.get();
                    next_->before_leaf = node.get();
          } 
          if (position == size() - 1 && parent != nullptr) parent->change_key(list[size() - 2], list[size() - 1]);
          return nullptr;
}
template <typename T, typename V, uint m>
const bpNode<T, V, m> Node<T, V, m>::insert(const bpNode<T, V, m>& node) noexcept { return insert(node->get_key(), node); }
template <typename T, typename V, uint m>
void Node<T, V, m>::erase(const uint index) noexcept {
          if (index < 0 || index >= size()) {
                    printf("Erase Error: Index(%d) out of bount(0 - %ld).\n", index, size() - 1);
                    exit(1);
          }
          if (size() <= 0) crash("Erase element in a empty node.");

          if (index == size() - 1 && parent != nullptr) parent->change_key(get_key(), list[size() - 2]);
          list.erase(list.begin() + index);
          if (_type == NodeType::LeafNode) value.erase(value.begin() + index);
          else next.erase(next.begin() + index);
          if (size() < limit && !_is_root) merge();
}
template <typename T, typename V, uint m>
const bpNode<T, V, m> Node<T, V, m>::split() noexcept {                         // split need change Key in parent node
          auto node = Node::create_node(_type);   // node with same type
          int pos = -1;
          if (parent != nullptr) pos = parent->find_key(get_key());
          // list moves to new node
          moveV2V(list, node->list, limit);
          if (_type == NodeType::LeafNode) moveV2V(value, node->value, limit);
          else {
                    for (
                              typename std::vector<bpNode<T, V, m>>::iterator iter = next.begin() + limit;
                              iter != next.end();
                              iter++
                    ) { (*iter)->parent = node.get(); node->next.push_back(*iter); }
                    for (int i = limit, len = next.size(); i < len; ++i) next.pop_back();
          }

          if (pos != -1) parent->change_key(pos, get_key(), 0);
          return node;
}
template <typename T, typename V, uint m>
void Node<T, V, m>::merge() noexcept {   
          if (before_leaf != nullptr && before_leaf->size() > limit) { 
                    insert((*before_leaf)[before_leaf->size() - 1]);
                    before_leaf->erase(before_leaf->size() - 1);
                    return;
          } 
          if (next_leaf != nullptr && next_leaf->size() > limit) { 
                    insert((*next_leaf)[0]);
                    next_leaf->erase(0);
                    return;
          }
          if (before_leaf != nullptr) {                                  
                    int pos = parent->find_key(before_leaf->get_key());
                    if (pos == -1) crash("Can not find key in parent list.");

                    for (int i = 0; i < size(); ++i) before_leaf->insert((*this)[i]);
                    before_leaf->next_leaf = next_leaf;
                    if (next_leaf != nullptr) next_leaf->before_leaf = before_leaf;

                    parent->erase(pos + 1);
                    return;
          }
          if (next_leaf != nullptr) {  
                    int pos = parent->find_key(get_key());
                    if (pos == -1) crash("Can not find key in parent list.");

                    for (int i = 0; i < next_leaf->size(); ++i) insert((*next_leaf)[i]);
                    next_leaf = next_leaf->next_leaf;
                    if (next_leaf->next_leaf != nullptr) next_leaf->next_leaf->before_leaf = this;

                    parent->erase(pos + 1);
                    return;
          }
          crash("Call merge in an invalid position."); 
}

template <typename T, typename V, uint m>
void Node<T, V, m>::change_key(const T& old_key, const T& new_key) noexcept {
          uint position = 0;
          while (position < size() && list[position] < old_key) position++;
          if (list[position] != old_key) crash("Can not find old key.");
          if (position == size() - 1 && parent != nullptr) parent->change_key(old_key, new_key);
          list[position] = new_key;
}
template <typename T, typename V, uint m>
void Node<T, V, m>::change_key(const int pos, const T& new_key, int) noexcept {
          if (pos < 0 || pos >= size()) crash("Key position out of bound.");
          if (pos - 1 >= 0) assert(list[pos - 1] < new_key);                    // Debug
          if (pos + 1 < size() - 1) assert(list[pos + 1] > new_key);                // Debug
          if (pos == size() - 1 && parent != nullptr) parent->change_key(list[pos], new_key);
          list[pos] = new_key;
}

template <typename T, typename V, uint m>
int32_t Node<T, V, m>::pfind(const T& key) const noexcept {
          if (size() == 0) return -1;
          
          int left = 0, right = list.size() - 1, mid;
          while (left <= right) {
                    mid = (right + left) / 2;
                    if (list[mid] == key) return mid;
                    if (list[mid] < key) left = mid + 1;
                    else right = mid - 1;
          }
          return left;
}

template <typename T, typename V, uint m>
void Node<T, V, m>::check() {
          if (_type == NodeType::LeafNode) assert(list.size() == value.size());
          else assert(list.size() == next.size());

          for (int i = 1; i < size(); ++i) {
                    std::cout << "assert: " << list[i] << " > " << list[i - 1] << "\n";
                    assert(list[i] > list[i - 1]);
          }

          if (_type == NodeType::InnerNode)
          for (int i = 0; i < next.size(); ++i) {
                    Node<T, V, m>* each = next[i].get();
                    assert(each->parent == this);
                    each->check();
          }
}

/**
 * B+ Tree Entity
 */

template <typename T, typename V, uint m>
BPTree<T, V, m>::BPTree(): root(Node<T, V, m>::create_node(NodeType::LeafNode)) { root->_is_root = true; }
template <typename T, typename V, uint m>
BPTree<T, V, m>::~BPTree() { }
template <typename T, typename V, uint m>
bool BPTree<T, V, m>::is_empty() const noexcept { return root == nullptr || root->size() == 0; }
template <typename T, typename V, uint m>
std::vector<V> BPTree<T, V, m>::serialize() const {
          auto cpy = root.get();
          while (cpy->_type == NodeType::InnerNode) cpy = cpy->next[0].get();
          std::vector<V> ret;
          while (cpy != nullptr) {
                    for (auto each : cpy->value)
                    ret.push_back(each);

                    cpy = cpy->next_leaf;
          }
          return std::move(ret);
}
template <typename T, typename V, uint m>
void BPTree<T, V, m>::insert(const T& key, const V& val) {
          Node<T, V, m>* cpy = root.get();
          while (cpy->_type != NodeType::LeafNode) {
                    uint pos = 0;
                    while (pos < cpy->size() && cpy->list[pos] < key) {
                              pos++;
                    }
                    cpy = cpy->next[pos < cpy->size() ? pos : pos - 1].get();
          }
          bpNode<T, V, m> ret = cpy->insert(key, val);
          if (ret != nullptr) {
                    bpNode<T, V, m> new_root = Node<T, V, m>::create_node(NodeType::InnerNode);
                    new_root->_is_root = true;
                    new_root->insert(root->get_key(), root);
                    new_root->insert(ret->get_key(), ret);
                    root = std::move(new_root);
          }
}
template <typename T, typename V, uint m>
const V* BPTree<T, V, m>::find(const T& key) const noexcept {
          Node<T, V, m>* cpy = root.get();
          if (key > cpy->list.back()) return nullptr;
          while (cpy->_type != NodeType::LeafNode) 
                    cpy = cpy->next[cpy->pfind(key)].get();
          int32_t pos = cpy->pfind(key);
          return cpy->list[pos] == key ? &(cpy->value[pos]) : nullptr;
}
template <typename T, typename V, uint m>
V& BPTree<T, V, m>::operator[](const T& key) const noexcept { return *(const_cast<V*>(find(key))); }
template <typename T, typename V, uint m>
bool BPTree<T, V, m>::erase(const T& key) {
          return false;
}
template <typename T, typename V, uint m>
bool BPTree<T, V, m>::tombstone(const T& key) {
          return false;
}
template <typename T, typename V, uint m>
bool BPTree<T, V, m>::exist(const T& key) { return find(key) != nullptr; }

/**
 * B+ Tree logic cheaking
 */

template <typename T, typename V, uint m>
void BPTree<T, V, m>::check_serialize(std::vector<V>& arr) { check_serialize_(arr, root); }
template <typename T, typename V, uint m>
void BPTree<T, V, m>::check_serialize_(std::vector<V>& arr, const bpNode<T, V, m>& node) {
          if (node->_type == NodeType::LeafNode) {
                    for (auto val : node->value) arr.push_back(val);
                    return;
          }
          for (int i = 0; i < node->list.size(); ++i) {
                    check_serialize_(arr, node->next[i]);
                    assert(node->next[i]->get_key() == node->list[i]);
          }
}

template <typename T, typename V, uint m>
void BPTree<T, V, m>::check() { 
          root->check(); 
          std::vector<V> arr; check_serialize(arr);
          std::unordered_map<V, size_t> ref;
          for (auto each : arr) ref[each] += 1;
          for (auto &&[k, v] : ref) if (v > 1) {
                    std::cout << "Error: 出现次数大于一次 => " << k << "\n";
                    exit(1);
          }
}

#endif