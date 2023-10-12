#include "bst.h"
#include <iostream>

/**
 * BSTTree constructor depends on BSTNode
 */

template <typename T>
BSTTree<T>::BSTTree(std::function<bool(const T&, const T&)> fn): root(nullptr), fn(fn)
{  }

template <typename T>
BSTTree<T>::~BSTTree() { 
          root = nullptr; 
          head = tail = nullptr;
          modified = false;
}

template <typename T>
BSTTree<T>::BSTTree(const BSTTree& bst): root(bst.root), fn(bst.fn), 
head(bst.head), tail(bst.tail), modified(bst.modified)
{  }

template <typename T>
BSTTree<T>::BSTTree(const BSTTree&& bst): 
root(std::move(bst.root)), fn(bst.fn),
head(std::move(bst.head)), tail(std::move(bst.tail)), modified(bst.modified)
{  }

template <typename T>
bool BSTTree<T>::operator==(const BSTTree& bst) 
{ return root == bst.root; }


/**
 * Functional part for BSTTree
 */

template <typename T>
bool BSTTree<T>::insert(const T&& elem) 
{ return insert(elem); }

template <typename T>
const typename BSTTree<T>::iterator BSTTree<T>::find(const T& elem) {
          typename BSTNode<T>::node node = root;
          while (node)
          if (fn(node->element, elem)) node = node->left;
          else if (node->element == elem) return iterator(node.get(), this);
          else node = node->right;
          
          return iterator(nullptr, nullptr);
}

template <typename T>
const std::vector<T> BSTTree<T>::serilize() {
          std::vector<T> ret;
          std::stack<typename BSTNode<T>::node> s;
          s.push(root);
          while (s.top()->left != nullptr && !s.top()->left->destroyed) {
                    s.push(s.top()->left);
          }
          while (s.size()) {
                    BSTNode<int>::node n = s.top();
                    s.pop();
                    ret.push_back(n->element);
                    if (n->right != nullptr && !n->right->destroyed) {
                              s.push(n->right);
                              while (s.top()->left != nullptr && !s.top()->left->destroyed) s.push(s.top()->left);
                    }
          }
          return std::move(ret);
}

template <typename T>
bool BSTTree<T>::customize(std::function<bool(const T&, const T&)> f) { 
          this->fn = f; 
          if (root == nullptr || (root->left == nullptr && root->right == nullptr)) return true;

          // 对已有节点进行重构
          std::vector<T> arr;
          std::vector<typename BSTNode<T>::node> s1;
          s1.push_back(root);
          while (s1.size() != 0) {
                    std::vector<typename BSTNode<T>::node> s2;
                    for (auto each : s1) {
                              arr.push_back(each->element);

                              if (each->left != nullptr && !each->left->destroyed)
                              s2.push_back(each->left);
                              if (each->right != nullptr && !each->right->destroyed)
                              s2.push_back(each->right);
                    }
                    s1 = std::move(s2);
          }

          // 用新的排序方式，重新插入节点
          root = nullptr;
          bool ret = true;
          for (auto each : arr) {
                    ret &= this->insert(each);
                    if (ret == false) return false;
          }

          // 返回处理结果
          return std::move(ret);
}


/**
 * Iterator implimentations
 */
template <typename T>
BSTTree<T>::iterator::iterator(const BSTNode<T> *crt, const BSTTree *from): 
crt(const_cast<BSTNode<T>*>(crt)), from(const_cast<BSTTree*>(from)) { }

template <typename T>
BSTTree<T>::iterator::iterator(const iterator& iter):
crt(iter.crt), from(iter.from) { }

template <typename T>
BSTTree<T>::iterator::iterator(const iterator&& iter):
crt(std::move(iter.crt)), from(std::move(iter.from)) { }

template <typename T>
typename BSTTree<T>::iterator& BSTTree<T>::iterator::operator++() { 
          if (crt != nullptr) crt = crt->next;
          return *this;
}

template <typename T>
const typename BSTTree<T>::iterator BSTTree<T>::iterator::operator++(int) {
          iterator ret = iterator(*this);
          if (crt != nullptr) crt = crt->next;
          return std::move(ret);
}

template <typename T>
typename BSTTree<T>::iterator& BSTTree<T>::iterator::operator--() {
          if (crt != nullptr) crt = crt->before;
          return *this;
}

template <typename T>
const typename BSTTree<T>::iterator BSTTree<T>::iterator::operator--(int) {
          iterator ret = iterator(*this);
          if (crt != nullptr) crt = crt->before;
          return std::move(ret);
}

template <typename T>
const typename BSTTree<T>::iterator BSTTree<T>::iterator::operator+(uint32_t steps) {
          iterator ret(*this);
          ret += steps;
          return std::move(ret);
}

template <typename T>
const typename BSTTree<T>::iterator BSTTree<T>::iterator::operator-(uint32_t steps) {
          iterator ret(*this);
          ret -= steps;
          return std::move(ret);
}

template <typename T>
void BSTTree<T>::iterator::operator+=(uint32_t steps) { while (steps--) (*this)++; }

template <typename T>
void BSTTree<T>::iterator::operator-=(uint32_t steps) { while (steps--) (*this)--; }

template <typename T>
BSTNode<T>* BSTTree<T>::iterator::operator->() const { return crt; }


