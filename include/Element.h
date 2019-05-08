#ifndef ELEMENT_H
#define ELEMENT_H
#include "Body.h"
#include "Edge.h"
#include "Node.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <vector>

class Node;
class Edge; // Forward declarations

class Element {
private:
  friend class Body;
  static unsigned nextElementID;
  unsigned elementID;
  std::vector<Node *> vertices;
  std::vector<Edge *> edges;
  std::vector<Element *> adjacent;

protected:
  void getEdges();
  Element *splitAtEdge(Node *, Node *, Node *); // Split element at edge

public:
  // Constructors
  Element() : elementID(nextElementID), vertices(0) { nextElementID++; };
  Element(std::vector<Node *>);
  Element(const Element &); // Copy
  // Destructor
  ~Element();
  // Operators
  Element &operator=(const Element &); // Assignment
  Node *operator[](int);               // Indexing nodes
  const Node *operator[](int) const;   // Indexing node const
  // Public methods
  std::vector<Element *> split(Node *);    // Split element by internal node
  unsigned getID() const;                  // Return elementID
  std::vector<Node *> getVertices() const; // return vertices of this element
  bool isVertex(Node *);                   // Is node a vertex of this element
  bool containsNode(Node *);               // Is node inside the element
  bool nodeIsOnEdge(Node *);               // Is node on edge of element
  bool overlaps(Edge *);                  // Does edge overlap with edge of elem
  Node *findOppositeNode(Node *, Node *); // Finds node opposite edge
  bool shareEdge(Element *);              // Finds the edge these elems share
  std::vector<Node *> findSharedNodes(Element *); // Finds nodes they share
  std::vector<Edge *> sourceNode(int);  // Finds edges in this elem from Node
  void redefine(std::vector<Node *> &); // Redefines this element
  std::vector<double> findCoordinates(Edge *, Edge *, Edge *);
  std::vector<Element *> getAdjacent();       // Return adjacent
  void setAdjacent(std::vector<Element *> &); // Update adjacent
  void fixAdjacency(std::vector<Element *> &,
                    std::vector<Element *> &); // Update adjacent
  std::vector<Element *>
  getFringe(std::vector<Element *> &); // get elems that border a set
};

std::ostream &operator<<(std::ostream &, const Element *); // Out stream

#endif /*__ELEMENT_H__*/
