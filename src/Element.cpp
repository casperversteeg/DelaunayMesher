#include "../include/Element.h"

// Initializer for member variable
unsigned Element::nextElementID = 1;

// Constructors
Element::Element(std::vector<Node *> nodes) {
  elementID = nextElementID;
  nextElementID++;
  vertices = nodes;
  getEdges();
}

Element::Element(const Element &rhs) {
  elementID = nextElementID;
  nextElementID++;
  vertices.resize(3);
  vertices = rhs.getVertices();
}

// Destructor
Element::~Element() {}

// Operators
Element &Element::operator=(const Element &rhs) {
  if (&rhs != this) {
    Element temp = rhs;
    unsigned id = elementID;
    std::vector<Node *> v = vertices;
    elementID = temp.elementID;
    vertices = temp.vertices;
    temp.elementID = id;
    temp.vertices = v;
  }
  return *this;
}

Node *Element::operator[](int index) {
  if (index >= (int)vertices.size() || index < 0) {
    throw std::invalid_argument("Index out of bounds in element\n");
  } else {
    return vertices[index];
  }
}

const Node *Element::operator[](int index) const {
  if (index >= (int)vertices.size() || index < 0) {
    throw std::invalid_argument("Index out of bounds in element\n");
  } else {
    return vertices[index];
  }
}

std::ostream &operator<<(std::ostream &s, const Element *rhs) {
  if (rhs->getVertices().size() == 0) {
    s << "";
  } else {
    s << rhs->getID();
    for (unsigned i = 0; i < 3; i++) {
      s << " ";
      s << (*rhs)[i]->getID();
    }
  }
  return s;
}

// Public methods
std::vector<Element *> Element::split(Node *internalNode) {
  std::vector<Element *> ans;
  for (unsigned i = 0; i < edges.size(); i++) {
    if (edges[i]->crossesNode(internalNode)) {
      Edge temp = *(edges[i]);
      bool extraSplitFlag = false;
      // Keep track of all elems adjacent to this one, and the one we change
      std::vector<Element *> oldElems = {this};
      // Split this element, add it to return vector
      Element *newElem1 =
          splitAtEdge(edges[i]->getSource(), edges[i]->getSink(), internalNode);
      ans.push_back(newElem1);
      // Find out if this node is on an internal edge
      for (unsigned j = 0; j < adjacent.size(); j++) {
        if (adjacent[j]->nodeIsOnEdge(internalNode)) {
          // If it does, we need to add the adjacent to the oldElems
          oldElems.push_back(adjacent[j]);
          Element *newElem2 = adjacent[j]->splitAtEdge(
              temp.getSource(), temp.getSink(), internalNode);
          ans.push_back(newElem2);
          extraSplitFlag = true;
          ans.push_back(adjacent[j]);
        }
      }
      std::vector<Element *> fringe = getFringe(oldElems);
      ans.push_back(this);
      fixAdjacency(ans, fringe);
      ans.pop_back();
      if (extraSplitFlag) {
        ans.pop_back();
      }
      return ans;
    }
  }
  for (unsigned m = 0; m < vertices.size(); m++) {
    internalNode->connect(vertices[m]);
  }
  std::vector<Node *> el1 = {internalNode, vertices[0], vertices[2]};
  std::vector<Node *> el2 = {internalNode, vertices[1], vertices[2]};
  ans.push_back(new Element(el1));
  ans.push_back(new Element(el2));
  vertices[2] = internalNode;
  getEdges();
  std::vector<Element *> old = {this};
  std::vector<Element *> thePool = getFringe(old);
  ans.push_back(this);
  fixAdjacency(ans, thePool);
  ans.pop_back();
  return ans;
}

unsigned Element::getID() const { return elementID; }

std::vector<Node *> Element::getVertices() const { return vertices; }

bool Element::isVertex(Node *testNode) {
  for (unsigned i = 0; i < vertices.size(); i++) {
    if (vertices[i] == testNode) {
      return true;
    }
  }
  return false;
}

bool Element::containsNode(Node *testNode) {
  if (isVertex(testNode)) {
    return false;
  }
  try {
    std::vector<Edge *> basis = sourceNode(0);
    Node *origin = basis[0]->getSource();
    Edge *testEdge = new Edge(origin, testNode);
    // Solve system using Cramer's rule
    std::vector<double> coords = findCoordinates(testEdge, basis[0], basis[1]);
    delete testEdge;
    return (sqrt(pow(coords[0], 2) + pow(coords[1], 2)) < 1 && coords[0] > 0 &&
            coords[1] > 0);
  } catch (std::overflow_error &e) {
    std::cout << "returned determinant zero" << std::endl;
    return false;
  }
}

bool Element::nodeIsOnEdge(Node *testNode) {
  for (unsigned i = 0; i < vertices.size(); i++) {
    if (testNode->isOnEdge(vertices[i], vertices[(i + 1) % vertices.size()])) {
      return true;
    }
  }
  return false;
}

Node *Element::findOppositeNode(Node *node1, Node *node2) {
  for (unsigned i = 0; i < vertices.size(); i++) {
    if (vertices[i] != node1 && vertices[i] != node2) {
      return vertices[i];
    }
  }
  return nullptr;
}

bool Element::shareEdge(Element *otherElem) {
  if (findSharedNodes(otherElem).size() > 1) {
    return true;
  }
  return false;
}

std::vector<Node *> Element::findSharedNodes(Element *otherElem) {
  std::vector<Node *> ans;
  for (unsigned i = 0; i < vertices.size(); i++) {
    if (otherElem->isVertex(vertices[i])) {
      ans.push_back(vertices[i]);
    }
  }
  return ans;
}

std::vector<Edge *> Element::sourceNode(int index) {
  if (index >= (int)vertices.size() || index < 0) {
    throw std::invalid_argument("Index out of bounds in element\n");
  }
  Node *thisVertex = vertices[index];
  std::vector<Edge *> ans;
  ans.reserve(2);
  for (unsigned i = 0; i < edges.size(); i++) {
    if (edges[i]->getSource() == thisVertex) {
      ans.push_back(edges[i]);
    } else {
    }
  }
  return ans;
}

bool Element::overlaps(Edge *testEdge) {
  for (unsigned i = 0; i < edges.size(); i++) {
    if ((*testEdge > edges[i]) < 1e-15) {
      return true;
    }
  }
  return false;
}

void Element::redefine(std::vector<Node *> &nodes) {
  vertices = nodes;
  getEdges();
}

std::vector<double> Element::findCoordinates(Edge *testEdge, Edge *basis1,
                                             Edge *basis2) {
  // Find coordinates of a vector relative to some basis in 2D using
  // Cramer's rule
  double a1 = (*basis1)[0];
  double a2 = (*basis1)[1];
  double b1 = (*basis2)[0];
  double b2 = (*basis2)[1];
  double v1 = (*testEdge)[0];
  double v2 = (*testEdge)[1];
  double det = a1 * b2 - b1 * a2;
  // Check determinant of system is not zero, else throw overflow
  if (det != 0) {
    double c1 = (v1 * b2 - b1 * v2) / det;
    double c2 = (a1 * v2 - v1 * a2) / det;
    std::vector<double> C = {c1, c2};
    return C;
  } else {
    throw std::runtime_error("Generated line element\n");
  }
}

std::vector<Element *> Element::getAdjacent() { return adjacent; }

void Element::setAdjacent(std::vector<Element *> &adj) { adjacent = adj; }

void Element::fixAdjacency(std::vector<Element *> &core,
                           std::vector<Element *> &fringe) {
  std::vector<Element *> thePool(fringe);
  for (unsigned i = 0; i < core.size(); i++) {
    thePool.push_back(core[i]);
  }
  // New elems don't have adjacents, so can build them
  for (unsigned i = 0; i < core.size(); i++) {
    std::vector<Element *> adj;
    for (unsigned j = 0; j < thePool.size(); j++) {
      if (core[i] != thePool[j] && core[i]->shareEdge(thePool[j])) {
        adj.push_back(thePool[j]);
      }
    }
    core[i]->setAdjacent(adj);
  }
  // Now update the elements in the fringe, check if their adj are valid
  for (unsigned k = 0; k < fringe.size(); k++) {
    std::vector<Element *> fringeAdj = fringe[k]->getAdjacent();
    for (unsigned m = 0; m < fringeAdj.size(); m++) {
      if (!(fringe[k]->shareEdge(fringeAdj[m]))) {
        // Search the core for which element does border it.
        for (unsigned n = 0; n < core.size(); n++) {
          if (core[n] != fringe[k] && core[n]->shareEdge(fringe[k])) {
            fringeAdj[m] = core[n];
            break;
          }
        }
        break;
      }
    }
    fringe[k]->setAdjacent(fringeAdj);
  }
}

// Protected methods
void Element::getEdges() {
  edges.clear();
  for (unsigned i = 0; i < vertices.size(); i++) {
    std::vector<Edge *> candidates = vertices[i]->sourceNode();
    for (unsigned j = 0; j < candidates.size(); j++) {
      if (candidates[j]->getSink() == vertices[(i + 1) % vertices.size()] ||
          candidates[j]->getSink() == vertices[(i + 2) % vertices.size()]) {
        edges.push_back(candidates[j]);
      }
    }
  }
}

Element *Element::splitAtEdge(Node *source, Node *sink, Node *splitNode) {
  // Node crosses this edge, need to get its nodes and see which vertices
  // they are
  Node *oppositeNode = findOppositeNode(sink, source);
  source->disconnect(sink);
  source->connect(splitNode);
  sink->connect(splitNode);
  splitNode->connect(oppositeNode);
  std::vector<Node *> newElem = {splitNode, sink, oppositeNode};
  vertices = {splitNode, source, oppositeNode};
  getEdges();
  return new Element(newElem);
}

std::vector<Element *> Element::getFringe(std::vector<Element *> &core) {
  std::vector<Element *> fringe = core[0]->getAdjacent();
  for (unsigned i = 1; i < core.size(); i++) {
    std::vector<Element *> adj = core[i]->getAdjacent();
    for (unsigned j = 0; j < adj.size(); j++) {
      fringe.push_back(adj[j]);
    }
  }
  sort(fringe.begin(), fringe.end());
  fringe.erase(unique(fringe.begin(), fringe.end()), fringe.end());
  for (unsigned k = 0; k < core.size(); k++) {
    for (unsigned m = 0; m < fringe.size(); m++) {
      if (fringe[m] == core[k]) {
        fringe.erase(fringe.begin() + m);
        break;
      }
    }
  }
  return fringe;
}
