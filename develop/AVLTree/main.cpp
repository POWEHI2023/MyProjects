#include "avl.h"

void normal_test() {
          AVL::avl<int> tree;
          tree.insert(8);
          tree.insert(1);
          tree.insert(15);
          tree.insert(-8);
          tree.insert(23);
          tree.insert(5);
          tree.insert(11);
/**
 *                            8
 *                 1                     15
 *        -8             5        11            23
 */
          tree.insert(-10);
          tree.insert(-11);
/**
 *                            8
 *                 1                     15
 *        -10             5        11            23
 *   -11       -8
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