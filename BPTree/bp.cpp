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

template <typename T, uint m>
Node<T, m>::Node(const Node* node):
_type(node->_type), _is_root(node->_is_root), next_leaf(node->next_leaf), before_leaf(node->before_leaf), parent(node->parent) 
{ copy_list(*node); }

template <typename T, uint m>
Node<T, m>::Node(const Node& node):
_type(node._type), _is_root(node._is_root), next_leaf(node.next_leaf), before_leaf(node->before_leaf), parent(node.parent) 
{ copy_list(node); }

template <typename T, uint m>
Node<T, m>::Node(const Node&& node):
_type(std::move(node._type)), _is_root(std::move(node._is_root)),
next_leaf(std::move(node.next_leaf)), before_leaf(std::move(node->before_leaf)), parent(std::move(node.parent))
{ copy_list(std::move(node)); }

template <typename T, uint m>
Node<T, m>::~Node() { 
          free_list();
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
 * Function part
 */

template <typename T, uint m>
T& Node<T, m>::operator[](uint index) {
          if (index < 0 || index >= crt_size) {
                    printf("Outof bound of current node. operator[%d]\n", index);
                    exit(1);
          }

          return list[index];
}

template <typename T, uint m>
const Node<T, m>* Node<T, m>::insert(const T& elem) {
          if (crt_size == m) {
                    // 拆分，给后一个节点插入elem，给父节点插入新key和新list(一个Element)
                    auto [left, right] = split();
                    parend_element->key = *(static_cast<typename T::key_type*>(left->get_key()));

                    (static_cast<Node<T, m>*>(right))->insert(elem);
                    return parent->insert(make_inner_element(*(static_cast<typename T::key_type*>(right->get_key())), right));
          } else {
                    // 选择一个位置插入
                    int pst = 0;
                    for (; pst < crt_size && list[pst] < elem; ++pst) { }
                    uint ptr = crt_size;
                    while (ptr > pst) list[ptr] = std::move(list[ptr - 1]);
                    list[pst] = elem;
                    return nullptr;
          }
}

template <typename T, uint m>
uint32_t Node<T, m>::find_key(const T& elem) const {
          for (int i = 0; i < crt_size; ++i) {
                    if (list[i] == elem) return i;
          }
          return -1;
}

template <typename T, uint m>
uint32_t Node<T, m>::find_key(const T* elem) const { find_key(*elem); }

template <typename T, uint m>
const AbstractNode* Node<T, m>::find_neighbour(const uint32_t index) const {
          if (index < 0 || index >= crt_size) return nullptr;
          if (index - 1 >= 0 && list[index - 1].value.size() <= limit) {
                    return &list[index - 1];
          } else if (index + 1 < crt_size && list[index + 1].value.size() <= limit) {
                    return &list[index + 1];
          } else {
                    return nullptr;
          }
}

template <typename T, uint m>
const typename Node<T, m>::Deliver Node<T, m>::split() {

}

/*template <typename T, uint m>
void Node<T, m>::erase(const uint32_t index) {

}

template <typename T, uint m>
void Node<T, m>::_force_del(const uint32_t index) {

}*/
