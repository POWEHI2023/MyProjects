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
Node<T, V, m>::Node(const Node* node): // list((T*)malloc(sizeof(T) * m)), value((T*)malloc(sizeof(V) * m)),
_type(node->_type), next_leaf(node->next_leaf), before_leaf(node->before_leaf), parent(node->parent) 
{ copy_list(*node); }

template <typename T, typename V, uint m>
Node<T, V, m>::Node(const Node& node): // list((T*)malloc(sizeof(T) * m)), value((T*)malloc(sizeof(V) * m)),
_type(node._type), next_leaf(node.next_leaf), before_leaf(node.before_leaf), parent(node.parent) 
{ copy_list(node); }

template <typename T, typename V, uint m>
Node<T, V, m>::Node(const Node&& node): 
_type(std::move(node._type)),
next_leaf(std::move(node.next_leaf)), before_leaf(std::move(node.before_leaf)), parent(std::move(node.parent))
{ copy_list(std::move(node)); }

template <typename T, typename V, uint m>
Node<T, V, m>::~Node() { 
          list.clear();
          value.clear();
          next.clear();

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
const bpNode<T, V, m> Node<T, V, m>::insert(const T& elem, const V& val) {
          if (crt_size == m) {
                    auto ret = split();
                    if (ret == nullptr) {
                              printf("Error: Split get a null pointer.\n");
                              exit(1);
                    }

                    if (parent != nullptr) {
                              uint pos = parent->find_key(ret->get_key());
                              parent->change_key(pos, get_key());

                              if (elem < this->get_key()) this->insert(elem, val);
                              else ret->insert(elem, val);

                              return std::move(parent->insert(ret->get_key(), ret));
                    } else {
                              if (elem < this->get_key()) this->insert(elem, val);
                              else ret->insert(elem, val);
                              return ret;
                    }
          }
          uint pst = 0;
          while (pst < crt_size && list[pst] < elem) pst++;
          list.insert(list.begin() + pst, elem);
          value.insert(value.begin() + pst, val);

          if (pst == crt_size && parent != nullptr)  {
                    uint pos = parent->find_key(list[pst - 1]);
                    parent->change_key(pos, elem);
          }

          crt_size++;

          return nullptr;
}

template <typename T, typename V, uint m>
const bpNode<T, V, m> Node<T, V, m>::insert(const T& elem, const bpNode<T, V, m>& node) {
          node->_is_root = false;
          node->parent = this;

          if (crt_size == m) {
                    auto ret = split();
                    if (ret == nullptr) {
                              printf("Error: Split get a null pointer.\n");
                              exit(1);
                    }
                    if (parent != nullptr) {
                              uint pos = parent->find_key(ret->get_key());
                              parent->change_key(pos, get_key());

                              if (elem < this->get_key()) this->insert(elem, node);
                              else ret->insert(elem, node);

                              return std::move(parent->insert(ret->get_key(), ret));
                    } else {
                              if (elem < this->get_key()) this->insert(elem, node);
                              else ret->insert(elem, node);
                              return ret;
                    }
          }
          // 没有满就直接插入
          uint pst = 0;
          while (pst < crt_size && list[pst] < elem) pst++;
          list.insert(list.begin() + pst, elem);
          next.insert(next.begin() + pst, node);

          if (pst == crt_size && parent != nullptr)  {
                    uint pos = parent->find_key(list[pst - 1]);
                    parent->change_key(pos, elem);
          }

          crt_size++;

          return nullptr;
}

template <typename T, typename V, uint m>
const bpNode<T, V, m> Node<T, V, m>::split() {
          // Node* new_node = const_cast<Node*>(create_node_(_type));
          bpNode<T, V, m> new_node = create_node(_type);

          for (int i = limit; i < crt_size; ++i) {
                    auto [k, v, t] = this->operator[](i);
                    if (t) new_node->insert(k, v._v);
                    else new_node->insert(k, v._n);
          }
          for (int i = limit; i < crt_size; ++i) {
                    list.pop_back();
                    if (_type == NodeType::LeafNode) value.pop_back();
                    else next.pop_back();
          }
          crt_size = limit;

          // new_node->parent = parent;
          new_node->next_leaf = next_leaf;
          if (next_leaf != nullptr) next_leaf->before_leaf = new_node.get();
          new_node->before_leaf = this;
          this->next_leaf = new_node.get();

          return std::move(new_node);
}

template <typename T, typename V, uint m>
size_t Node<T, V, m>::erase(const uint32_t index) {
          if (index < 0 || index >= crt_size) {
                    printf("Erase Error: Index(%d) out of bount(0 - %ld).\n", index, crt_size - 1);
                    exit(1);
          }
          if (crt_size > 0) {
                    if (index == crt_size - 1 && parent != nullptr)
                    parent->change_key(parent->find_key(get_key()), list[crt_size - 2]);
                    list.erase(list.begin() + index);
                    if (_type == NodeType::LeafNode) value.erase(value.begin() + index);
                    else next.erase(next.begin() + index);
                    crt_size--;

                    // 当包含的节点数量不够时，进行合并操作
                    if (crt_size < limit && !_is_root) {
                              // 如果非root，一定包含父节点
                              merge();
                    }
                    // 当为root节点时，可能删到只剩下1个节点，并且root是InnerNode类型，需要将仅剩的一个InnerNode作为root
          } else {
                    printf("Erase element in a empty node.\n");
                    exit(1);
          }

          return crt_size;
}

// 只有非root节点才会触发merge操作
template <typename T, typename V, uint m>
void Node<T, V, m>::merge() {
          // 当前距离limit缺少一个节点时就会触发
          if (before_leaf != nullptr && before_leaf->size() > limit) {
                    // 取出左边节点的最后一个值插入当前节点
                    auto [k, v, t] = before_leaf->operator[](before_leaf->size() - 1);
                    if (t) this->insert(k, v._v);
                    else this->insert(k, v._n);

                    before_leaf->erase(before_leaf->size() - 1);
                    return;
          } 
          if (next_leaf != nullptr && next_leaf->size() > limit) {
                    // 取出右边节点第一个值插入当前节点
                    auto [k, v, t] = next_leaf->operator[](0);
                    if (t) this->insert(k, v._v);
                    else this->insert(k, v._n);

                    next_leaf->erase(0);
                    return;
          }
          // 优先把左边的节点合并到一起, 向左合并
          if (before_leaf != nullptr) {
                    uint pos = parent->find_key(get_key());

                    for (int i = 0; i < crt_size; ++i) {
                              auto [k, v, t] = this->operator[](i);
                              if (t) before_leaf->insert(k, v._v);
                              else before_leaf->insert(k, v._n);
                    }
                    // 父节点中修改左节点的索引为当前节点的索引key，并在父节点中删除当前节点

                    before_leaf->next_leaf = next_leaf;
                    if (next_leaf != nullptr) next_leaf->before_leaf = before_leaf;

                    parent->erase(pos);

                    return;
          }
          if (next_leaf != nullptr) {
                    uint pos = parent->find_key(get_key());

                    for (int i = 0; i < next_leaf->crt_size; ++i) {
                              auto [k, v, t] = next_leaf->operator[](i);
                              if (t) this->insert(k, v._v);
                              else this->insert(k, v._n);
                    }

                    next_leaf->before_leaf = before_leaf;
                    if (before_leaf != nullptr) before_leaf->next_leaf = next_leaf;

                    parent->erase(pos + 1);

                    return;
          }
          // 没有节点可以被Merge
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