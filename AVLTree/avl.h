#include "../BSTTree/bst.cpp"

template <typename T>
class AVLTree : public BSTTree<T> {
public:
          virtual void insert(const T& elem);
          virtual void insert(const T&& elem);
          template <typename... _Args>
          void insert(const T& first, const _Args... others) {
                    insert(first);
                    insert(others...);
          }
          template <typename... _Args>
          void insert(const T&& first, _Args&&... others) {
                    insert(std::move(first));
                    insert(std::forward(others...));
          }

          using Iterator = typename BSTTree<T>::iterator;

          AVLTree(const AVLTree* avl);
          AVLTree(const AVLTree& avl);
          AVLTree(const AVLTree&& avl);
          virtual void operator=(const AVLTree& avl);
          virtual void operator=(const AVLTree&& avl);
private:
          virtual bool erase_root(bstNode<T>& root);
};