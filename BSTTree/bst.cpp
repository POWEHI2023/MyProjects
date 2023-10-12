#include "bst.h"
#include <iostream>

template <typename Type>
BSTNode<Type>::BSTNode(
          const Type& elem, 
          const BSTNode* left, 
          const BSTNode* right
): element(elem), left(const_cast<BSTNode<Type>*>(left)), right(const_cast<BSTNode<Type>*>(right)) 
{ std::cout << "Construct BSTNode element " << element << std::endl; }

template <typename Type>
BSTNode<Type>::BSTNode(const BSTNode* node):
element(node->element), left(node->left), right(node->right)
{ std::cout << "Copy from a pointer element " << element << std::endl; }

template <typename Type>
BSTNode<Type>::BSTNode(const BSTNode& node): 
element(node.element), left(node.left), right(node.right)
{ std::cout << "Copy from a reference element " << element << std::endl; }

template <typename Type>
BSTNode<Type>::BSTNode(const BSTNode&& node): 
element(std::move(node.element)), left(std::move(node.left)), right(std::move(node.right)) 
{ std::cout << "Copy from a rvalue element " << element << std::endl; }

template <typename Type>
void BSTNode<Type>::clear() { 
          left.reset();
          right.reset();

          left = nullptr;
          right = nullptr;
          before = next = nullptr; 
}

template <typename Type>
BSTNode<Type>* BSTNode<Type>::create_node(int, const Type& relem, const BSTNode* left, const BSTNode* right) 
{ return new BSTNode(relem, left, right); }

template <typename Type>
std::shared_ptr<BSTNode<Type>> BSTNode<Type>::create_node(const Type& relem, const BSTNode<Type>* left, const BSTNode<Type>* right) 
{ return std::shared_ptr<BSTNode<Type>>(BSTNode<Type>::create_node(0, relem, left, right), BSTNode<Type>::release_node); }

template <typename Type>
void BSTNode<Type>::release_node(const BSTNode* bst) 
{ delete bst; }

template <typename Type>
BSTNode<Type>::~BSTNode() {
          // if (left != nullptr) BSTNode::release_node(left);
          // if (right != nullptr) BSTNode::release_node(right);
          std::cout << "Release BSTNode element " << element << std::endl;
}



/**
 * BSTTree constructor depends on BSTNode
 */

template <typename T, BST::CMP cmp>
BSTTree<T, cmp>::BSTTree(): root(nullptr), fn(cmp == BST::DESC ? BST::greater<T> : BST::less<T>)
{ std::cout << "Construct BSTTree instance." << std::endl; }

template <typename T, BST::CMP cmp>
BSTTree<T, cmp>::~BSTTree() { 
          // if (root != nullptr) BSTNode<T>::release_node(root); 
          std::cout << "Release BSTTree instance." << std::endl;
}

template <typename T, BST::CMP cmp>
BSTTree<T, cmp>::BSTTree(const BSTTree& bst): root(bst.root), fn(bst.fn), 
head(bst.head), tail(bst.tail), modified(bst.modified)
{ std::cout << "Construct BSTTree from reference." << std::endl; }

template <typename T, BST::CMP cmp>
BSTTree<T, cmp>::BSTTree(const BSTTree&& bst): 
root(std::move(bst.root)), fn(bst.fn),
head(std::move(bst.head)), tail(std::move(bst.tail)), modified(bst.modified)
{ std::cout << "Construct BSTTree from rvalue." << std::endl; }

template <typename T, BST::CMP cmp>
bool BSTTree<T, cmp>::operator==(const BSTTree& bst) 
{ return root == bst.root; }

template <typename T>
inline bool node_exist(typename BSTNode<T>::node& node)
{ return node != nullptr && !node->destroyed; }

template <typename T, BST::CMP cmp>
bool BSTTree<T, cmp>::insert(const T& elem) {
          bool old_moditied = modified;
          modified = false;

          if (root == nullptr)  {
                    root = BSTNode<T>::create_node(elem);
                    return (modified = (root != nullptr));
          }
          else {
                    typename BSTNode<T>::node node = root;

                    BEGIN: 
                    if (fn(node->element, elem)) {
                              if (!node_exist<T>(node->left)) {
                                        if (node->left != nullptr) {
                                                  // typename BSTNode<T>::node temp = node->left;
                                                  node->left = nullptr;
                                                  // temp.reset();
                                        }

                                        node->left = BSTNode<T>::create_node(elem);
                                        // Links for fast iterator
                                        node->left->before = node->before;
                                        if (node->before != nullptr) 
                                        node->before->next = node->left.get();

                                        node->before = node->left.get();
                                        node->left->next = node.get();
                                        return (modified = (node_exist<T>(node->left)));
                              } else {
                                        node = node->left;
                                        goto BEGIN;
                              }
                    } else {
                              if (!node_exist<T>(node->right)) {
                                        if (node->right != nullptr) {
                                                  // typename BSTNode<T>::node temp = node->right;
                                                  node->right = nullptr;
                                                  // temp.reset();
                                        }
                                        
                                        node->right = BSTNode<T>::create_node(elem);
                                        // Links for fase iterator
                                        node->right->next = node->next;
                                        if (node->next != nullptr) 
                                        node->next->before = node->right.get();

                                        node->next = node->right.get();
                                        node->right->before = node.get();
                                        return (modified = (node_exist<T>(node->right)));
                              } else {
                                        node = node->right;
                                        goto BEGIN;
                              }
                    }
          }

          modified = old_moditied;
          return false;
}

template <typename T, BST::CMP cmp>
bool BSTTree<T, cmp>::insert(const T&& elem) 
{ return insert(elem); }

template <typename T, BST::CMP cmp>
bool BSTTree<T, cmp>::erase_root(typename BSTNode<T>::node& root) {
          typename BSTNode<T>::node node = root;
          if (node->left != nullptr) {
                    node = node->left;
                    while (node_exist<T>(node->right)) node = node->right;
                    if (node->right != nullptr) {
                              // typename BSTNode<T>::node temp = node->right;
                              node->right = nullptr;
                              // temp.reset();
                    }
                    // node->right = root->right;

                    // std::cout << "Right element " << node->element << std::endl;
                    // Update links
                    if (node_exist<T>(root->right)) {
                              node->right = root->right;

                              typename BSTNode<T>::node n = root->right;
                              while (node_exist<T>(n->left)) n = n->left;

                              // std::cout << "Left element " << n->element << std::endl;
                              // assert(node->next == n.get() && n->before == node.get());
                              node->next = n.get();
                              n->before = node.get();
                    } else {
                              if (root->right != nullptr) {
                                        // typename BSTNode<T>::node temp = node->right;
                                        root->right = nullptr;
                                        // temp.reset();
                              }
                              node->next = root->next;
                              if (root->next != nullptr) root->next->before = node.get();
                    }
                    
                    // node = root;
                    // printf("RESETED\n");
                    root = root->left;
                    // node.reset();
          }
          else {
                    if (!node_exist<T>(root->right)) {
                              if (root->before != nullptr) root->before->next = root->next;
                              if (root->next != nullptr) root->next->before = root->before;
                              root->destroyed = true;
                    } else {
                              // Update links
                              node = root->right;
                              while (node_exist<T>(node->left)) node = node->left;

                              assert(root->next == node.get() && node->before == root.get());

                              node->before = root->before;
                              if (root->before != nullptr) root->before->next = node.get();

                              // node = root;
                              root = root->right;
                              // node.reset();
                    }
          }

          return true;
}

template <typename T, BST::CMP cmp>
bool BSTTree<T, cmp>::erase(const iterator& iter) {
          bool old_modified = modified;
          modified = false;

          if (root.get() == iter.crt) erase_root(root);
          else {
                    typename BSTNode<T>::node node = root;
                    while (node_exist<T>(node)) {
                              if (fn(node->element, iter.crt->element)) 
                              if (node->left.get() == iter.crt) {
                                        modified = erase_root(node->left);
                                        break;
                              } else node = node->left;
                              else 
                              if (node->right.get() == iter.crt) {
                                        std::cout << "Find elem " << node->right->element << std::endl;

                                        modified = erase_root(node->right);
                                        break;
                              } else node = node->right;
                    } 
          }
          if (modified == true) {
                    iter.crt->clear();
                    // BSTNode<T>::release_node(iter.crt);
          }

          modified |= old_modified;
          return modified;
}

template <typename T, BST::CMP cmp>
BSTTree<T, cmp>::iterator::iterator(const BSTNode<T> *crt, const BSTTree *from): 
crt(const_cast<BSTNode<T>*>(crt)), from(const_cast<BSTTree*>(from)) 
{ }

template <typename T, BST::CMP cmp>
BSTTree<T, cmp>::iterator::iterator(const iterator& iter):
crt(iter.crt), from(iter.from) 
{ }

template <typename T, BST::CMP cmp>
BSTTree<T, cmp>::iterator::iterator(const iterator&& iter):
crt(std::move(iter.crt)), from(std::move(iter.from))
{ }

template <typename T, BST::CMP cmp>
typename BSTTree<T, cmp>::iterator& BSTTree<T, cmp>::iterator::operator++() { 
          if (crt != nullptr) crt = crt->next;
          return *this;
}

template <typename T, BST::CMP cmp>
const typename BSTTree<T, cmp>::iterator BSTTree<T, cmp>::iterator::operator++(int) {
          iterator ret = iterator(*this);
          if (crt != nullptr) crt = crt->next;
          return std::move(ret);
}

template <typename T, BST::CMP cmp>
typename BSTTree<T, cmp>::iterator& BSTTree<T, cmp>::iterator::operator--() {
          if (crt != nullptr) crt = crt->before;
          return *this;
}

template <typename T, BST::CMP cmp>
const typename BSTTree<T, cmp>::iterator BSTTree<T, cmp>::iterator::operator--(int) {
          iterator ret = iterator(*this);
          if (crt != nullptr) crt = crt->before;
          return std::move(ret);
}

template <typename T, BST::CMP cmp>
const typename BSTTree<T, cmp>::iterator BSTTree<T, cmp>::iterator::operator+(uint32_t steps) {
          iterator ret(*this);
          ret += steps;
          return std::move(ret);
}

template <typename T, BST::CMP cmp>
const typename BSTTree<T, cmp>::iterator BSTTree<T, cmp>::iterator::operator-(uint32_t steps) {
          iterator ret(*this);
          ret -= steps;
          return std::move(ret);
}

template <typename T, BST::CMP cmp>
void BSTTree<T, cmp>::iterator::operator+=(uint32_t steps) 
{ while (steps--) (*this)++; }

template <typename T, BST::CMP cmp>
void BSTTree<T, cmp>::iterator::operator-=(uint32_t steps) 
{ while (steps--) (*this)--; }

template <typename T, BST::CMP cmp>
BSTNode<T>* BSTTree<T, cmp>::iterator::operator->() const 
{ return crt; }

template <typename T, BST::CMP cmp>
const typename BSTTree<T, cmp>::iterator BSTTree<T, cmp>::find(const T& elem) {
          typename BSTNode<T>::node node = root;
          while (node)
          if (fn(node->element, elem)) node = node->left;
          else if (node->element == elem) return iterator(node.get(), this);
          else node = node->right;
          
          return iterator(nullptr, nullptr);
}

template <typename T, BST::CMP cmp>
const typename BSTTree<T, cmp>::iterator BSTTree<T, cmp>::begin() {
          if (modified == false) return std::move(iterator(head, this));

          typename BSTNode<T>::node node = root;
          while (node->left != nullptr && !node->left->destroyed) node = node->left;
          head = node.get();

          node = root;
          while (node->right != nullptr && !node->right->destroyed) node = node->right;
          tail = node.get();

          modified = false;
          return std::move(iterator(head, const_cast<const BSTTree<T, cmp>*>(this)));
}

template <typename T, BST::CMP cmp>
const typename BSTTree<T, cmp>::iterator BSTTree<T, cmp>::end() {
          if (modified == false) return std::move(iterator(tail, this));

          typename BSTNode<T>::node node = root;
          while (node->right != nullptr && !node->right->destroyed) node = node->right;
          tail = node.get();

          node = root;
          while (node->left != nullptr && !node->left->destroyed) node = node->left;
          head = node.get();

          modified = false;
          return std::move(iterator(tail, const_cast<const BSTTree<T, cmp>*>(this)));
}

template <typename T, BST::CMP cmp>
const std::vector<T> BSTTree<T, cmp>::to_array() {
          std::vector<T> ret;
          typename BSTTree<T, cmp>::iterator iter = begin();
          while (iter != end()) {
                    ret.push_back(iter->element);
                    iter++;
          } ret.push_back(iter->element);
          return std::move(ret);
}

template <typename T, BST::CMP cmp>
const std::vector<T> BSTTree<T, cmp>::serilize() {
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

template <typename T, BST::CMP cmp>
bool BSTTree<T, cmp>::customize(std::function<bool(const T&, const T&)> f) { 
          this->fn = f; 
          if (root == nullptr) return true;

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

          // root.reset();
          root = nullptr;

          bool ret = true;
          for (auto each : arr) {
                    ret &= this->insert(each);
                    if (ret == false) return false;
          }

          return std::move(ret);
}
