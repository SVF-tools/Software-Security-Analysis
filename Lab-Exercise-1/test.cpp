#include "GraphAlgorithm.h"

void Test1() {
    /*


        1
       /  \
      2   3
       \ /
        4
        |
        5

  */
    // init nodes
    Node *node1 = new Node(1);
    Node *node2 = new Node(2);
    Node *node3 = new Node(3);
    Node *node4 = new Node(4);
    Node *node5 = new Node(5);

    // init edges
    Edge *edge1 = new Edge(node1, node2);
    Edge *edge2 = new Edge(node1, node3);
    node1->addOutEdge(edge1);
    node1->addOutEdge(edge2);
    Edge *edge3 = new Edge(node2, node4);
    Edge *edge4 = new Edge(node3, node4);
    node2->addOutEdge(edge3);
    node3->addOutEdge(edge4);
    Edge *edge5 = new Edge(node4, node5);
    node4->addOutEdge(edge5);

    // init Graph
    Graph *g = new Graph();
    g->addNode(node1);
    g->addNode(node2);
    g->addNode(node3);
    g->addNode(node4);
    g->addNode(node5);
    // test
    g->reachability(node1, node5);
    // print paths
    std::set<std::string> results = {"1245", "1345"};
    for (auto path : g->getPaths())
        assert(results.find(path)!= results.end() && "Your result is not correct!");
}

void Test2() {
    /*
     *   1 --(Addr)---> 2 --(Store)---> 4 --(Copy)---> 5 --(Load)---> 6
     *   3 --------(Addr)---------------^
     */
    // init nodes
    CGNode *node1 = new CGNode(1);
    CGNode *node2 = new CGNode(2);
    CGNode *node3 = new CGNode(3);
    CGNode *node4 = new CGNode(4);
    CGNode *node5 = new CGNode(5);
    CGNode *node6 = new CGNode(6);
    CGraph *g = new CGraph();
    g->addNode(node1);
    g->addNode(node2);
    g->addNode(node3);
    g->addNode(node4);
    g->addNode(node5);
    g->addNode(node6);
    g->addEdge(node1, node2, CGEdge::ADDR);
    g->addEdge(node3, node4, CGEdge::ADDR);
    g->addEdge(node2, node4, CGEdge::STORE);
    g->addEdge(node4, node5, CGEdge::COPY);
    g->addEdge(node5, node6, CGEdge::LOAD);
    g->solveWorklist();

    std::map<unsigned, std::set<unsigned>> results = {
            {2, {1}},
            {3, {1}},
            {4, {3}},
            {5, {3}},
            {6, {1}},
    };

    for (auto res: results) {
        assert(res.second == g->getInclusionSet(res.first) && "Your result is not correct!");
    }
}

/// Entry of the program
int main() {
    Test1();
    Test2();
    return 0;
}