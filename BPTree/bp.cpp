#include "bp.h"

/**
 * For Element
 */

template <typename KeyType, typename ValueType>
Element<KeyType, ValueType>::Element(KeyType&& key, ValueType&& value, bool is_leaf):
key(key), value(value), is_leaf(is_leaf) { }

template <typename KeyType, typename ValueType>
Element<KeyType, ValueType>::Element(const Element* e):
key(e->key), value(e->value), is_leaf(e->is_leaf), tombstone(e->tombstone) { }

template <typename KeyType, typename ValueType>
Element<KeyType, ValueType>::Element(const Element& e):
key(e.key), value(e.value), is_leaf(e.is_leaf), tombstone(e.tombstone) { }

template <typename KeyType, typename ValueType>
Element<KeyType, ValueType>::Element(const Element&& e):
key(std::move(e.key)), value(std::move(e.value)), is_leaf(std::move(e.is_leaf)), tombstone(std::move(e.tombstone)) { }

template <typename KeyType, typename ValueType>
void Element<KeyType, ValueType>::operator=(const Element& elem) {
          this->key = elem.key;
          this->value = elem.value;
          this->is_leaf = elem.is_leaf;
          this->tombstone = elem.tombstone;
}

template <typename KeyType, typename ValueType>
void Element<KeyType, ValueType>::operator=(const Element&& elem) {
          this->key = std::move(elem.key);
          this->value = std::move(elem.value);
          this->is_leaf = std::move(elem.is_leaf);
          this->tombstone = std::move(elem.tombstone);
}

/**
 * For Node
 */

template <typename T, typename V, uint m>
Node<T, V, m>::Node(const Node* node): list((T*)malloc(sizeof(T) * m)), value((T*)malloc(sizeof(V) * m)),
_type(node->_type), next_leaf(node->next_leaf), before_leaf(node->before_leaf), parent(node->parent) 
{ copy_list(*node); }

template <typename T, typename V, uint m>
Node<T, V, m>::Node(const Node& node): list((T*)malloc(sizeof(T) * m)), value((T*)malloc(sizeof(V) * m)),
_type(node._type), next_leaf(node.next_leaf), before_leaf(node.before_leaf), parent(node.parent) 
{ copy_list(node); }

template <typename T, typename V, uint m>
Node<T, V, m>::Node(const Node&& node): 
_type(std::move(node._type)),
next_leaf(std::move(node.next_leaf)), before_leaf(std::move(node.before_leaf)), parent(std::move(node.parent))
{ copy_list(std::move(node)); }

template <typename T, typename V, uint m>
Node<T, V, m>::~Node() { 
          if (list != nullptr) free(list);
          if (value != nullptr) free(value);

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
 * Basic tool functions part
 */

template <typename T, typename V, uint m>
decltype(auto) Node<T, V, m>::operator[](uint index) const {
          if (index < 0 || index >= crt_size) {
                    printf("Outof bound of current node. operator[%d]\n", index);
                    exit(1);
          }

          return std::pair{ list[index], value[index] };
}

template <typename T, typename V, uint m>
uint Node<T, V, m>::find_key(const T& elem) const noexcept {
          for (int i = 0; i < crt_size; ++i) {
                    if (list[i] == elem) return i;
          }
          return -1;
}

/**
 * Basic functions part
 */

template <typename T, typename V, uint m>
const void Node<T, V, m>::insert(const T& elem, const V& val) {
          if (crt_size == m) {
                    printf("Insert Error: Full size!.\n");
                    exit(1);
          }

          uint pst = 0, ptr = crt_size;
          while (pst < crt_size && list[pst] < elem) pst++;
          while (ptr > pst) {
                    list[ptr] = std::move(list[ptr - 1]);
                    value[ptr] = std::move(value[ptr - 1]);
                    ptr--;
          }
          list[pst] = elem;
          value[pst] = val;

          crt_size++;
}

template <typename T, typename V, uint m>
const typename Node<T, V, m>::Deliver Node<T, V, m>::split() {
          Node* new_node = const_cast<Node*>(create_node_(_type));
          for (int i = limit; i < crt_size; ++i)
          new_node->insert(list[i], value[i]);

          return {this, std::move(new_node)};
}

template <typename T, typename V, uint m>
size_t Node<T, V, m>::erase(const uint32_t index) {
          if (index < 0 || index >= crt_size) {
                    printf("Erase Error: Index(%d) out of bount(0 - %ld).\n", index, crt_size - 1);
                    exit(1);
          }
          if (crt_size > 0) {
                    uint ptr = index;
                    for (; ptr < crt_size - 1; ++ptr) 
                    list[ptr] = list[ptr + 1];
          } else {
                    printf("Erase element in a empty node.\n");
                    exit(1);
          }

          return --crt_size;
}

template <typename T, typename V, uint m>
const typename Node<T, V, m>::Deliver Node<T, V, m>::merge() {
          // 当前距离limit缺少一个节点时就会触发
          if (before_leaf != nullptr && before_leaf->size() > limit) {
                    // 取出左边节点的最后一个值插入当前节点
                    auto [k, v] = before_leaf->operator[](before_leaf->size() - 1);
                    this->insert(k, v);
                    before_leaf->erase(before_leaf->size() - 1);
                    return { before_leaf, this };
          } 
          if (next_leaf != nullptr && next_leaf->size() > limit) {
                    // 取出右边节点第一个值插入当前节点
                    auto [k, v] = next_leaf->operator[](0);
                    this->insert(k, v);
                    next_leaf->erase(0);
                    return { this, next_leaf };
          }
          // 优先把左边的节点合并到一起, 向左合并
          if (before_leaf != nullptr) {
                    for (int i = 0; i < crt_size; ++i) {
                              before_leaf->insert(list[i], value[i]);
                    }
                    return { before_leaf, nullptr };
          }
          if (next_leaf != nullptr) {
                    for (int i = 0; i < next_leaf->crt_size; ++i) {
                              auto [k, v] = next_leaf->operator[](i);
                              this->insert(k, v);
                    }
                    return { this, nullptr };
          }
          // 没有节点可以被Merge
          printf("Call merge in an invalid position.\n");
          exit(1);
}

