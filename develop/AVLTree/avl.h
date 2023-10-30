#ifndef AVLENTITY_
#define AVLENTITY_

#include <memory>
#include <functional>
#include <cmath>

namespace AVL {

template <typename ValueType>      
struct AVLNode;
template <typename ValueType>
using avl_node = AVLNode<ValueType>*;
template <typename ValueType>
using smart_avl_node = std::shared_ptr<AVLNode<ValueType>>;

template <typename T>
inline T max(const T& x, const T& y) noexcept { return x > y ? x : y; };

template <typename ValueType>
struct AVLNode {
          ValueType value_;
          
          pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
          smart_avl_node<ValueType> left_ = nullptr, right_ = nullptr;
          avl_node<ValueType> llink_ = nullptr, rlink_ = nullptr, parent_ = nullptr;
          uint32_t deep_ = 1;

          AVLNode(const ValueType& v): value_(v) {}
          ~AVLNode() { pthread_rwlock_destroy(&rwlock); }

          AVLNode(const AVLNode& n): 
          value_(n.value_), llink_(n.llink_), rlink_(n.rlink_), deep_(n.deep_), left_(n.left_), right_(n.right_)
          /*deep_(1 + max(left_.deep_, right_.deep_))*/, parent_(n.parent_) {}
          AVLNode(const avl_node<ValueType> n): 
          value_(n->value_), llink_(n->llink_), rlink_(n->rlink_), deep_(n->deep_), left_(n->left_), right_(n->right_)
          /*deep_(1 + max(left_.deep_, right_.deep_))*/, parent_(n->parent_) {}
          void operator=(const AVLNode& n) {
                    // left_ = nullptr; right_ = nullptr;
                    llink_ = n.llink_; rlink_ = n.rlink_; parent_ = n.parent_;
                    value_ = n.value_; 
                    left_ = std::move(n->left_); right_ = std::move(n->right_);
                    // deep_ = 1 + max(left_.deep_, right_.deep_);
                    deep_ = n.deep_;
          }
          void operator=(const smart_avl_node<ValueType> n) {
                    // left_ = nullptr; right_ = nullptr;
                    llink_ = n->llink_; rlink_ = n->rlink_; parent_ = n->parent_;
                    value_ = n->value_; 
                    
                    left_ = std::move(n->left_); right_ = std::move(n->right_);
                    // deep_ = 1 + max(left_.deep_, right_.deep_);
                    deep_ = n->deep_;
          }
          operator ValueType() { return value_; }

          void add1layer() noexcept {
                    int ld = left_deep();
                    int rd = right_deep();

                    if ((1 + max(ld, rd)) == deep_) return;
                    
                    deep_ = (1 + max(ld, rd));
                    if (abs(ld - rd) <= 1) goto RET;

                    if (ld > rd)  {
                              int lld = left_->left_ == nullptr ? 0 : left_->left_deep();
                              int lrd = left_->right_ == nullptr ? 0 : left_->right_deep();
                              if (lld < lrd) left_->rotate_left();
                              rotate_right();
                    }
                    else {
                              int rld = right_->left_ == nullptr ? 0 : right_->left_deep();
                              int rrd = right_->right_ == nullptr ? 0 : right_->right_deep();
                              if (rld > rrd) right_->rotate_right();
                              rotate_left();
                    }

                    RET:
                    if (parent_ != nullptr) parent_->add1layer();
          };

          uint32_t right_deep() noexcept { return right_ == nullptr ? 0 : right_->deep_; }
          uint32_t left_deep() noexcept { return left_ == nullptr ? 0 : left_->deep_; }

          void rotate_left() noexcept {
                    smart_avl_node<ValueType> nr = right_, nl = smart_avl_node<ValueType>(new AVLNode(this));
                    nr->llink_ = get_origin_p(nl);
                    if (nl->rlink_ == get_origin_p(nr)) nl->rlink_ = this;

                                                  deep_ = 1 + max(left_deep(), right_->left_deep());
                                                  nr->deep_ = 1 + max(deep_, nr->right_deep());

                    nl->right_ = nr->left_;       if (nr->left_ != nullptr) nr->left_->parent_ = get_origin_p(nl->right_);
                    nr->left_ = nl;               nl->parent_ = get_origin_p(nr->left_);
                    nr->parent_ = parent_;        

                    if (nr->right_ != nullptr && nr->right_->llink_ == get_origin_p(nr)) nr->right_->llink_ = this;
                    *this = std::move(nr);
          }
          void rotate_right() noexcept {
                    smart_avl_node<ValueType> nl = left_; left_ = nullptr;

                    smart_avl_node<ValueType> nr = smart_avl_node<ValueType>(new AVLNode(this));
                    nl->rlink_ = get_origin_p(nr);          
                    if (nr->llink_ == get_origin_p(nl)) nr->llink_ = this;

                                                  deep_ = 1 + max(right_deep(), nl->right_deep());
                                                  nl->deep_ = 1 + max(deep_, nl->left_deep());

                    nr->left_ = nl->right_;       
                    nl->right_ = nr;     
                                                  nr->parent_ = get_origin_p(nl->right_);
                                                  if (nl->right_ != nullptr) nl->right_->parent_ = get_origin_p(nr->left_);
                                                  nl->parent_ = parent_;        

                    if (nl->left_ != nullptr && nl->left_->rlink_ == get_origin_p(nl)) nl->left_->rlink_ = this;
                                                  *this = std::move(nl);
          }

          void link_left(const smart_avl_node<ValueType>& child) noexcept { link_left(child.get()); }
          void link_right(const smart_avl_node<ValueType>& child) noexcept { link_right(child.get()); }

          // TODO FUNC
          void link_left(const avl_node<ValueType>& child) noexcept {
                    smart_avl_node<ValueType> left_node = smart_avl_node<ValueType>(new AVLNode(child->value_));
                    left_node->parent_ = this;
                              // oleft <-> this => oleft <-> node <-> node
                    left_node->llink_ = llink_;
                    left_node->rlink_ = this;
                    if (llink_ != nullptr) llink_->rlink_ = left_node.get();
                    llink_ = left_node.get();
                    left_ =  std::move(left_node);
                    add1layer();
          }
          // TODO FUNC
          void link_right(const avl_node<ValueType>& child) noexcept {
                    smart_avl_node<ValueType> right_node = smart_avl_node<ValueType>(new AVLNode(child->value_));
                    right_node->parent_ = this;
                              // this <-> oright => this <-> node <-> oright
                    right_node->llink_ = this;
                    right_node->rlink_ = this->rlink_;
                    if (rlink_ != nullptr) rlink_->llink_ = right_node.get();
                    rlink_ = right_node.get();
                    right_ = std::move(right_node);
                    add1layer();
          }

          template <typename Arg>
          void jump_node(const Arg& n) noexcept {
                    avl_node<ValueType> n_ = get_origin_p(n);
                    if (n_->llink_ != nullptr) n_->llink_->rlink_ = n_->rlink_;
                    if (n_->rlink_ != nullptr) n_->rlink_->llink_ = n_->llink_;
          }

          // TODO FUNC
          void unlink_left() noexcept {
                    pthread_rwlock_wrlock(&rwlock);
                    if (left_ == nullptr) goto RET;
                    jump_node(left_);


                    RET:
                    pthread_rwlock_unlock(&rwlock);
          }

          // TODO FUNC
          void unlink_right() noexcept {
                    pthread_rwlock_wrlock(&rwlock);
                    if (right_ == nullptr) goto RET;
                    jump_node(right_);


                    RET:
                    pthread_rwlock_unlock(&rwlock);
          }

          ValueType operator*() { return value_; }

          avl_node<ValueType> find_value(const ValueType& v, std::function<bool(const ValueType& x, const ValueType& y)>& fn) noexcept {
                    if (value_ == v) return this;
                    if (fn(value_, v)) 
                              return right_ == nullptr ? nullptr : right_->find_value(v, fn);
                    else
                              return left_ == nullptr ? nullptr : left_->find_value(v, fn);
          }
};

template<typename T>
inline bool operator==(const AVLNode<T>& ln, const AVLNode<T>& rn) noexcept { return ln.value_ == rn.value_; }
template<typename T>
inline bool operator!=(const AVLNode<T>& ln, const AVLNode<T>& rn) noexcept { return !(ln == rn); }
template<typename T>
inline bool operator>(const AVLNode<T>& ln, const AVLNode<T>& rn) noexcept { return ln.value_ > rn.value_; }
template<typename T>
inline bool operator>=(const AVLNode<T>& ln, const AVLNode<T>& rn) noexcept { return ln.value_ >= rn.value_; }
template<typename T>
inline bool operator<(const AVLNode<T>& ln, const AVLNode<T>& rn) noexcept { return ln.value_ < rn.value_; }
template<typename T>
inline bool operator<=(const AVLNode<T>& ln, const AVLNode<T>& rn) noexcept { return ln.value_ <= rn.value_; }

/**
 * Transition structure
 */
template <typename T>
struct OpaqueNode_ {
          avl_node<T> node_;
          OpaqueNode_(const avl_node<T> n): node_(n) {}
          OpaqueNode_(const smart_avl_node<T> n): node_(n.get()) {}
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

template <typename T>
inline const avl_node<T> get_origin_p(const avl_node<T> n) noexcept { return n; }
template <typename T>
inline const avl_node<T> get_origin_p(const smart_avl_node<T> n) noexcept { return n.get(); }

template <
          typename ValueType, 
          SORTER cmp = SORTER::greater,
          typename NodeType = smart_avl_node<ValueType>
          >
class AVLTree {
public:

private:
          NodeType root;
          pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

          std::function<bool(const ValueType&, const ValueType&)> fn = [](const ValueType& x, const ValueType& y) -> bool {
                    return cmp == SORTER::greater ? x < y : x > y;
          };
public:
          void insert(const AVLNode<ValueType>& node) noexcept {
                    NodeType temp = NodeType(const_cast<avl_node<ValueType>>(new AVLNode<ValueType>(node))); 
                    insert(temp);
          }
          void insert(const NodeType& node) noexcept {
                    if (root == nullptr) { root = node; return; }

                    avl_node<ValueType> tar = get_origin_p(root);
                    while(1) {
                              if (fn(tar->value_, node->value_))     // right
                              if (tar->right_ == nullptr) return tar->link_right(node);
                              else tar = get_origin_p(tar->right_);

                              else
                              if (tar->left_ == nullptr) return tar->link_left(node);
                              else tar = get_origin_p(tar->left_);
                    }
          }
};
template <
          typename ValueType, 
          SORTER cmp = SORTER::greater,
          typename NodeType = smart_avl_node<ValueType>
          >
using avl = AVLTree<ValueType, cmp, NodeType>;

}
#endif