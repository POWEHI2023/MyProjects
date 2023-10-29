#include "bst.h"

void normal_test() {
          BST::bst<int> tree;
          tree.insert(5);
          tree.insert(3);
          tree.insert(7);
          tree.insert(1);
          tree.insert(4);
          tree.insert(6);
          tree.insert(8);

// normal itarator test
          BST::bst<int>::iterator iter = tree.begin();      // 1
          iter++;                                           // 3
          iter++;                                           // 4
          iter++;++iter;iter++;++iter;                      // 8 is_end_ false
          iter++;                                           // 8 is_end_ true

          iter--;--iter;
          iter--;--iter;
          iter--;--iter;
/**
 *                  5
 *        3                   7
 *   1        4          6         8
 */
          // BST::bst<int>::iterator iter = tree.find(3);
          tree.erase(tree.find(3));
/**
 *                  5
 *        1                   7
 *            4          6         8
 */    
          tree.erase(tree.find(5));
/**
 *                  1
 *                            4
 *                            
 *                                      7
 *                                 6         8
 */
}

int main(int args, char ** argv) {
          if (args == 1) normal_test();
          else 
{
          // Some customized test


}
          return 0;
}