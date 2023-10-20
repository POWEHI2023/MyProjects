#include "BSTTree/bst.cpp"

#include <gtest/gtest.h>

template <typename T>
void display(const std::vector<T>& arr) {
          for (auto x : arr) std::cout << x << " ";
          std::cout << std::endl;
}

void tree_test() {
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

          {
                    BSTTree<int> bst1;
                    bst1.insert(6);
                    bst1.insert(3);
                    bst1.insert(8);
                    bst1.insert(1);
                    bst1.insert(4);

                    std::vector<int> arr1 = bst1.to_array();
                    display(arr1);

                    arr1 = bst1.serilize();
                    display(arr1);

                    bst1.insert(5);
                    arr1 = bst1.to_array();
                    display(arr1);

                    arr1 = bst1.serilize();
                    display(arr1);
          }

          {
                    BSTTree<int> bst1;
                    bst1.insert(6);
                    bst1.insert(3);
                    bst1.insert(8);
                    bst1.insert(1);
                    bst1.insert(4);

                    BSTTree<int>::iterator iter = bst1.begin();
                    for (; iter != bst1.end(); ++iter) {
                              printf("%d ", iter->element);
                    }
                    printf("\n");

                    bst1.insert(4);
                    iter = bst1.begin();
                    for (; iter != bst1.end(); ++iter) {
                              printf("%d ", iter->element);
                    }
                    printf("\n");
          }

          {
                    BSTTree<int> bst{6,3,8,1,4};
          
                    std::vector<int> arr = bst.to_array();
                    display(arr);

                    bst.customize([](int x, int y){ return x < y; });
                    arr = bst.to_array();
                    display(arr);
          }
}

int main() {
          tree_test();
}