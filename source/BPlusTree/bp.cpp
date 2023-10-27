#include "bp.h"

#define BPT
#ifndef BPT_

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
 * Basic function implimentation
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