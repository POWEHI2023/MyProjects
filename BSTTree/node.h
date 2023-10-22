#include <iostream>

template <typename T>
class BSTNode;

template <typename T>
inline bool node_exist(typename BSTNode<T>::node& node);

template <typename T>
inline bool operator<(const typename BSTNode<T>::node left, const typename BSTNode<T>::node right) 
{ return left->element < right->element; }
template <typename T>
inline bool operator<=(const typename BSTNode<T>::node left, const typename BSTNode<T>::node right) 
{ return left->element <= right->element; }
template <typename T>
inline bool operator>(const typename BSTNode<T>::node left, const typename BSTNode<T>::node right) 
{ return left->element > right->element; }
template <typename T>
inline bool operator>=(const typename BSTNode<T>::node left, const typename BSTNode<T>::node right) 
{ return left->element >= right->element; }
template <typename T>
inline bool operator==(const typename BSTNode<T>::node left, const typename BSTNode<T>::node right) {
          if (left.get() == nullptr && right.get() == nullptr) return true;

          if (left.get() != nullptr && right.get() != nullptr) {
                    bool ret = left->element == right->element;
                    if (ret == false) return ret;

                    if (!(left->left != nullptr ^ right->left != nullptr)) 
                    ret &= left->left != nullptr ? left->left == right->left : true;
                    else return false;

                    if (!(left->right != nullptr ^ right->right != nullptr))
                    ret &= left->right != nullptr ? left->right == right->right : true;
                    else return false;

                    return ret;
          } else return false;   
}
template <typename T>
inline bool operator!=(const typename BSTNode<T>::node left, const typename BSTNode<T>::node right) 
{ return !operator==(left, right); }

template <typename Type>
class BSTNode {
public:
          typedef Type value_type;
          typedef std::shared_ptr<BSTNode<Type>> node;

          // BSTNode(): element(0), left(nullptr), right(nullptr) { }
          explicit BSTNode(const Type& relem, const BSTNode *left = nullptr, const BSTNode *right = nullptr);
          ~BSTNode();

          BSTNode(const BSTNode<Type>* node);
          BSTNode(const BSTNode<Type>& node);
          BSTNode(const BSTNode<Type>&& rnode);
          void operator=(const BSTNode<Type>& node);
          void operator=(const BSTNode<Type>&& rnode);

          static BSTNode* create_node_(const Type& relem, const BSTNode<Type>* left = nullptr, const BSTNode<Type>* right = nullptr);
          static BSTNode* create_node_(const Type&& relem, const BSTNode<Type>* left = nullptr, const BSTNode<Type>* right = nullptr);
          static void release_node(const BSTNode<Type>* node);
          static const std::shared_ptr<BSTNode<Type>> create_node(const Type& relem, const BSTNode<Type>* left = nullptr, const BSTNode<Type>* right = nullptr);
          static const std::shared_ptr<BSTNode<Type>> create_node(const Type&& relem, const BSTNode<Type>* left = nullptr, const BSTNode<Type>* right = nullptr);

          Type element;
          node left = nullptr, right = nullptr;

          // Extra fundation for fast iterator
          // BSTNode<Type> *before = nullptr, *next = nullptr;
          node before = nullptr, next = nullptr;

          bool destroyed = false;
};

template <typename T>
using bstNode = typename BSTNode<T>::node;