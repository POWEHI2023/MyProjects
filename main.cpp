#include "BSTTree/bst.cpp"
#include "BPTree/bp.cpp"

#include <gtest/gtest.h>

template <typename T>
void display(const std::vector<T>& arr) {
          for (auto x : arr) std::cout << x << " ";
          std::cout << std::endl;
}

template <typename T>
void arr4display(const BSTTree<T>& tree) {
          std::vector<T> arr = tree.to_array();
          display(arr);
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

          {
                    BSTTree<int> bst{6, 3, 8, 1, 4, 5};
                    arr4display(bst);

                    bst.insert(10);
                    arr4display(bst);

                    bst.erase(bst.find(3));
                    arr4display(bst);

                    bst.erase(bst.begin());
                    arr4display(bst);

                    BSTTree<int>::iterator iter = bst.begin();
                    iter += 4;
                    bst.erase(iter);
                    arr4display(bst);

                    bst.erase(bst.begin() + 1 + 2);
                    arr4display(bst);
          }

          {
                    std::shared_ptr<BSTTree<int>> p(new BSTTree<int>, [](BSTTree<int>* x) {
                              printf("Delete BSTTree\n");
                              delete x;
                    });  
          }
          printf("After\n");
}

void element_test() {
          auto displayElement4ii = [](const Element<int, int>& x) {
                    auto [k, v] = x.get_content();
                    printf("Element{Key: %d : Value: %d}\n", k, v);
          };
          auto x = Element<int, int>(1, 0, true);
          displayElement4ii(x);

          x.set_key(2);
          x.set_value(1);
          displayElement4ii(x);
}

void node_test() {
          auto displayNode4i5 = [](const Node<int, int>& node) {
                    printf("Node size: %ld, Type: %s\n", node.size(),
                              node._type == NodeType::LeafNode ? "Leaf node" : "Inner node");
                    for (int i = 0; i < node.size(); ++i) {
                              auto [k, v, t] = node[i];
                              if (t) printf("``Key: %d : Value: %d\n", k, v._v);
                    }
                    
          };

          auto x = Node<int, int>();
          x.insert(0, 0);
          displayNode4i5(x);

          auto y = Node<int, int>();
          y.insert(-1, -1);
          displayNode4i5(y);
          y.insert(1, 1);

          auto z(y);
          z.insert(2, 2);
          displayNode4i5(z);
          
          auto m = Node<int, int>(x);
          displayNode4i5(m);

          auto i = Node<int, int>::create_node(NodeType::LeafNode, -2, -2, -1, -1, 0, 0);
          i->_is_root = true;
          
          displayNode4i5(*i);
          i->erase(1);
          displayNode4i5(*i);
          i->erase(1);
          displayNode4i5(*i);
          i->erase(0);
          displayNode4i5(*i);
}

int main() {
          //tree_test();
          // element_test();
          node_test();
}