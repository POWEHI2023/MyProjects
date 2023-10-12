#include "BSTTree/bst.cpp"

#include <gtest/gtest.h>

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
                    printf("@@@\n");
                    BSTTree<int> *bst1 = new BSTTree<int>();
                    bst1->insert(6);
                    bst1->insert(3);
                    bst1->insert(8);
                    bst1->insert(1);
                    bst1->insert(4);

                    std::cout << bst1->modified << "\n";
                    std::vector<int> arr1 = bst1->to_array();
                    for (auto x : arr1) std::cout << x << " ";
                    std::cout << std::endl;
                    arr1 = bst1->serilize();
                    for (auto x : arr1) std::cout << x << " ";
                    std::cout << std::endl;
                    std::cout << bst1->modified << "\n";

                    BSTTree<int>::iterator iter = bst1->end();
                    std::cout << "Get iter " << iter->element << std::endl;
                    iter--;
                    std::cout << "Get iter " << iter->element << std::endl;
                    bst1->erase(iter);

                    arr1 = bst1->to_array();
                    for (auto x : arr1) std::cout << x << " ";
                    std::cout << std::endl;
                    arr1 = bst1->serilize();
                    for (auto x : arr1) std::cout << x << " ";
                    std::cout << std::endl;

                    bst1->customize([](const int& x, const int& y){ return x < y; });

                    arr1 = bst1->to_array();
                    for (auto x : arr1) std::cout << x << " ";
                    std::cout << std::endl;
                    arr1 = bst1->serilize();
                    for (auto x : arr1) std::cout << x << " ";
                    std::cout << std::endl;

                    bst1->customize([](const int& x, const int& y){ return x > y; });
                    arr1 = bst1->to_array();
                    for (auto x : arr1) std::cout << x << " ";
                    std::cout << std::endl;
                    arr1 = bst1->serilize();
                    for (auto x : arr1) std::cout << x << " ";
                    std::cout << std::endl;
          }
}

int main() {
          tree_test();
}