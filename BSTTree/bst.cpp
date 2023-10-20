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
root(std::move(bst.root)), fn(std::move(bst.fn)),
head(std::move(bst.head)), tail(std::move(bst.tail)), modified(std::move(bst.modified))
{  }

template <typename T>
bool BSTTree<T>::operator==(const BSTTree& bst) 
{ return root == bst.root; }


/**
 * Functional part for BSTTree
 */

template <typename T>
const typename BSTTree<T>::iterator BSTTree<T>::find(const T& elem) {
          BSTNode<T>* node = root.get();
          while (node)
          if (fn(node->element, elem)) node = node->left != nullptr ? node->left.get() : nullptr;
          else if (node->element == elem) return std::move(iterator(node));
          else node = node->right != nullptr ? node->right.get() : nullptr;
          
          return std::move(end());
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
          std::vector<BSTNode<T>*> s1;
          s1.push_back(root.get());
          while (s1.size() != 0) {
                    std::vector<BSTNode<T>*> s2;
                    for (auto each : s1) {
                              arr.push_back(each->element);

                              if (each->left != nullptr)
                              s2.push_back(each->left.get());
                              if (each->right != nullptr)
                              s2.push_back(each->right.get());
                    }
                    s1 = std::move(s2);
          }

          // 用新的排序方式，重新插入节点
          root = nullptr;
          for (auto each : arr) this->insert(each);

          // 返回处理结果
          return true;
}


/**
 * Iterator implimentations
 */

// template <typename T>
// BSTTree<T>::iterator::iterator(const BSTNode<T> *crt, const BSTTree *from): 
// crt(const_cast<BSTNode<T>*>(crt)), from(const_cast<BSTTree*>(from)) { }

template <typename T>
BSTTree<T>::iterator::iterator(const BSTNode<T> *crt):
crt(const_cast<BSTNode<T>*>(crt)) { }

template <typename T>
BSTTree<T>::iterator::iterator(const typename BSTNode<T>::node& crt):
crt(crt.get()) { }

template <typename T>
BSTTree<T>::iterator::iterator(const iterator& iter):
crt(iter.crt), end(iter.end) { }

template <typename T>
BSTTree<T>::iterator::iterator(const iterator&& iter):
crt(std::move(iter.crt)), end(std::move(iter.end)) { }

template <typename T>
void BSTTree<T>::iterator::operator=(const iterator& iter) {
          this->crt = iter.crt;
          this->end = iter.end;
}

template <typename T>
void BSTTree<T>::iterator::operator=(const iterator&& iter) { 
          this->crt = std::move(iter.crt);
          this->end = std::move(iter.end);
}

template <typename T>
typename BSTTree<T>::iterator& BSTTree<T>::iterator::operator++() { 
          if (!end && !destroyed()) {
                    if (crt->next != nullptr) crt = crt->next.get();
                    else end = true;
          } else {
                    printf("operator++ can not be executed in this iterator which is destroyed or the end element.\n");
                    exit(1);
          }
          return *this;
}

template <typename T>
const typename BSTTree<T>::iterator BSTTree<T>::iterator::operator++(int) {
          iterator ret = iterator(*this);
          ++(*this);
          return std::move(ret);
}

template <typename T>
typename BSTTree<T>::iterator& BSTTree<T>::iterator::operator--() {
          if (destroyed()) {
                    printf("Current iterator point to element which has been destroyed when execure operator--.\n");
                    exit(1);
          }

          if (end) { end = false; }
          else {
                    if (crt->before != nullptr) crt = crt->before.get();
                    else {
                              printf("Current iterator point to the first element, can not execure any operator--.\n");
                              exit(1);
                    }
          }
          return *this;
}

template <typename T>
const typename BSTTree<T>::iterator BSTTree<T>::iterator::operator--(int) {
          iterator ret = iterator(*this);
          --(*this);
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


/**
 * Append functions
 */

template <typename T>
void BSTTree<T>::insert(const T& elem) {
          if (root == nullptr) {
                    root = BSTNode<T>::create_node(elem);
                    this->head = root.get();
                    this->tail = root.get();
                    return;
          }

          typename BSTNode<T>::node target = root;
          while (1)
          if (fn(target->element, elem)) {
                    // Judge left
                    if (target->left != nullptr) {
                              target = target->left;
                    }
                    else {
                              target->left = BSTNode<T>::create_node(elem);

                              // All four links
                              target->left->before = target->before;  
                              if (target->before != nullptr) target->before->next = target->left;
                              target->left->next = target;
                              target->before = target->left;

                              if (this->head == target.get()) {
                                        this->head = target->left.get();
                              }
                              break;
                    }
          } else {
                    // Judge right
                    if (target->right != nullptr) {
                              target = target->right;
                    }
                    else {
                              target->right = BSTNode<T>::create_node(elem);
                    
                              /**
                               * Target <-> TargetOriginNext
                               * =>
                               * Target <-> TargetRight <-> TargetOriginNext
                               */
                              target->right->next = target->next;
                              if (target->next != nullptr) target->next->before = target->right;
                              target->right->before = target;
                              target->next = target->right;

                              /**
                               * Origin last links: Target <-> nullptr (tail is Target),
                               * After insert links: Target <-> TargetRight <-> nullptr (tail is TargetRight)
                               */ 
                              if (this->tail == target.get()) {
                                        this->tail = target->right.get();
                              }
                              break;
                    }
          }
          // printf("\n%d, %d, %d\n", target->element, target->left != nullptr ? target->left->element : -1, target->right != nullptr ? target->right->element : -1);
}

template <typename T>
const typename BSTTree<T>::iterator BSTTree<T>::begin() 
{ return iterator(this->head); }

template <typename T>
const typename BSTTree<T>::iterator BSTTree<T>::end() {  
          iterator iter(this->tail);
          iter++;
          return std::move(iter);
}

template <typename T>
const std::vector<T> BSTTree<T>::to_array() {
          std::vector<T> ret;
          BSTNode<T> *start = this->head;
          while (start != nullptr)  {
                    ret.push_back(start->element);
                    if (start->next != nullptr) start = start->next.get();
                    else break;
          }
          return std::move(ret);
}