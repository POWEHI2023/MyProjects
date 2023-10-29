/**
 * Parallel safe red-black tree template (binary search tree part)
 * @author POWEHI2023
 * 
 */

#ifndef BSTENTITY_
#define BSTENTITY_

#include <iostream>
#include <memory>
#include <functional>
#include <string>

#include <stdlib.h>
#include <pthread.h>

namespace BST {

template <typename ValueType>
class BinarySearchTree_Node_;
template <typename ValueType>
using bst_node = BinarySearchTree_Node_<ValueType>*;
template <typename ValueType>
using smart_bst_node = std::shared_ptr<BinarySearchTree_Node_<ValueType>>;
/**
 * Template implementation for bst
 */

template <typename ValueType>
struct BinarySearchTree_Node_  {
          ValueType value_;
          
          pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
          smart_bst_node<ValueType> left_ = nullptr, right_ = nullptr;
          bst_node<ValueType> llink_ = nullptr, rlink_ = nullptr;

          bool destroyed = false;

          BinarySearchTree_Node_(const ValueType& v): value_(v) {}
          ~BinarySearchTree_Node_() { pthread_rwlock_destroy(&rwlock); }

          BinarySearchTree_Node_(const BinarySearchTree_Node_& n): 
          value_(n.value_), left_(n.left_), right_(n.right_), llink_(n.llink_), rlink_(n.rlink_) {}
          BinarySearchTree_Node_(const bst_node<ValueType> n): 
          value_(n->value_), left_(n->left_), right_(n->right_), llink_(n->llink_), rlink_(n->rlink_) {}
          void operator=(const BinarySearchTree_Node_& n) {
                    value_ = n.value_; left_ = n.left_; right_ = n.right_;
                    llink_ = n.llink_; rlink_ = n.rlink_;
          }
          operator ValueType() { return value_; }

          void link_left(const smart_bst_node<ValueType>& child) noexcept { link_left(child.get()); }
          void link_right(const smart_bst_node<ValueType>& child) noexcept { link_right(child.get()); }
          void link_left(const bst_node<ValueType>& child) noexcept {
                    smart_bst_node<ValueType> left_node = smart_bst_node<ValueType>(new BinarySearchTree_Node_(child->value_));
                              // oleft <-> this => oleft <-> node <-> node
                    left_node->llink_ = llink_;
                    left_node->rlink_ = this;
                    if (llink_ != nullptr) llink_->rlink_ = left_node.get();
                    llink_ = left_node.get();
                    left_ =  std::move(left_node);
          }
          void link_right(const bst_node<ValueType>& child) noexcept {
                    smart_bst_node<ValueType> right_node = smart_bst_node<ValueType>(new BinarySearchTree_Node_(child->value_));
                              // this <-> oright => this <-> node <-> oright
                    right_node->llink_ = this;
                    right_node->rlink_ = this->rlink_;
                    if (rlink_ != nullptr) rlink_->llink_ = right_node.get();
                    rlink_ = right_node.get();
                    right_ = std::move(right_node);
          }

          template <typename Arg>
          void jump_node(const Arg& n) noexcept {
                    bst_node<ValueType> n_ = get_origin_p(n);
                    if (n_->llink_ != nullptr) n_->llink_->rlink_ = n_->rlink_;
                    if (n_->rlink_ != nullptr) n_->rlink_->llink_ = n_->llink_;
          }
          void unlink_left() noexcept {
                    pthread_rwlock_wrlock(&rwlock);
                    if (left_ == nullptr) goto RET;

                    jump_node(left_);
                    if (left_->left_ == nullptr) left_ = left_->right_;
                    else 
                    if (left_->right_ == nullptr) left_ = left_->left_;
                    else {
                              smart_bst_node<ValueType> l = left_->left_;
                              while (l->right_ != nullptr) l = l->right_;
                              l->right_ = left_->right_;
                              left_ = left_->left_;
                    }
                    RET:
                    pthread_rwlock_unlock(&rwlock);
          }
          void unlink_right() noexcept {
                    pthread_rwlock_wrlock(&rwlock);
                    if (right_ == nullptr) goto RET;

                    jump_node(right_);
                    if (right_->left_ == nullptr) right_ = right_->right_;
                    else 
                    if (right_->right_ == nullptr) right_ = right_->left_;
                    else {
                              smart_bst_node<ValueType> r = right_->right_;
                              while (r->left_ != nullptr) r = r->left_;
                              r->left_ = right_->left_;
                              right_ = right_->right_;
                    }
                    RET:
                    pthread_rwlock_unlock(&rwlock);
          }

          // ValueType operator->() { return value_; }
          ValueType operator*() { return value_; }

          bst_node<ValueType> find_value(const ValueType& v, std::function<bool(const ValueType& x, const ValueType& y)>& fn) noexcept {
                    if (value_ == v) return this;
                    if (fn(value_, v)) 
                              return right_ == nullptr ? nullptr : right_->find_value(v, fn);
                    else
                              return left_ == nullptr ? nullptr : left_->find_value(v, fn);
          }
};

template<typename T>
inline bool operator==(const BinarySearchTree_Node_<T>& ln, const BinarySearchTree_Node_<T>& rn) noexcept { return ln.value_ == rn.value_; }
template<typename T>
inline bool operator!=(const BinarySearchTree_Node_<T>& ln, const BinarySearchTree_Node_<T>& rn) noexcept { return !(ln == rn); }
template<typename T>
inline bool operator>(const BinarySearchTree_Node_<T>& ln, const BinarySearchTree_Node_<T>& rn) noexcept { return ln.value_ > rn.value_; }
template<typename T>
inline bool operator>=(const BinarySearchTree_Node_<T>& ln, const BinarySearchTree_Node_<T>& rn) noexcept { return ln.value_ >= rn.value_; }
template<typename T>
inline bool operator<(const BinarySearchTree_Node_<T>& ln, const BinarySearchTree_Node_<T>& rn) noexcept { return ln.value_ < rn.value_; }
template<typename T>
inline bool operator<=(const BinarySearchTree_Node_<T>& ln, const BinarySearchTree_Node_<T>& rn) noexcept { return ln.value_ <= rn.value_; }

template <typename T>
struct OpaqueNode_ {
          bst_node<T> node_;
          OpaqueNode_(const bst_node<T> n): node_(n) {}
          OpaqueNode_(const smart_bst_node<T> n): node_(n.get()) {}
          ~OpaqueNode_() {}
};
template<typename T>
inline bool operator==(const OpaqueNode_<T>& ln, const  OpaqueNode_<T>& rn) noexcept { return *ln.node_ == *rn.node_; }
template<typename T>
inline bool operator!=(const OpaqueNode_<T>& ln, const  OpaqueNode_<T>& rn) noexcept { return *ln.node_ != *rn.node_; }
template<typename T>
inline bool operator<(const OpaqueNode_<T>& ln, const  OpaqueNode_<T>& rn) noexcept { return *ln.node_ < *rn.node_; }
template<typename T>
inline bool operator>(const OpaqueNode_<T>& ln, const  OpaqueNode_<T>& rn) noexcept { return *ln.node_ > *rn.node_; }
template<typename T>
inline bool operator>=(const OpaqueNode_<T>& ln, const  OpaqueNode_<T>& rn) noexcept { return *ln.node_ >= *rn.node_; }
template<typename T>
inline bool operator<=(const OpaqueNode_<T>& ln, const  OpaqueNode_<T>& rn) noexcept { return *ln.node_ <= *rn.node_; }

/**
 * @details
 *        greater:  MIN -> MAX
 *        less:     MAX -> MIN
 */
enum class SORTER: uint8_t { greater, less };

template<typename ValueType>
const bst_node<ValueType> create_node(const ValueType& v) noexcept { return new BinarySearchTree_Node_<ValueType>(v); }
template<typename ValueType>
void release_node(const bst_node<ValueType> n) noexcept { 
          // n->left_ = n->right_ = nullptr;
          delete n; 
}
template<typename ValueType>
const smart_bst_node<ValueType> create_smart_node(const ValueType& v) noexcept { return std::move(smart_bst_node<ValueType>(create_node(v), release_node)); }

void crash(const std::string& msg) noexcept {
          std::cout << "Crash: " << msg << "\n";
          exit(1);
}
template <typename T>
inline const bst_node<T> get_origin_p(const bst_node<T> n) noexcept { return n; }
template <typename T>
inline const bst_node<T> get_origin_p(const smart_bst_node<T> n) noexcept { return n.get(); }
template <typename T>
inline const void set_p(bst_node<T>& l, smart_bst_node<T>& r) noexcept { l = r.get(); }
template <typename T>
inline const void set_p(smart_bst_node<T>& l, smart_bst_node<T>& r) noexcept { l = r; }


struct nullfunc_t {
          template<typename F>
          constexpr operator std::function<F>() { return [](auto&&...){}; }
} nullfunc;
template <typename T>
void nulldel_(const T*) {}
/**
 * Binary search tree
 */
template <
          typename ValueType, 
          typename NodeType = smart_bst_node<ValueType>, 
          SORTER cmp = SORTER::greater
          >
class BinarySearchTree {
          using uint32 = u_int32_t;
          using int32 = int32_t;
public:
          class iterator {
          public:
                    explicit iterator(const bst_node<ValueType>& tar): pos(tar) {}
                    explicit iterator(const smart_bst_node<ValueType>& tar): pos(tar.get()) {}
                    iterator(const iterator& iter): pos(iter.pos), is_end_(iter.is_end_) {}
                    ~iterator() {}

                    const iterator& operator++() const noexcept {
                              if (is_end_ == true) crash("Can not cal operator++ for an end-iterator!");
                              if (pos->rlink_ != nullptr) pos = pos->rlink_;
                              else is_end_ = true;
                              return *this;
                    }
                    const iterator operator++(int) const noexcept {
                              if (is_end_ == true) crash("Can not cal operator++ for an end-iterator!");
                              iterator iter(*this);
                              if (pos->rlink_ != nullptr) pos = pos->rlink_;
                              else is_end_ = true;
                              return std::move(iter);
                    }
                    const iterator& operator--() const noexcept {
                              if (is_end_ == true) is_end_ = false;
                              else if (pos->llink_ != nullptr) pos = pos->llink_;
                              else crash("Can not call operator-- for an begin-iterator!");
                              return *this;
                    }
                    const iterator operator--(int) const noexcept {
                              iterator iter(*this);
                              if (is_end_ == true) is_end_ = false;
                              else if (pos->llink_ != nullptr) pos = pos->llink_;
                              else crash("Can not call operator-- for an begin-iterator!");
                              return std::move(iter);
                    }

                    // bst_node<ValueType> operator->() noexcept { return pos; }
                    ValueType operator*() noexcept { return pos->operator*(); }
                    // const bst_node<ValueType> operator->() const noexcept { return const_cast<const bst_node<ValueType>>(pos); }
                    const ValueType operator*() const noexcept { return *pos; }
          private:
                    mutable bst_node<ValueType> pos = NULL;
                    mutable bool is_end_ = false;
          };
private:
          NodeType root;
          pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

          std::function<bool(const ValueType&, const ValueType&)> fn = [](const ValueType& x, const ValueType& y) -> bool {
                    return cmp == SORTER::greater ? x < y : x > y;
          };
public:
          BinarySearchTree(): root(nullptr) {}
          BinarySearchTree(BinarySearchTree& bst): root(bst.root), fn(bst.fn) {}
          BinarySearchTree(BinarySearchTree* bst): root(bst->root), fn(bst->fn) {}
          void operator=(BinarySearchTree& bst) { root = bst.root; fn = bst.fn; }
          void operator=(BinarySearchTree* bst) { root = bst->root; fn = bst->fn; }
          ~BinarySearchTree() {}
          void insert(const BinarySearchTree_Node_<ValueType>& node) noexcept {
                    NodeType temp = NodeType(const_cast<bst_node<ValueType>>(new BinarySearchTree_Node_<ValueType>(node))); 
                    insert(temp);
          }
          void insert(const NodeType& node) noexcept {
                    if (root == nullptr) { root = node; return; }

                    bst_node<ValueType> tar = get_origin_p(root);
                    while(1) {
                              if (fn(tar->value_, node->value_))     // right
                              if (tar->right_ == nullptr) return tar->link_right(node);
                              else tar = get_origin_p(tar->right_);

                              else
                              if (tar->left_ == nullptr) return tar->link_left(node);
                              else tar = get_origin_p(tar->left_);
                    }
          }
          void erase(const iterator& iter) noexcept {
                    if (*iter == root->value_) {
                              pthread_mutex_lock(&mtx);
                              if (root->llink_ != nullptr) root->llink_->rlink_ = root->rlink_;
                              if (root->rlink_ != nullptr) root->rlink_->llink_ = root->llink_;
                              if (root->left_ != nullptr) 
                              if (root->right_ == nullptr) set_p(root, root->left_);
                              else {
                                        smart_bst_node<ValueType> l = root->left_;
                                        while (l->right_ != nullptr) l = l->right_;
                                        l->right_ = root->right_;
                                        set_p(root, root->left_);
                              } 
                              else set_p(root, root->right_);
                              pthread_mutex_unlock(&mtx);
                              return;
                    }
                    bst_node<ValueType> tar = get_origin_p(root);
                    while (tar != nullptr) {
                              if (fn(tar->value_, *iter))
                              if (tar->right_ != nullptr && tar->right_->value_ == *iter) return tar->unlink_right();
                              else tar = get_origin_p(tar->right_);

                              else
                              if (tar->left_ != nullptr && tar->left_->value_ == *iter) return tar->unlink_left();
                              else tar = get_origin_p(tar->left_);
                    }
          }

          iterator find(const ValueType& v) noexcept { return iterator(root->find_value(v, fn)); }
          const iterator cfind(const ValueType& v) const noexcept { return const_cast<const iterator>(find(v)); }
          iterator begin() noexcept {
                    bst_node<ValueType> tar = get_origin_p(root);
                    while (tar->left_) tar = get_origin_p(tar->left_);
                    return iterator(tar);
          }

          iterator end() noexcept {
                    bst_node<ValueType> tar = get_origin_p(root);
                    while (tar->right_) tar = get_origin_p(tar->right_);
                    iterator iter(tar); tar++;
                    return std::move(tar);
          }
};
template <
          typename ValueType, 
          typename NodeType = smart_bst_node<ValueType>, 
          SORTER cmp = SORTER::greater
          >
using bst = BinarySearchTree<ValueType, NodeType, cmp>;

}
#endif