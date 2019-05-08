#ifndef EDGE_H
#define EDGE_H
#include "Node.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

class Element;
class Node; // Forward declarations

class Edge {
private:
  std::vector<double> edgeVec;
  double length;
  Node *source;
  Node *sink;

protected:
  void updateVector();

public:
  // Constructors
  Edge() : edgeVec(0), length(0), source(nullptr), sink(nullptr){};
  Edge(Node *, Node *);
  Edge(const Edge &); // Copy
  // Destructor
  ~Edge();
  // Operators
  Edge &operator=(const Edge &);           // Assignment
  double operator%(std::vector<double> &); // "Projection" operator
  double operator%(Edge &);                // "Projection" operator on edges
  double operator>(Edge *);                // "Angle" operator
  double operator[](int);                  // Indexing vector component
  const double operator[](int) const;      // Indexing vector component const
  // Public methods
  bool crossesNode(Node *);              // Does this edge intersect node
  void split(Node *);                    // Split edge at node
  std::vector<double> normalize() const; // Returns this edge normalized
  double distanceTo(Node *);             // Perpendicular distance to node
  double getLength() const;              // Returns length of this edge
  Node *getSink() const;     // Returns which node this edge points to
  Node *getSource() const;   // Returns which node this edge comes from
  void setSink(Node *);      // Sets sink of this edge to different node
  bool isParallelTo(Edge *); // Checks if this parallel to another edge
};

#endif /* __EDGE_H__ */
