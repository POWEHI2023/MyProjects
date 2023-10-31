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

          tree.check();
/**
 *                            8
 *                 1                     15
 *        -8             5        11            23
 */
          tree.insert(-10);   // YES
          tree.insert(-11);   

          tree.check();
/**
 *                            8
 *                 1                     15
 *        -10             5        11            23
 *   -11       -8
 */
          tree.erase(tree.find(-8));
          tree.erase(tree.find(5));

          tree.check();
/**
 *                            8
 *                -10                     15
 *         -11           1         11            23     
 */
          tree.insert(24);
          tree.insert(25);

          tree.check();
/**
 *                            8
 *                -10                     15
 *         -11           1         11            24 
 *                                          23        25
 */
          tree.erase(tree.find(23));
          tree.erase(tree.find(11));    // YES

          tree.check();
/**
 *                            8
 *                -10                     24
 *         -11           1          15          25
 */
          tree.insert(-1);
          tree.insert(2);

          tree.check();
/**
 *                            8
 *                -10                     24
 *         -11           1          15          25
 *                  -1       2
 */
          tree.insert(-2);

          tree.check();
/**
 *                            8
 *                 -1                     24
 *          -10           1          15          25
 *     -11       -2          2
 *                                
 */
          void normal_test1();
          normal_test1();
}

void normal_test1() {
          AVL::avl<int> tree;
          tree.insert(8);
          tree.insert(1);
          tree.insert(15);
          tree.insert(-8);
          tree.insert(23);
          tree.insert(5);
          tree.insert(11);

          tree.check();
/**
 *                            8
 *                 1                     15
 *        -8             5        11            23
 */
          tree.erase(tree.find(-8));
          tree.erase(tree.find(8));

          tree.check();
/**
 *                         1
 *                                5
 *                                       15
 *                                11            23
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