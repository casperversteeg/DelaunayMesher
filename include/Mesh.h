#ifndef MESH_H
#define MESH_H
#include "Body.h"
#include "Node.h"
#include "Triangulation.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

class Mesh : public Body {
private:
  std::vector<Node *> body;
  std::vector<Node *> nodes;
  Triangulation *T;
  double x_size; // actual value
  double y_size;
  // Special errors
  class noMesh : std::exception {
    const char *what() { return "No mesh has been built\n"; };
  };
  class noBody : std::exception {
    const char *what() { return "Need a body to mesh\n"; };
  };

protected:
  void createGrid(std::vector<Node *> &,
                  bool = false); // Creates evenly spaced nodes
public:
  // Constructors
  Mesh() : body(0), nodes(0), T(nullptr), x_size(0), y_size(0){};
  Mesh(Body &);
  Mesh(const Mesh &); // Copy
  // Destructor
  ~Mesh();
  // Operators
  Mesh &operator=(const Mesh &);     // Assignment
  Node *operator[](int);             // Index nodes
  const Node *operator[](int) const; // Index nodes const
  // Public methods
  void mesh();                  // Mesh input body
  void printMesh();             // Print mesh to stdout
  void printMesh(const char *); // Print mesh to file
  unsigned size() const;        // Size of the mesh
  void Delaunay();              // Delaunay meshes the domain
  void randomize();             // Pseudo-randomly moves node points
};

#endif /*__MESH_H__*/
