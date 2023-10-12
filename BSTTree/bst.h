#include <iostream>
#include <stdlib.h>
#include <memory.h>
#include <stack>
#include <vector>
#include <functional>
#include <memory>
#include <cassert>

#include "../iterator.h"

namespace BST {
          enum CMP {DESC, ASC};
          template <typename T>
          bool greater(const T& x, const T& y)
          { return x > y; }
          template <typename T>
          bool less(const T& x, const T& y)
          { return x < y; }
}

template <typename T>
class BSTNode;
template <typename T, BST::CMP cmp = BST::DESC>
class BSTTree;

template <typename T>
inline bool node_exist(typename BSTNode<T>::node& node);

template <typename T>
inline bool operator<(const std::shared_ptr<BSTNode<T>> left, const std::shared_ptr<BSTNode<T>> right) 
{ return left->element < right->element; }
template <typename T>
inline bool operator<=(const std::shared_ptr<BSTNode<T>> left, const std::shared_ptr<BSTNode<T>> right) 
{ return left->element <= right->element; }
template <typename T>
inline bool operator>(const std::shared_ptr<BSTNode<T>> left, const std::shared_ptr<BSTNode<T>> right) 
{ return left->element > right->element; }
template <typename T>
inline bool operator>=(const std::shared_ptr<BSTNode<T>> left, const std::shared_ptr<BSTNode<T>> right) 
{ return left->element >= right->element; }
template <typename T>
inline bool operator==(const std::shared_ptr<BSTNode<T>> left, const std::shared_ptr<BSTNode<T>> right) {
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
inline bool operator!=(const std::shared_ptr<BSTNode<T>> left, const std::shared_ptr<BSTNode<T>> right) 
{ return !operator==(left, right); }

template <typename Type>
class BSTNode {
public:
          typedef Type value_type;
          typedef std::shared_ptr<BSTNode<Type>> node;

          // BSTNode(): element(0), left(nullptr), right(nullptr) { }
          explicit BSTNode(const Type& relem, const BSTNode *left, const BSTNode *right);
          ~BSTNode();

          BSTNode(const BSTNode<Type> *node);
          BSTNode(const BSTNode<Type>& node);
          BSTNode(const BSTNode<Type>&& rnode);
          void clear();

          static BSTNode* create_node(int, const Type& relem, const BSTNode<Type>* left = nullptr, const BSTNode<Type>* right = nullptr);
          static void release_node(const BSTNode<Type>* node);

          static std::shared_ptr<BSTNode<Type>> create_node(const Type& relem, const BSTNode<Type>* left = nullptr, const BSTNode<Type>* right = nullptr);

          Type element;
          node left, right;

          // Extra fundation for fast iterator
          BSTNode<Type> *before = nullptr, *next = nullptr;
          bool destroyed = false;
};

template <typename Type, BST::CMP cmp>
class BSTTree {
          typedef bool (*cpf) (const Type& x, const Type& y);
public:
          typedef default_iterator iterator_type;
          typedef Type value_type;

          class iterator {
          public:
                    iterator(const BSTNode<Type> *crt, const BSTTree<Type, cmp> *from);
                    iterator(const iterator& iter);
                    iterator(const iterator&& iter);
                    bool operator==(const iterator& iter) const
                    { return this->crt == iter.crt && this->from == iter.from; }
                    bool operator==(const nullptr_t v) const
                    { return this->crt == v; }

                    bool operator!=(const iterator& iter) const
                    { return !this->operator==(iter); }

                    iterator& operator++();
                    const iterator operator++(int);
                    iterator& operator--();
                    const iterator operator--(int);
                    const iterator operator+(uint32_t steps);
                    const iterator operator-(uint32_t steps);
                    void operator+=(uint32_t steps);
                    void operator-=(uint32_t steps);

                    BSTNode<Type>* operator->() const;

                    bool destroyed() const
                    { return !crt->before && !crt->next && crt != from->root; }
          private:
                    BSTNode<Type> *crt;
                    BSTTree *from;

          public:          
                    friend BSTTree;
                    
          };

          BSTTree();
          ~BSTTree();
          BSTTree(const BSTTree& bst);
          BSTTree(const BSTTree&& bst);
          bool operator==(const BSTTree& bst);

          // Base insert
          bool insert(const Type& elem);
          // Rval insert
          bool insert(const Type&& elem);
          bool erase(const iterator& iter);
          const iterator find(const Type& elem);

          const iterator begin();
          const iterator end();

          const std::vector<Type> to_array();
          const std::vector<Type> serilize();

          bool customize(std::function<bool(const Type&, const Type&)> f);
private:
          typename BSTNode<Type>::node root;
          // cpf fn;
          
          // Support for altanative customized function
          std::function<bool(const Type&, const Type&)> fn;

          // Extra fundation for fast finding begin and end iterator
          BSTNode<Type> *head = nullptr, *tail = nullptr;
          bool modified = false;

          bool erase_root(typename BSTNode<Type>::node& root);
public:
};