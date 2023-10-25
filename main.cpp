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

void bp_test2() {
          BPTree<std::string, double, 10> bp1;
          int pos = 174;                // TODO：插入174时出现错误
          bp1.insert(std::to_string(1), 1);

          for (int i = 0; i < pos; ++i) {
                    printf("%d\n", i);
                    bp1.insert(std::to_string(i), i);
                    auto arr = bp1.serialize();
                    display(arr);

                    bp1.check();
          }
          bp1.insert(std::to_string(pos), pos);
          bp1.check();
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
          bp_test2();
          // bp_test3();
}