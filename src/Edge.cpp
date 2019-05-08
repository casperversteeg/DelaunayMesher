#include "../include/Edge.h"

// Constructors
Edge::Edge(Node *setSource, Node *setSink) {
  source = setSource;
  sink = setSink;
  updateVector();
}

Edge::Edge(const Edge &rhs) {
  source = rhs.getSource();
  sink = rhs.getSink();
  length = rhs.getLength();
  for (unsigned i = 0; i < rhs.normalize().size(); i++) {
    edgeVec.push_back(rhs[i]);
  }
}

// Destructor
Edge::~Edge() {}

// Operators
Edge &Edge::operator=(const Edge &rhs) {
  if (&rhs != this) {
    Edge temp = rhs;
    std::vector<double> v = edgeVec;
    double l = length;
    Node *so = source;
    Node *si = sink;
    edgeVec = temp.edgeVec;
    length = temp.length;
    source = temp.source;
    sink = temp.sink;
    temp.edgeVec = v;
    temp.length = l;
    temp.source = so;
    temp.sink = si;
  }
  return *this;
}

double Edge::operator%(std::vector<double> &projectEdge) {
  double ans = 0;
  for (unsigned i = 0; i < edgeVec.size(); i++) {
    ans += edgeVec[i] * projectEdge[i];
  }
  return ans / length;
}

double Edge::operator%(Edge &projectEdge) {
  double ans = 0;
  for (unsigned i = 0; i < edgeVec.size(); i++) {
    ans += edgeVec[i] * projectEdge[i];
  }
  return ans / length;
}

double Edge::operator>(Edge *angleEdge) {
  if (angleEdge == this) {
    return 0;
  }
  double den = length * angleEdge->getLength();
  double num = (*this % *angleEdge) * length;
  return acos(num / den);
}

double Edge::operator[](int index) {
  if (index >= (int)edgeVec.size() || index < 0) {
    throw std::invalid_argument("Index out of bounds in edge\n");
  } else {
    return edgeVec[index];
  }
}

const double Edge::operator[](int index) const {
  if (index >= (int)edgeVec.size() || index < 0) {
    throw std::invalid_argument("Index out of bounds in edge\n");
  } else {
    return edgeVec[index];
  }
}

// Public methods
bool Edge::crossesNode(Node *testNode) {
  if (distanceTo(testNode) < 1e-15) {
    return true;
  }
  return false;
}

void Edge::split(Node *insertNode) {
  // Get both nodes that define this edge, node 1 and node 2
  Node *node1 = source; // get this source
  Node *node2 = sink;   // get this sink
  // Find edge on node 2 that connects to node 1
  std::vector<Edge *> edges2 = node2->sourceNode();
  for (unsigned i = 0; i < edges2.size(); i++) {
    if (edges2[i]->getSink() == node1) {
      edges2[i]->setSink(insertNode);
      edges2[i]->updateVector();
      break;
    }
  }
  sink = insertNode; // update sink of this edge
  insertNode->connect(node1);
  insertNode->connect(node2);
  updateVector();
}

std::vector<double> Edge::normalize() const {
  std::vector<double> ans;
  for (unsigned i = 0; i < edgeVec.size(); i++) {
    ans.push_back(edgeVec[i] / length);
  }
  return ans;
}

double Edge::distanceTo(Node *node) { return node->distanceTo(this); }

double Edge::getLength() const { return length; }

Node *Edge::getSink() const { return sink; }

Node *Edge::getSource() const { return source; }

void Edge::setSink(Node *newSink) {
  sink = newSink;
  updateVector();
}

bool Edge::isParallelTo(Edge *testEdge) {
  if (abs(*this % *testEdge) == length) {
    return true;
  }
  return false;
}

// Protected methods
void Edge::updateVector() {
  edgeVec.clear();
  edgeVec.resize(2);
  edgeVec[0] = (*sink)[0] - (*source)[0];
  edgeVec[1] = (*sink)[1] - (*source)[1];
  double l = 0;
  for (unsigned i = 0; i < edgeVec.size(); i++) {
    l += pow(edgeVec[i], 2);
  }
  length = sqrt(l);
}
