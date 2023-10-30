#ifndef RBTENTITY_
#define RBTENTITY_

#include <iostream>
#include <memory>
#include <functional>

namespace RBT {

enum class SORTER { greater, less };

template <typename ValueType>
class RedBlackNode;
template <typename ValueType>
using rbt_node = RedBlackNode<ValueType>*;
template <typename ValueType>
using smart_rbt_node = std::shared_ptr<RedBlackNode<ValueType>>;
template <
          typename ValueType, 
          SORTER cmp = SORTER::greater,
          typename NodeType = smart_rbt_node<ValueType>
          >
class RedBlackTree;
template <
          typename ValueType, 
          SORTER cmp = SORTER::greater,
          typename NodeType = smart_rbt_node<ValueType>
          >
using rbt = RedBlackTree<ValueType, cmp, NodeType>;

// Defninations & Implementations

template <typename ValueType>
struct RedBlackNode {
          ValueType value_;

          rbt_node<ValueType> rlink_ = nullptr, llink_ = nullptr;
};

template <typename ValueType, SORTER cmp, typename NodeType >
class RedBlackTree {
public:

private:
          NodeType root = nullptr;

          std::function<bool(const ValueType&, const ValueType&)> fn = [](const ValueType& x, const ValueType& y) -> bool {
                    return cmp == SORTER::greater ? x < y : x > y;
          };
public:

};



}

#endif