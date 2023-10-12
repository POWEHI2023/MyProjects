#include "BSTTree/bst.cpp"

#include <gtest/gtest.h>

void node_test () {
          // BSTTree<int> bst;
          {
          auto node = BSTNode<int>::create_node(0);
          // ASSERT_EQ(node->element, 0);
          // ASSERT_EQ((node->left == nullptr && node->right == nullptr), true);
          assert(node->element == 0);
          assert(node->left == nullptr && node->right == nullptr);
          }

          {
          BSTNode<int>::node root = BSTNode<int>::create_node(0);
          root->left = BSTNode<int>::create_node(1);
          root->right = BSTNode<int>::create_node(2);
          }

          {
          BSTNode<int>::node root = BSTNode<int>::create_node(0);
          root->left = BSTNode<int>::create_node(1);
          root->right = BSTNode<int>::create_node(2);

          BSTNode<int> another(root->left.get());
          BSTNode<int> another1(another);
          BSTNode<int> another2(std::move(another1));

          another1.element = -1;
          }

          {
          BSTNode<int>::node root1 = BSTNode<int>::create_node(0);
          root1->left = BSTNode<int>::create_node(1);
          root1->right = BSTNode<int>::create_node(2);

          BSTNode<int>::node root2 = BSTNode<int>::create_node(0);
          root2->left = BSTNode<int>::create_node(2);
          root2->right = BSTNode<int>::create_node(1);

          assert(root1 != root2);

          BSTNode<int>::node root3 = BSTNode<int>::create_node(0);
          root3->left = BSTNode<int>::create_node(2);
          root3->right = BSTNode<int>::create_node(1);

          assert(root2 == root3);
          }

          {
          BSTNode<int>::node root = BSTNode<int>::create_node(0);
          root->left = BSTNode<int>::create_node(1);
          root->right = BSTNode<int>::create_node(2);   
          root->left->right = BSTNode<int>::create_node(2);

          assert(root->right > root->left);
          assert(root->right >= root->left->right);
          assert(root->right <= root->left->right);
          assert(root->left < root->left->right);
          assert(root->left->right == root->right);
          }
}

void tree_test() {
          {
          BSTTree<int> *bst = new BSTTree<int>();
          delete bst;
          }

          {
          BSTTree<int> *bst = new BSTTree<int>();
          BSTTree<int> another(*bst);
          BSTTree<int> another1(std::move(another));
          delete bst;
          }

          {
          BSTTree<int> *bst = new BSTTree<int>();
          bst->insert(0);
          delete bst;
          }

          {
          BSTTree<int> *bst = new BSTTree<int>();
          /**
           *       6
           *     /   \
           *    3     8
           *   / \
           *  1   4
           * 
           * 
           *       \
           *        5
           */
          bst->insert(6);
          bst->insert(3);
          bst->insert(8);
          bst->insert(1);
          bst->insert(4);

          {
          BSTTree<int>::iterator iter = bst->begin();
          std::cout << iter->element << std::endl;
          iter++;
          std::cout << iter->element << std::endl;
          iter++;
          std::cout << iter->element << std::endl;
          iter++;
          std::cout << iter->element << std::endl;
          }

          std::vector<int> arr = bst->to_array();
          for (auto x : arr) std::cout << x << " ";
          std::cout << std::endl;

          bst->insert(5);
          std::vector<int> arr1 = bst->serilize();
          for (auto x : arr1) std::cout << x << " ";
          std::cout << std::endl;

          {
          BSTTree<int>::iterator iter = bst->begin();
          std::cout << iter->element << std::endl;
          iter++;
          std::cout << iter->element << std::endl;
          ++iter;
          std::cout << iter->element << std::endl;
          iter++;
          std::cout << iter->element << std::endl;
          ++iter;
          std::cout << iter->element << std::endl;
          iter++;
          std::cout << iter->element << std::endl;

          iter++;iter++;iter++;
          assert(iter == nullptr);

          BSTTree<int>::iterator niter = iter + 5;
          assert(niter == nullptr);
          }

          {
          BSTTree<int>::iterator iter = bst->end();
          std::cout << iter->element << std::endl;
          iter--;
          std::cout << iter->element << std::endl;
          --iter;
          std::cout << iter->element << std::endl;
          iter--;
          std::cout << iter->element << std::endl; 
          --iter;
          std::cout << iter->element << std::endl;
          iter--;
          std::cout << iter->element << std::endl;    
          }

          {
          BSTTree<int>::iterator iter = bst->end();
          iter -= 2;
          //iter--;
          std::cout << iter->element << std::endl;
          iter += 1;
          std::cout << iter->element << std::endl;
          }


          delete bst;
          }

          {
          std::cout << "Erase TEST" << std::endl;
          std::cout << std::endl;
          std::cout << std::endl;
          std::cout << std::endl;

          BSTTree<int> *bst = new BSTTree<int>();
          bst->insert(6);
          bst->insert(3);
          bst->insert(8);
          bst->insert(1);
          bst->insert(4);

          std::vector<int> arr = bst->to_array();
          for (auto x : arr) std::cout << x << " ";
          std::cout << std::endl;

          BSTTree<int>::iterator iter = bst->find(3);
          std::cout << iter->element << std::endl;
          iter++;
          std::cout << iter->element << std::endl;

          bst->erase(iter);
          arr = bst->to_array();
          for (auto x : arr) std::cout << x << " ";
          std::cout << std::endl;
          arr = bst->serilize();
          for (auto x : arr) std::cout << x << " ";
          std::cout << std::endl;

          bst->insert(4);
          arr = bst->to_array();
          for (auto x : arr) std::cout << x << " ";
          std::cout << std::endl;
          arr = bst->serilize();
          for (auto x : arr) std::cout << x << " ";
          std::cout << std::endl;

          BSTTree<int>::iterator iter1 = bst->end();
          bst->erase(iter1);
          arr = bst->to_array();
          for (auto x : arr) std::cout << x << " ";
          std::cout << std::endl;
          arr = bst->serilize();
          for (auto x : arr) std::cout << x << " ";
          std::cout << std::endl;

          delete bst;
          }

          {

                    printf("@@@\n");
                    BSTTree<int> bst1;
                    bst1.insert(6);
                    bst1.insert(3);
                    bst1.insert(8);
                    bst1.insert(1);
                    bst1.insert(4);

                    std::vector<int> arr1 = bst1.to_array();
                    for (auto x : arr1) std::cout << x << " ";
                    std::cout << std::endl;
                    arr1 = bst1.serilize();
                    for (auto x : arr1) std::cout << x << " ";
                    std::cout << std::endl;

                    BSTTree<int>::iterator iter = bst1.end();
                    iter--;
                    bst1.erase(iter);

                    arr1 = bst1.to_array();
                    for (auto x : arr1) std::cout << x << " ";
                    std::cout << std::endl;
                    arr1 = bst1.serilize();
                    for (auto x : arr1) std::cout << x << " ";
                    std::cout << std::endl;

                    bst1.customize([](const int& x, const int& y){ return x < y; });

                    arr1 = bst1.to_array();
                    for (auto x : arr1) std::cout << x << " ";
                    std::cout << std::endl;
                    arr1 = bst1.serilize();
                    for (auto x : arr1) std::cout << x << " ";
                    std::cout << std::endl;

                    bst1.customize([](const int& x, const int& y){ return x > y; });
                    arr1 = bst1.to_array();
                    for (auto x : arr1) std::cout << x << " ";
                    std::cout << std::endl;
                    arr1 = bst1.serilize();
                    for (auto x : arr1) std::cout << x << " ";
                    std::cout << std::endl;
          }
}

void func_test() {
          BSTTree<int> bst1;
          bst1.insert(6);
          bst1.insert(3);
          bst1.insert(8);
          bst1.insert(1);
          bst1.insert(4);

          std::vector<int> arr1 = bst1.to_array();
          for (auto x : arr1) std::cout << x << " ";
          std::cout << std::endl;

          BSTTree<int> bst2;
          bst1.insert(6);
          bst1.insert(3);
          bst1.insert(8);
          bst1.insert(1);
          bst1.insert(4);
}

int main() {
          // node_test();
          tree_test();
}