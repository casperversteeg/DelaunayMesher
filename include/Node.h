#ifndef NODE_H
#define NODE_H
#include "Body.h"
#include "Edge.h"
#include <assert.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <utility>
#include <vector>
#define INFTY 100000 // Some large number

class Edge; // Forward declaration

class Node {
private:
  friend class Body;
  static unsigned nextNodeID;
  unsigned nodeID;
  std::vector<double> coords;
  std::vector<Edge *> edges;

protected:
public:
  // Constructors
  Node() : nodeID(nextNodeID), coords(0, 0), edges(0, nullptr) { nextNodeID++; }
  Node(double, double);
  Node(const Node &); // Copy
  // Destructor
  virtual ~Node();
  // Operators
  Node &operator=(const Node &);      // Assignment
  double operator[](int);             // Coordinate index
  const double operator[](int) const; // Coordinate index const
  // Public methods
  unsigned getID() const;        // Returns nodeID;
  void connect(Node *);          // Connect this node to other node by edge
  void disconnect(Node *);       // Destroys edge that links nodes
  bool isConnected(Node *);      // Checks if node connected to another
  double distanceTo(Node *);     // Shortest distance to other node
  double distanceTo(Edge *);     // Perpendicular distance to edge
  bool isOnEdge(Node *, Node *); // Finds if this on lin interpolant of others
  std::vector<Edge *>
  sourceNode() const; // Returns all edges originating from this
};

// overloaded operators in std relevant to this class
std::ostream &operator<<(std::ostream &, const Node *); // Out stream
std::vector<double> &operator*(std::vector<double> &v,
                               double c); // scalar multiplication of vectors
std::vector<double> &operator+(std::vector<double> &v1,
                               std::vector<double> &v2); // Vector addition
std::vector<double> &operator-(std::vector<double> &v1,
                               std::vector<double> &v2); // Vector subtract

#endif /*__NODE_H__*/
