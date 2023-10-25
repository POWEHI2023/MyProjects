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
 * Node Constructors
 */

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
 * Basic functions part
 */

template <typename T, typename V, uint m>
const bpNode<T, V, m> Node<T, V, m>::insert(const T& elem, const V& val) noexcept {

          return nullptr;
}

template <typename T, typename V, uint m>
const bpNode<T, V, m> Node<T, V, m>::insert(const T& elem, const bpNode<T, V, m>& node) noexcept {
          return nullptr;
}

template <typename T, typename V, uint m>
void Node<T, V, m>::erase(const uint index) noexcept {
          if (index < 0 || index >= size()) {
                    printf("Erase Error: Index(%d) out of bount(0 - %ld).\n", index, size() - 1);
                    exit(1);
          }
          if (size() > 0) {
                    if (index == size() - 1 && parent != nullptr) 
                    parent->change_key(get_key(), list[size() - 2]);

                    list.erase(list.begin() + index);
                    if (_type == NodeType::LeafNode) value.erase(value.begin() + index);
                    else next.erase(next.begin() + index);

                    if (size() < limit && !_is_root) merge();
          } else {
                    printf("Erase element in a empty node.\n");
                    exit(1);
          }
}

template <typename T, typename V, uint m>
const bpNode<T, V, m> Node<T, V, m>::split() noexcept {
          return nullptr;
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
                    uint pos = parent->find_key(before_leaf->get_key());
                    parent->change_key(pos, get_key());
                    for (int i = 0; i < size(); ++i) before_leaf->insert((*this)[i]);
                    before_leaf->next_leaf = next_leaf;
                    if (next_leaf != nullptr) next_leaf->before_leaf = before_leaf;
                    parent->erase(pos + 1);
                    return;
          }
          if (next_leaf != nullptr) {
                    uint pos = parent->find_key(get_key());
                    parent->change_key(pos, next_leaf->get_key());
                    for (int i = 0; i < next_leaf->size(); ++i) insert((*next_leaf)[i]);
                    next_leaf = next_leaf->next_leaf;
                    if (next_leaf->next_leaf != nullptr) next_leaf->next_leaf->before_leaf = this;
                    parent->erase(pos + 1);
                    return;
          }
          printf("Call merge in an invalid position.\n");
          exit(1);
}


/**
 * B+ Tree Entity
 */

template <typename T, typename V, uint m>
BPTree<T, V, m>::BPTree(): root(Node<T, V, m>::create_node(NodeType::LeafNode))
{ root->_is_root = true; }

template <typename T, typename V, uint m>
BPTree<T, V, m>::~BPTree() { }

template <typename T, typename V, uint m>
bool BPTree<T, V, m>::is_empty() const noexcept 
{ return root == nullptr || root->size() == 0; }

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
          bpNode<T, V, m> ret = nullptr;
          {
                    Node<T, V, m>* cpy = root.get();
                    while (cpy->_type != NodeType::LeafNode) {
                              uint pos = 0;
                              while (pos < cpy->size() && cpy->list[pos] < key) {
                                        pos++;
                              }
                              cpy = cpy->next[pos < cpy->size() ? pos : pos - 1].get();
                    }

                    // cpy is a LeafNode
                    ret = cpy->insert(key, val);
          }

          if (ret != nullptr) {
                    bpNode<T, V, m> new_root = Node<T, V, m>::create_node(NodeType::InnerNode);
                    new_root->_is_root = true;
                    new_root->insert(root->get_key(), root);

                    new_root->insert(ret->get_key(), ret);

                    root = std::move(new_root);
          }
}

template <typename T, typename V, uint m>
bool BPTree<T, V, m>::erase(const T& key) {
          return false;
}

template <typename T, typename V, uint m>
bool BPTree<T, V, m>::tombstone(const T& key) {
          return false;
}

template <typename T, typename V, uint m>
bool BPTree<T, V, m>::exist(const T& key) {
          return false;
}