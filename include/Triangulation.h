#ifndef TRIANGULATION_H
#define TRIANGULATION_H
#include "Edge.h"
#include "Element.h"
#include "Node.h"
#include <algorithm>
#include <assert.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

class Edges;
class Node; // Forward declarations

class Triangulation {
private:
  std::vector<Node *> nodes;
  std::vector<Element *> elements;
  bool DelaunayFlag;
  bool randFlag;

  class overlappingEdge : std::exception {
    const char *what() { return "Edges in element overlap"; };
  };

protected:
  void push(Node *);        // Adds node to mesh
  void buildOutsideEdges(); // connects all the outside nodes, i.e. nodes[1-4]
  void addFirstNode();      // First node creates first elements
  void triangulate();       // Builds elements from nodes
  double minimumInteriorAngle(Element *, Element *); // Finds min inter'r angle
  std::vector<std::vector<Node *>> tryDelaunay(Element *, Element *);
  void buildDiagonal(); // Special case if number of nodes == 4
  void initAdjacents(); // Finds all the adjacent elements to all current elems

public:
  // Constructors
  Triangulation()
      : nodes(0), elements(0), DelaunayFlag(false), randFlag(false) {}
  Triangulation(std::vector<Node *> &);
  Triangulation(const Triangulation &); // Copy
  // Destructor
  ~Triangulation();
  // Operators
  Triangulation &operator=(const Triangulation &); // Assignment
  // Public methods
  std::vector<Node *> getNodes() const;   // Returns nodes in this
  std::vector<Element *> getElem() const; // Returns elements in this
  void printMesh();                       // Print mesh to stdout
  void printMesh(const char *);           // Print mesh to file
  void Delaunay();                        // Delaunay-ifies the mesh
  bool isDelaunay() const;                // Has Delaunay triang been performed
  void setRandFlag(bool);                 // Set if nodes been randomized
  bool isRandom() const;                  // Have nodes been randomized
};

#endif /* __TRIANGULATION_H__ */
