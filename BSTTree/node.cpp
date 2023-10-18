#include "node.h"

template <typename T>
inline bool node_exist(typename BSTNode<T>::node& node)
{ return node != nullptr && !node->destroyed; }

template <typename Type>
BSTNode<Type>::BSTNode(
          const Type& elem, 
          const BSTNode* left, 
          const BSTNode* right
): element(elem), left(const_cast<BSTNode<Type>*>(left)), right(const_cast<BSTNode<Type>*>(right)) 
{ }

template <typename Type>
BSTNode<Type>::BSTNode(const BSTNode* node):
element(node->element), left(node->left), right(node->right)
{ }

template <typename Type>
BSTNode<Type>::BSTNode(const BSTNode& node): 
element(node.element), left(node.left), right(node.right)
{ }

template <typename Type>
BSTNode<Type>::BSTNode(const BSTNode&& node): 
element(std::move(node.element)), left(std::move(node.left)), right(std::move(node.right)) 
{ }

template <typename Type>
void BSTNode<Type>::clear() { 
          left = nullptr;
          right = nullptr;
          before = nullptr; 
          next = nullptr;
}

template <typename Type>
BSTNode<Type>* BSTNode<Type>::create_node_(const Type& relem, const BSTNode* left, const BSTNode* right) 
{ return new BSTNode(relem, left, right); }

template <typename Type>
const std::shared_ptr<BSTNode<Type>> BSTNode<Type>::create_node(const Type& relem, const BSTNode<Type>* left, const BSTNode<Type>* right) 
{ return std::shared_ptr<BSTNode<Type>>(BSTNode<Type>::create_node_(relem, left, right), BSTNode<Type>::release_node); }

template <typename Type>
void BSTNode<Type>::release_node(const BSTNode* bst) 
{ delete bst; }

template <typename Type>
BSTNode<Type>::~BSTNode() 
{ clear(); }