#include <iostream>
#include <stdlib.h>
#include <memory.h>
#include <stack>
#include <vector>
#include <functional>
#include <memory>
#include <cassert>

#include "../iterator.h"
#include "node.h"

template <typename T>
class BSTTree;

template <typename Type>
class BSTTree {
public:
          typedef default_iterator iterator_type;
          typedef Type value_type;

          class iterator {
          public:
                    iterator(const BSTNode<Type> *crt, const BSTTree<Type> *from);
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

          explicit BSTTree(std::function<bool(const Type&, const Type&)> fn = [](const Type& x, const Type& y) { return x > y; });
          BSTTree(const BSTTree& bst);
          BSTTree(const BSTTree&& bst);
          ~BSTTree();

          bool operator==(const BSTTree& bst);

          // Waiting......
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
          // Support for altanative customized function
          std::function<bool(const Type&, const Type&)> fn;

          // Extra fundation for fast finding begin and end iterator
          BSTNode<Type> *head = nullptr, *tail = nullptr;

          // Basic elements
          typename BSTNode<Type>::node root;
          bool modified = false;
          
          // Tool function
          bool erase_root(typename BSTNode<Type>::node& root);
public:
};