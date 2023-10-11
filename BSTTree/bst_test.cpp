#include "bst.cpp"

#include <gtest/gtest.h>

void node_test () {
          // BSTTree<int> bst;
          {
          std::shared_ptr<BSTNode<int>> node = std::make_shared<BSTNode<int>>(BSTNode<int>::create_node(0));
          ASSERT_EQ(node->element, 0);
          ASSERT_EQ((node->left == nullptr && node->right == nullptr), true);
          }
}