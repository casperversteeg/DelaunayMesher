#include "../include/Mesh.h"

// Constructors
Mesh::Mesh(Body &inBody) {
  body = inBody.getBoundingPoints();
  x_size = inBody.getXsize();
  y_size = inBody.getYsize();
  T = nullptr;
}

Mesh::Mesh(const Mesh &rhs) {
  for (unsigned i = 0; i < rhs.size(); i++) {
    nodes.push_back(new Node(*(rhs[i])));
  }
}

// Destructor
Mesh::~Mesh() {
  for (unsigned i = 0; i < nodes.size(); i++) {
    delete nodes[i];
  }
  delete T;
}

// Operators
Mesh &Mesh::operator=(const Mesh &rhs) {
  if (&rhs != this) {
    Mesh temp = rhs;
    std::vector<Node *> n = nodes;
    nodes = temp.nodes;
    temp.nodes = n;
  }
  return *this;
}

Node *Mesh::operator[](int index) { return nodes[index]; }

const Node *Mesh::operator[](int index) const { return nodes[index]; }

// Public methods
void Mesh::mesh() {
  if (body.size() == 0) {
    throw noBody();
  }
  // Generate all mesh points in the body, filter out existing bounding nodes
  for (unsigned i = 0; i < body.size(); i++) {
    nodes.push_back(body[i]);
  }
  createGrid(nodes);
  // Form triangulation per the prescribed algorithm
  T = new Triangulation(nodes);
}

void Mesh::printMesh() {
  if (nodes.size() != 0 || T != nullptr) {
    T->printMesh();
  } else {
    throw noMesh();
  }
}

void Mesh::printMesh(const char *outFile) {
  if (nodes.size() != 0 || T != nullptr) {
    T->printMesh(outFile);
  } else {
    throw noMesh();
  }
}

unsigned Mesh::size() const { return nodes.size(); }

void Mesh::Delaunay() {
  if (nodes.size() != 0 || T != nullptr) {
    T->Delaunay();
  } else {
    throw noMesh();
  }
}

void Mesh::randomize() {
  srand(time(NULL));
  for (unsigned i = 0; i < body.size(); i++) {
    nodes.push_back(body[i]);
  }
  createGrid(nodes, true);
  T = new Triangulation(nodes);
  T->setRandFlag(true);
}

// Protected methods
void Mesh::createGrid(std::vector<Node *> &vertices, bool randFlag) {
  // Divide domain up into side lengths
  double x_rand = 0;
  double y_rand = 0;
  double x_min = (*(vertices[0]))[0];
  double y_min = (*(vertices[0]))[1];
  int numNodesX = ceil(((*(vertices[1]))[0] - x_min) / x_size);
  x_size = ((*(vertices[1]))[0] - (*(vertices[0]))[0]) / numNodesX;
  int numNodesY = ceil(((*(vertices[2]))[1] - y_min) / y_size);
  // using ceil ensures edges smaller than max x/y_size
  y_size = ((*(vertices[2]))[1] - (*(vertices[0]))[1]) / numNodesY;
  for (int j = 0; j <= numNodesX; j++) {   // x-nodes builder
    for (int k = 0; k <= numNodesY; k++) { // y-nodes builder
      if (!((((j == 0 || j == numNodesX) && k == 0)) ||
            ((j == 0 || j == numNodesX) && k == numNodesY))) {
        if (randFlag) {
          x_rand = (double)(rand() % 20) / 100 - 0.1;
          y_rand = (double)(rand() % 20) / 100 - 0.1;
        }
        if (j == 0 || j == numNodesX) {
          x_rand = 0;
        }
        if (k == 0 || k == numNodesY) {
          y_rand = 0;
        }
        nodes.push_back(new Node(x_size * (j + x_rand) + x_min,
                                 y_size * (k + y_rand) + y_min));
      }
    }
  }
}
