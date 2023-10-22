#include <iostream>
#include <stdlib.h>
#include <memory.h>
#include <stack>
#include <vector>
#include <functional>
#include <memory>
#include <cassert>

#include "../iterator.h"
#include "node.cpp"

template <typename T>
class BSTTree;

template <typename Type>
class BSTTree {
public:
          typedef default_iterator iterator_type;
          typedef Type value_type;

          class iterator {
          public:
                    iterator(const BSTNode<Type> *crt);
                    iterator(const typename BSTNode<Type>::node& crt);
                    iterator(const iterator& iter);
                    iterator(const iterator&& iter);
                    void operator=(const iterator& iter);
                    void operator=(const iterator&& iter);
                    // 
                    bool operator==(const iterator& iter) const noexcept
                    { return this->crt == iter.crt && this->end == iter.end; }
                    bool operator==(const nullptr_t v) const noexcept
                    { return this->crt == v; }
                    bool operator!=(const iterator& iter) const noexcept
                    { return !this->operator==(iter); }
                    // 
                    iterator& operator++() noexcept;
                    const iterator operator++(int) noexcept;
                    iterator& operator--() noexcept;
                    const iterator operator--(int) noexcept;
                    iterator operator+(uint32_t steps) const noexcept;
                    iterator operator-(uint32_t steps) const noexcept;
                    void operator+=(uint32_t steps) noexcept;
                    void operator-=(uint32_t steps) noexcept;

                    BSTNode<Type>* operator->() const noexcept;
                    BSTNode<Type>& operator*() const noexcept;

                    bool destroyed() const
                    { return iter_destroyed || crt->destroyed; }
          private:
                    BSTNode<Type>* crt;
                    bool end = false;

                    bool iter_destroyed = false;
          public:          
                    friend BSTTree;
          };

          explicit BSTTree(std::function<bool(const Type&, const Type&)> fn = [](const Type& x, const Type& y) { return x > y; });
          BSTTree(const std::initializer_list<Type>& list, std::function<bool(const Type&, const Type&)> fn = [](const Type& x, const Type& y) { return x > y; }): 
          root(nullptr), fn(fn) { for (auto each : list) insert(each); }
          BSTTree(const BSTTree* bst);
          BSTTree(const BSTTree& bst);
          BSTTree(const BSTTree&& bst);

          virtual void operator=(const BSTTree& bst);
          virtual void operator=(const BSTTree&& bst);

          virtual ~BSTTree();

          bool operator==(const BSTTree& bst);

          // Waiting......
          // Base insert
          virtual void insert(const Type& elem);
          template <typename... _Args>
          void insert(const Type& first, const _Args&... others) {
                    insert(first);
                    insert(others...);
          }

          virtual void insert(const Type&& elem);
          template <typename... _Args>
          void insert(const Type&& first, _Args&&... others) {
                    insert(std::move(first));
                    insert(std::forward(others...));
          }

          // Rval insert
          // bool insert(const Type&& elem);
          bool erase(const iterator& iter);
          const iterator find(const Type& elem) const;
          iterator begin() const;
          iterator end() const;

          const std::vector<Type> to_array() const;
          const std::vector<Type> serilize() const;

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
          virtual bool erase_root(typename BSTNode<Type>::node& root);
public:
};

template <typename T>
inline decltype(auto) operator+(const typename BSTTree<T>::iterator iter, int x) {
          return std::forward(iter + x);
}