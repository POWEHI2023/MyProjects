#include "BPTree/bp.cpp"

#include <gtest/gtest.h>

template <typename T>
void display(const std::vector<T>& arr) {
          for (auto x : arr) std::cout << x << " ";
          std::cout << std::endl;
}

void node_test() {
          auto displayNode4i5 = [](const Node<int, int>& node) {
                    printf("Node size: %ld, Type: %s\n", node.size(),
                              node._type == NodeType::LeafNode ? "Leaf node" : "Inner node");

                    for (int i = 0; i < node.size(); ++i) {
                              auto [k, v, t] = node[i];
                              if (t) printf("``Key: %d : Value: %d\n", k, v._v);
                              else {
                                        printf("``Key: %d, Node Size: %ld\n", k, v._n->size());
                              }
                    }
                    
          };

          auto i = Node<int, int>::create_node();
          i->insert(1, 1);
          displayNode4i5(*i);
}

void node_test2 () {
          auto displayNode4i5 = [](const Node<int, int>& node) {
                    printf("Node size: %ld, Type: %s\n", node.size(),
                              node._type == NodeType::LeafNode ? "Leaf node" : "Inner node");

                    for (int i = 0; i < node.size(); ++i) {
                              auto [k, v, t] = node[i];
                              if (t) printf("``Key: %d : Value: %d\n", k, v._v);
                              else {
                                        printf("``Key: %d, Node Size: %ld\n", k, v._n->size());
                              }
                    }
                    
          };

          auto i = Node<int, int>::create_node(NodeType::LeafNode, -2, -2, -1, -1, 0, 0);
          i->insert(1, 1);
          i->insert(2, 2);
          displayNode4i5(*i);

          auto ret = i->insert(3, 3);
          displayNode4i5(*i);

          displayNode4i5(*ret);

          auto root = Node<int, int>::create_node(NodeType::InnerNode);
          root->_is_root = true;

          root->insert(i->get_key(), i);
          root->insert(ret->get_key(), ret);
          printf("\n");
          displayNode4i5(*root);

          ret->erase(1);
          displayNode4i5(*root);
          if (root->size() == 1 && root->_type == NodeType::InnerNode) {
                    auto [k, v, t] = (*root)[0];
                    root = v._n;
                    root->_is_root = true;
          }
          displayNode4i5(*root);
}

void bp_test() {
          BPTree<int, double, 5> bp;
          bp.insert(1,1);
          bp.insert(2,2);
          bp.insert(3,3);
          bp.insert(4,4);
          bp.insert(5,5);

          auto arr = bp.serialize();
          display(arr);

          bp.insert(6,6);
          arr = bp.serialize();
          display(arr);
          bp.insert(7,7);
          arr = bp.serialize();
          display(arr);

          bp.insert(8,8.0);
          arr = bp.serialize();
          display(arr);
          bp.insert(9,9.1);
          arr = bp.serialize();
          display(arr);
          bp.insert(10,10);
          arr = bp.serialize();
          display(arr);
          bp.insert(11,11);
          arr = bp.serialize();
          display(arr);

          bp.insert(0,0);
          arr = bp.serialize();
          display(arr);

          bp.insert(-1,-1);
          arr = bp.serialize();
          display(arr);

          bp.insert(-5,-5);
          arr = bp.serialize();
          display(arr);

          bp.insert(-3,-3);
          arr = bp.serialize();
          display(arr);

          bp.insert(12,12);
          arr = bp.serialize();
          display(arr);

          bp.insert(13,13);
          arr = bp.serialize();
          display(arr);
          bp.insert(14,14);
          arr = bp.serialize();
          display(arr);

          for (int i = 15; i < 100; ++i) {
                    bp.insert(i,i);
                    arr = bp.serialize();
                    display(arr);
          }
}

void bp_test2() {
          BPTree<std::string, double, 10> bp1;
          int pos = 150;

          for (int i = 0; i < pos; ++i) {
                    printf("%d\n", i);
                    bp1.insert(std::to_string(i), i);
                    auto arr = bp1.serialize();
                    display(arr);
          }
          bp1.insert(std::to_string(pos), pos);
}

void bp_test3() {
          BPTree<std::string, double, 10> bp1;
          int pos = 10;

          for (int i = 0; i < pos; ++i) {
                    printf("%d\n", i);
                    bp1.insert(std::to_string(i), i);
                    auto arr = bp1.serialize();
                    display(arr);
          }

          
}

int main() {
          //tree_test();
          // element_test();
          // node_test();
          // node_test2();
          // bp_test();
          // bp_test2();
          bp_test3();
}