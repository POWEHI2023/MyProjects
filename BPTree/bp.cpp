#include "bp.h"
#include "../locker.h"

/**
 * 拷贝构造函数的实现，赋值构造函数被deleted
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
 * 基本功能的实现
 *        insert / erase
 *        split / merge
 */

template <typename T, typename V, uint m>
const bpNode<T, V, m> Node<T, V, m>::insert(const T& elem, const V& val) noexcept {
          if (size() == m) {  // 因为当前节点已经满了，所以需要首先分割节点，触发split操作
                    auto ret = split();                                         // split会将当前节点分成两份，返回一个bpNode代表分离后的右半部分
                    if (ret == nullptr) crash("Split get an empty node.");      // 分割后没有得到另一半节点，触发错误
                    if (elem < get_key()) {
                              insert(elem, val);
                              if (ret->size() < limit) {
                                        int pos = -1;                 // 由于移动了部分节点填充ret，可能导致Key改变
                                        if (parent != nullptr) pos = parent->find_key(get_key());
                                        
                                        ret->list.push_back(list.back()); list.pop_back();
                                        if (_type == NodeType::LeafNode) {
                                                  ret->value.push_back(value.back()); value.pop_back();
                                        } else { ret->next.push_back(next.back()); next.pop_back(); }
                                        if (pos != -1) parent->change_key(pos, get_key(), 0);
                              }
                    }
                    else ret->insert(elem, val);
                    return parent != nullptr ? parent->insert(ret->get_key(), ret) : ret;
          }
          // size < m 有剩余空间
          uint position = 0;
          while (position < size() && list[position] < elem) position++;
          list.insert(list.begin() + position, elem);
          value.insert(value.begin() + position, val);
          // 当改变的是最后一个元素，需要修改上层的Key为新Key
          if (position == size() - 1 && parent != nullptr) parent->change_key(list[size() - 2], list[size() - 1]);
          return nullptr;
}
template <typename T, typename V, uint m>
const bpNode<T, V, m> Node<T, V, m>::insert(const T& elem, const bpNode<T, V, m>& node) noexcept {
          node->_is_root = false; node->parent = this;

          if (size() == m) {  // 因为当前节点已经满了，所以需要首先分割节点，触发split操作
                    auto ret = split();                                         // split会将当前节点分成两份，返回一个bpNode代表分离后的右半部分
                    if (ret == nullptr) crash("Split get an empty node.");      // 分割后没有得到另一半节点，触发错误
                    if (elem < get_key()) {
                              insert(elem, node);
                              if (ret->size() < limit) {
                                        int pos = -1;                 // 由于移动了部分节点填充ret，可能导致Key改变
                                        if (parent != nullptr) pos = parent->find_key(get_key());
                                        ret->list.push_back(list.back()); list.pop_back();
                                        if (_type == NodeType::LeafNode) {
                                                  ret->value.push_back(value.back()); value.pop_back();
                                        } else { ret->next.push_back(next.back()); next.pop_back(); }
                                        if (pos != -1) parent->change_key(pos, get_key(), 0);
                              }
                    }
                    else ret->insert(elem, node);
                    return parent != nullptr ? parent->insert(ret->get_key(), ret) : ret;
          }
          // size < m 有剩余空间
          uint position = 0;
          while (position < size() && list[position] < elem) position++;
          list.insert(list.begin() + position, elem);
          next.insert(next.begin() + position, node);
          // 当改变的是最后一个元素，需要修改上层的Key为新Key
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
const bpNode<T, V, m> Node<T, V, m>::split() noexcept {                         // 分割后需要修改父节点中的Key
          auto node = Node::create_node(_type);   // 相同类型的节点

          int pos = -1;
          if (parent != nullptr) pos = parent->find_key(get_key());
          // list移动到新节点
          moveV2V(list, node->list, limit);
          if (_type == NodeType::LeafNode) moveV2V(value, node->value, limit);
          else moveV2V(next, node->next, limit);

          node->next_leaf = next_leaf;
          if (next_leaf != nullptr) next_leaf->before_leaf = node.get();
          next_leaf = node.get();
          node->before_leaf = this;

          if (pos != -1) parent->change_key(pos, get_key(), 0);

          return node;
}

template <typename T, typename V, uint m>
void Node<T, V, m>::merge() noexcept {            // merge期间会对涉及到的节点上锁，不会触发死锁是因为Merge应该找到可以合并的节点，不会触发split
          if (before_leaf != nullptr && before_leaf->size() > limit) {          // 取左节点的一个值补充至limit
                    insert((*before_leaf)[before_leaf->size() - 1]);
                    before_leaf->erase(before_leaf->size() - 1);
                    return;
          } 
          if (next_leaf != nullptr && next_leaf->size() > limit) {              // 取右节点的一个值补充至limit
                    insert((*next_leaf)[0]);
                    next_leaf->erase(0);
                    return;
          }
          if (before_leaf != nullptr) {                                         // 与左节点合并，父节点中删除当前节点
                    int pos = parent->find_key(before_leaf->get_key());
                    if (pos == -1) crash("Can not find key in parent list.");

                    for (int i = 0; i < size(); ++i) before_leaf->insert((*this)[i]);
                    before_leaf->next_leaf = next_leaf;
                    if (next_leaf != nullptr) next_leaf->before_leaf = before_leaf;

                    parent->erase(pos + 1);
                    return;
          }
          if (next_leaf != nullptr) {                                           // 与右节点合并，父节点中删除右节点
                    int pos = parent->find_key(get_key());
                    if (pos == -1) crash("Can not find key in parent list.");

                    for (int i = 0; i < next_leaf->size(); ++i) insert((*next_leaf)[i]);
                    next_leaf = next_leaf->next_leaf;
                    if (next_leaf->next_leaf != nullptr) next_leaf->next_leaf->before_leaf = this;

                    parent->erase(pos + 1);
                    return;
          }
          crash("Call merge in an invalid position.");                          // 非root节点缺少值时无法合并，触发错误
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