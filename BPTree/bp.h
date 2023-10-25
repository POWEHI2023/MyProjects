#include <iostream>
#include <stdlib.h>
#include <memory.h>



template <typename T, typename V, uint m = 5>
class Node;
template <typename T, typename V, uint m = 5>
class BPTree;

using uint = uint32_t;
template <typename T, typename V, uint m>
using bpNode = std::shared_ptr<Node<T, V, m>>;

enum class NodeType: uint8_t { LeafNode, InnerNode };

template <typename T, typename V, uint m>
class Node {
public:
          typedef struct { const Node *left, *right; } Deliver;

          /**
           * 作为一个节点的基本信息，链接信息以及类型信息
           */

          Node *next_leaf = nullptr, *before_leaf = nullptr, *parent = nullptr;
          NodeType _type { NodeType::LeafNode };
          bool _is_root { false };


          /**
           * 类型的基础功能性函数
           */

          Node(): _type(NodeType::LeafNode) {
                    if (m <= 0) exit(1); 
                    list.reserve(m);
                    if (_type == NodeType::LeafNode) {
                              value.reserve(m);
                    } else {
                              next.reserve(m);
                    }
          }
          explicit Node(NodeType new_type): _type(new_type) { 
                    if (m <= 0) exit(1); 
                    list.reserve(m);
                    if (_type == NodeType::LeafNode) {
                              value.reserve(m);
                    } else {
                              next.reserve(m);
                    }
          }
          ~Node();
          explicit Node(const Node*);
          explicit Node(const Node&);
          explicit Node(const Node&&);
          void operator=(const Node&) = delete;
          void operator=(const Node&&) = delete;

          struct RET { 
                    struct two4one { V _v; bpNode<T, V, m> _n; } y;
                    T x; bool _leaf;
          };
          const RET operator[](uint index) const {
                    if (index < 0 || index >= size()) {
                              printf("Outof bound of current node. operator[%d]\n", index);
                              exit(1);
                    }
                    if (_type == NodeType::LeafNode) return { list[index], value[index], true };
                    else return { list[index], next[index], false };
          }

          bool operator==(const Node& node) noexcept { return comp(node, [](const T& _x, const T& _y){ return _x == _y; }); }
          bool operator!=(const Node& node) { return !operator==(node); }
          bool operator>=(const Node& node) noexcept { return comp(node, [](const T& _x, const T& _y){ return _x >= _y; }); }
          bool operator<=(const Node& node) noexcept { return comp(node, [](const T& _x, const T& _y){ return _x <= _y; }); }
          bool operator>(const Node& node) noexcept { return comp(node, [](const T& _x, const T& _y){ return _x > _y; }); }
          bool operator<(const Node& node) noexcept { return comp(node, [](const T& _x, const T& _y){ return _x < _y; }); }

          /**
           * 一些主要的功能性函数
           */

          const bpNode<T, V, m> insert(const T& elem, const V& val) noexcept;
          const bpNode<T, V, m> insert(const T& elem, const bpNode<T, V, m>& node) noexcept;
          template <typename... _Args>
          const bpNode<T, V, m> insert(const T& elem , const V& val, _Args&&... _args) noexcept {
                    insert(elem, val);
                    return insert(_args...);
          }
          const bpNode<T, V, m> insert(const RET& ret) noexcept;
          void erase(const uint32_t index) noexcept;
          const bpNode<T, V, m> split() noexcept;
          void merge () noexcept;

          /**
           * 一些简单的功能性函数，头文件中被实现
           */

          inline constexpr int find_key(const T& elem) const noexcept { for (int i = 0; i < size(); ++i) if (list[i] == elem) return i; return -1; }
          inline constexpr const T get_key() const noexcept { return list[size() - 1]; }
          inline constexpr size_t size() const noexcept { return list.size(); }
          inline constexpr uint capacity() const noexcept { return m; }
private:
          
          /**
           * 作为节点的内容信息
           */

          std::vector<T> list;                    // 对应存储的Key信息
          std::vector<V> value;                   // 节点对应的值信息，被Key索引到的Value，当节点类型是LeafNode时被使用
          std::vector<bpNode<T, V, m>> next;      // 下层节点的信息，当节点类型为InnerNode时被使用

          uint32_t limit = (m + 1) / 2;           // limit为一个非root节点存储的最少内容
                                        // 当内容少于limit时需要合并节点，当内容多出 m 时需要分割节点

          /**
           * 内部使用的功能性函数
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

          /**
           * 重构部分，重构中......
           */

          template <typename ValueType>
          const bpNode<T, V, m> insert_(const T& key, ValueType&& value, bool type) noexcept;
          void change_key(const T& old_key, const T& new_key) noexcept;
          void change_key(const uint pos, const T& new_key) noexcept;
public:
          friend class BPTree<T, V, m>;

          /**
           * 一致性的静态构造函数，yong
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
};

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

          /**
           * 如果存在则设置，如果不存在则返回false
           */
          bool tombstone(const T& key);
          /**
           * 如果存在则删除，如果不存在则返回false
           */
          bool erase(const T& key);
          bool exist(const T& key);

          std::vector<V> serialize() const;
private:
          bpNode<T, V, m> root;
public:

};
