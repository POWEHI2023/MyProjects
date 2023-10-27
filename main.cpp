#include "source/BPlusTree/bp.h"


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

void bp_test1() {
          BPTree<std::string, double> bp;
          int pos = 1000;
          printf("BPlusTree-Test1: insert %d data (may need several seconds)\n", pos);
          for (int i = 0; i < pos; ++i) {
                    printf("Insert %d\n", i);
                    bp.insert(std::to_string(i), i);
                    display(bp.serialize());
                    bp.check();
          }
          bp.insert(std::to_string(pos), pos);
}

#define NSORTED_
#define DEBUG_
void bp_test2() {
          BPTree<int, double, 10> bp1;
          int pos = 10000000;            // 10Million?
          printf("BPlusTree-Test2: insert %d data (may need several seconds)\n", pos);
          #ifdef DEBUG
          // bp1.insert(std::to_string(1), 1);
          #endif
          for (int i = 0; i < pos; i += 1) {
                    #ifdef DEBUG
                    printf("%d\n", i);
                    // bp1.insert(std::to_string(i), i);
                    #endif

                    #ifdef NSORTED
                    for (int j = i; j < i + 6; j += 2) bp1.insert(j, j);
                    for (int j = i + 1; j < i + 6; j += 2) bp1.insert(j, j);
                    #else
                    bp1.insert(i, i);
                    // assert(bp1.find(i) != nullptr);
                    // assert(bp1[i] == i);
                    #endif

                    #ifdef DEBUG
                    display(bp1.serialize());
                    // bp1.check();
                    #endif
          }
          #ifdef DEBUG
          bp1.insert(pos, pos);
          // bp1.check();
          #endif
}

void bp_test3() {
          BPTree<std::string, double, 3> bp1;
          int pos = 10;
          bp1.insert(std::to_string(1), 1);
          printf("BPlusTree-Test3: insert %d data (may need several seconds)\n", pos);
          for (int i = 0; i < pos; ++i) {
                    printf("%d\n", i);
                    bp1.insert(std::to_string(i), i);
                    auto arr = bp1.serialize();
                    display(arr);
          }
}


#define _BP_TEST
#define _NODE_TEST
int main() {
          #ifndef _NODE_TEST
          node_test();
          #endif

          #ifndef _BP_TEST_
          bp_test1();
          bp_test2();
          bp_test3();
          #endif
}