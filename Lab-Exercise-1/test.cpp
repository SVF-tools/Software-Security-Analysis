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
*        1 (11)          2 (12)
*        |               |
*     (Store)         (Store)
*        |               |
*        v               v
*        3 (13)          4 (14)
*        |               |
*     (Copy)          (Copy)
*        |               |
*        v               v
*        5 <--        -->6
*        |   |        |  |
*        |   |        | (Load)
*        | (Store)    |  v
*     (Load) |--------|--8
*        v            |
*        7-(Store)----|
 * */
// init nodes
    CGNode *node1 = new CGNode(1);
    CGNode *node2 = new CGNode(2);
    CGNode *node3 = new CGNode(3);
    CGNode *node4 = new CGNode(4);
    CGNode *node5 = new CGNode(5);
    CGNode *node6 = new CGNode(6);
    CGNode *node7 = new CGNode(7);
    CGNode *node8 = new CGNode(8);

    CGNode *node11 = new CGNode(11);
    CGNode *node12 = new CGNode(12);
    CGNode *node13 = new CGNode(13);
    CGNode *node14 = new CGNode(14);

    // init Graph
    CGraph *g = new CGraph();
    g->addNode(node1);
    g->addNode(node2);
    g->addNode(node3);
    g->addNode(node4);
    g->addNode(node5);
    g->addNode(node6);
    g->addNode(node7);
    g->addNode(node8);

    g->addNode(node11);
    g->addNode(node12);
    g->addNode(node13);
    g->addNode(node14);

    // init edges
    g->addEdge(node1, node3, CGEdge::STORE);
    g->addEdge(node3, node5, CGEdge::COPY);
    g->addEdge(node5, node7, CGEdge::LOAD);
    g->addEdge(node2, node4, CGEdge::STORE);
    g->addEdge(node4, node6, CGEdge::COPY);
    g->addEdge(node6, node8, CGEdge::LOAD);
    g->addEdge(node8, node5, CGEdge::STORE);
    g->addEdge(node7, node6, CGEdge::STORE);

    g->addInclusionNode(node1, node11);

    g->addInclusionNode(node1, node11);
    g->addInclusionNode(node2, node12);
    g->addInclusionNode(node3, node13);
    g->addInclusionNode(node4, node14);

    g->pushIntoWorklist(1);
    g->pushIntoWorklist(2);
    g->pushIntoWorklist(3);
    g->pushIntoWorklist(4);
    g->constraintSolving(g);

    std::map<unsigned, std::set<unsigned>> result = {
            {1, {11}},            {2,  {12}},
            {3,  {13}},
            {4,  {14}},
            {5,  {13}},
            {6,  {14}},
            {7,  {11, 12}},
            {8,  {11, 12}},
    };
    for (unsigned id = 1; id <= 8; id++) {
        assert(result[id] == g->getInclusionSet(id) && "Your result is not correct!");
    }
}

/// Entry of the program
int main() {
    Test1();
    Test2();
    return 0;
}