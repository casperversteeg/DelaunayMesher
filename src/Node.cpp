#include "../include/Node.h"

// Initializer for member variable
unsigned Node::nextNodeID = 1;

// Constructors
Node::Node(double x, double y) {
  nodeID = nextNodeID;
  nextNodeID++;
  coords = {x, y};
}

Node::Node(const Node &rhs) {
  nodeID = nextNodeID;
  nextNodeID++;
  coords = {rhs[0], rhs[1]};
  edges = rhs.sourceNode();
}

// Destructor
Node::~Node() {
  for (unsigned i = 0; i < edges.size(); i++) {
    delete edges[i];
  }
}

// Operators
Node &Node::operator=(const Node &rhs) {
  if (&rhs != this) {
    Node temp = rhs;
    unsigned id = nodeID;
    std::vector<double> c = coords;
    std::vector<Edge *> e = edges;
    nodeID = temp.nodeID;
    coords = temp.coords;
    edges = temp.edges;
    temp.nodeID = id;
    temp.coords = c;
    temp.edges = e;
  }
  return *this;
}

double Node::operator[](int index) {
  if (index >= (int)coords.size() || index < 0) {
    throw std::invalid_argument("Index out of bounds in node\n");
  } else {
    return coords[index];
  }
}

const double Node::operator[](int index) const {
  if (index >= (int)coords.size() || index < 0) {
    throw std::invalid_argument("Index out of bounds in node\n");
  } else {
    return coords[index];
  }
}

std::ostream &operator<<(std::ostream &s, const Node *rhs) {
  s << rhs->getID() << " " << (*rhs)[0] << " " << (*rhs)[1];
  return s;
}

std::vector<double> &operator*(std::vector<double> &v, double c) {
  for (unsigned i = 0; i < v.size(); i++) {
    v[i] *= c;
  }
  return v;
}

std::vector<double> &operator+(std::vector<double> &v1,
                               std::vector<double> &v2) {
  assert(v1.size() == v2.size());
  for (unsigned i = 0; i < v1.size(); i++) {
    v1[i] += v2[i];
  }
  return v1;
}

std::vector<double> &operator-(std::vector<double> &v1,
                               std::vector<double> &v2) {
  assert(v1.size() == v2.size());
  for (unsigned i = 0; i < v1.size(); i++) {
    v1[i] -= v2[i];
  }
  return v1;
}

// Public methods
unsigned Node::getID() const { return nodeID; }

void Node::connect(Node *otherNode) {
  if (!isConnected(otherNode)) {
    edges.push_back(new Edge(this, otherNode));
  }
  if (!otherNode->isConnected(this)) {
    otherNode->connect(this);
  }
}

bool Node::isConnected(Node *otherNode) {
  for (unsigned i = 0; i < edges.size(); i++) {
    if (edges[i]->getSink() == otherNode) {
      return true;
    }
  }
  return false;
}

void Node::disconnect(Node *otherNode) {
  for (unsigned i = 0; i < edges.size(); i++) {
    if (edges[i]->getSink() == otherNode) {
      delete edges[i];
      edges.erase(edges.begin() + i);
      if (otherNode->isConnected(this)) {
        otherNode->disconnect(this);
      }
      break;
    }
  }
}

double Node::distanceTo(Node *node) {
  return sqrt(pow(coords[0] - (*node)[0], 2) + pow(coords[1] - (*node)[1], 2));
}

double Node::distanceTo(Edge *edge) {
  Edge *toSource = new Edge(this, edge->getSource());
  std::vector<double> unitToSource = toSource->normalize();
  double l = toSource->getLength();
  double parallelComp = (*edge) % unitToSource;
  if (fabs(l * parallelComp) >= edge->getLength() || parallelComp >= 0) {
    delete toSource;
    return INFTY;
  }
  std::vector<double> unitEdge = edge->normalize();
  std::vector<double> unitToEdge = unitToSource - (unitEdge * parallelComp);
  double ans = 0;
  for (unsigned i = 0; i < unitToEdge.size(); i++) {
    ans += pow(unitToEdge[i], 2);
  }
  delete toSource;
  return l * sqrt(ans);
}

bool Node::isOnEdge(Node *node1, Node *node2) {
  Edge test(node1, node2);
  if (distanceTo(&test) < 1e-15) {
    return true;
  }
  return false;
}

std::vector<Edge *> Node::sourceNode() const { return edges; }
