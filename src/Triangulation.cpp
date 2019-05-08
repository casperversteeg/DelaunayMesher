#include "../include/Triangulation.h"

// Constructors
Triangulation::Triangulation(std::vector<Node *> &nodeGrid) {
  nodes = nodeGrid;
  triangulate();
  DelaunayFlag = false;
  randFlag = false;
}

Triangulation::Triangulation(const Triangulation &rhs) {
  nodes = rhs.getNodes();
  elements = rhs.getElem();
  DelaunayFlag = rhs.isDelaunay();
  randFlag = rhs.isRandom();
}

// Destructor
Triangulation::~Triangulation() {
  for (unsigned i = 0; i < elements.size(); i++) {
    delete elements[i];
  }
}

// Operators
Triangulation &Triangulation::operator=(const Triangulation &rhs) {
  if (&rhs != this) {
    Triangulation temp = rhs;
    std::vector<Node *> n = nodes;
    std::vector<Element *> e = elements;
    bool d = DelaunayFlag;
    nodes = temp.nodes;
    elements = temp.elements;
    DelaunayFlag = temp.DelaunayFlag;
    temp.nodes = n;
    temp.elements = e;
    temp.DelaunayFlag = d;
  }
  return *this;
}

// Public methods
std::vector<Node *> Triangulation::getNodes() const { return nodes; }

std::vector<Element *> Triangulation::getElem() const { return elements; }

void Triangulation::printMesh() {
  std::cout << "$nodes" << std::endl;
  for (unsigned i = 0; i < nodes.size(); i++) {
    std::cout << nodes[i] << std::endl;
  }
  std::cout << "$elements" << std::endl;
  for (unsigned j = 0; j < elements.size(); j++) {
    std::cout << elements[j] << std::endl;
  }
}

void Triangulation::printMesh(const char *outFile) {
  std::string root = outFile;
  if (randFlag) {
    root = root + ".rnd";
  }
  if (DelaunayFlag) {
    root = root + ".del.msh";
  } else {
    root = root + ".msh";
  }
  std::ofstream w(root.c_str());
  if (!(w.is_open())) {
    fprintf(stderr, "Error opening %s", root.c_str());
    exit(EXIT_FAILURE);
  }
  w << "$nodes\n";
  for (unsigned i = 0; i < nodes.size(); i++) {
    std::ostringstream s;
    s << nodes[i];
    std::string nodeString = s.str();
    replace(nodeString.begin(), nodeString.end(), ' ', ',');
    w << nodeString << std::endl;
  }
  w << "$elements\n";
  for (unsigned j = 0; j < elements.size(); j++) {
    std::ostringstream s;
    s << elements[j];
    std::string elementString = s.str();
    replace(elementString.begin(), elementString.end(), ' ', ',');
    w << elementString << std::endl;
  }
  w.close();
}

void Triangulation::Delaunay() {
  bool keepRunningFlag;
  do {
    keepRunningFlag = false;
    for (unsigned i = 0; i < elements.size(); i++) {
      std::vector<Element *> adj = elements[i]->getAdjacent();
      for (unsigned j = 0; j < adj.size(); j++) {
        double minInt = minimumInteriorAngle(elements[i], adj[j]);
        // If the new min interior angle is greater than the prev one, make
        // hypothetical elements the real ones
        std::vector<std::vector<Node *>> newElem;
        try {
          newElem = tryDelaunay(adj[j], elements[i]);
          Element *hyp1 = new Element(newElem[0]);
          Element *hyp2 = new Element(newElem[1]);
          if ((minInt < minimumInteriorAngle(hyp1, hyp2))) {
            std::vector<Element *> oldElems = {adj[j], elements[i]};
            std::vector<Element *> thePool = elements[i]->getFringe(oldElems);
            for (unsigned m = 0; m < thePool.size(); m++) {
              if (thePool[m] == adj[j] || thePool[m] == elements[i]) {
                thePool.erase(thePool.begin() + m);
              }
            }
            keepRunningFlag = true;
            elements[i]->redefine(newElem[0]);
            adj[j]->redefine(newElem[1]);
            std::vector<Element *> updatedElem = {adj[j], elements[i]};
            elements[i]->fixAdjacency(updatedElem, thePool);
            delete hyp1;
            delete hyp2;
            break;
          } else {
            delete hyp1;
            delete hyp2;
          }
        } catch (...) {
          continue;
        }
      }
    }
  } while (keepRunningFlag);
  DelaunayFlag = true;
}

bool Triangulation::isDelaunay() const { return DelaunayFlag; }

void Triangulation::setRandFlag(bool what) { randFlag = what; }

bool Triangulation::isRandom() const { return randFlag; }

// Protected methods
void Triangulation::push(Node *newNode) {
  if (elements.size() == 0) {
    throw std::invalid_argument("No elements created. Cannot push node.\n");
  }
  for (unsigned i = 0; i < elements.size(); i++) {
    if (elements[i]->nodeIsOnEdge(newNode) ||
        elements[i]->containsNode(newNode)) {
      std::vector<Element *> newElem = elements[i]->split(newNode);
      for (unsigned j = 0; j < newElem.size(); j++) {
        elements.push_back(newElem[j]);
      }
      break;
    }
  }
}

void Triangulation::buildOutsideEdges() {
  nodes[0]->connect(nodes[1]);
  nodes[1]->connect(nodes[3]);
  nodes[3]->connect(nodes[2]);
  nodes[2]->connect(nodes[0]);
}

void Triangulation::addFirstNode() {
  // First node will always be on the edges coming from nodes[0]
  std::vector<Edge *> toCheck = nodes[0]->sourceNode();
  for (unsigned i = 0; i < toCheck.size(); i++) {
    if (toCheck[i]->crossesNode(nodes[4])) {
      if (toCheck[i]->getSink() == nodes[1]) {
        toCheck[i]->split(nodes[4]);
        nodes[4]->connect(nodes[2]);
        nodes[4]->connect(nodes[3]);
        std::vector<Node *> el1 = {nodes[0], nodes[2], nodes[4]};
        std::vector<Node *> el2 = {nodes[4], nodes[2], nodes[3]};
        std::vector<Node *> el3 = {nodes[1], nodes[3], nodes[4]};
        elements.push_back(new Element(el1));
        elements.push_back(new Element(el2));
        elements.push_back(new Element(el3));
      } else {
        toCheck[i]->split(nodes[4]);
        nodes[4]->connect(nodes[1]);
        nodes[4]->connect(nodes[3]);
        std::vector<Node *> el1 = {nodes[0], nodes[1], nodes[4]};
        std::vector<Node *> el2 = {nodes[4], nodes[1], nodes[3]};
        std::vector<Node *> el3 = {nodes[2], nodes[3], nodes[4]};
        elements.push_back(new Element(el1));
        elements.push_back(new Element(el2));
        elements.push_back(new Element(el3));
      }
    }
  }
}

void Triangulation::triangulate() {
  buildOutsideEdges();
  if (nodes.size() == 4) {
    buildDiagonal();
    initAdjacents();
  } else {
    addFirstNode();
    initAdjacents();
    for (unsigned i = 5; i < nodes.size(); i++) {
      push(nodes[i]);
    }
  }
}

void Triangulation::initAdjacents() {
  for (unsigned i = 0; i < elements.size(); i++) {
    std::vector<Element *> ans;
    for (unsigned j = 0; j < elements.size(); j++) {
      if (elements[i] != elements[j] && elements[i]->shareEdge(elements[j])) {
        ans.push_back(elements[j]);
      }
    }
    elements[i]->setAdjacent(ans);
  }
}

double Triangulation::minimumInteriorAngle(Element *el1, Element *el2) {
  std::vector<double> angles;
  for (int i = 0; i < 3; i++) {
    std::vector<Edge *> e1 = el1->sourceNode(i);
    std::vector<Edge *> e2 = el2->sourceNode(i);
    double angle1 = *(e1[0]) > e1[1];
    double angle2 = *(e2[0]) > e2[1];
    angles.push_back(std::min(angle1, angle2));
  }
  return *std::min_element(angles.begin(), angles.end());
}

std::vector<std::vector<Node *>> Triangulation::tryDelaunay(Element *adj,
                                                            Element *ele) {
  std::vector<Node *> sharedNodes = ele->findSharedNodes(adj);
  Node *op1 = ele->findOppositeNode(sharedNodes[0], sharedNodes[1]);
  Node *op2 = adj->findOppositeNode(sharedNodes[0], sharedNodes[1]);
  // Do a hypothetical swap of the edges
  Edge *hypE = new Edge(op1, op2);
  Edge *basis1 = new Edge(op1, sharedNodes[0]);
  Edge *basis2 = new Edge(op1, sharedNodes[1]);
  std::vector<double> hypECoords = ele->findCoordinates(hypE, basis1, basis2);
  delete basis1;
  delete basis2;
  if (ele->overlaps(hypE) || adj->overlaps(hypE) || hypECoords[0] < 0 ||
      hypECoords[1] < 0) {
    delete hypE;
    throw overlappingEdge();
  }
  delete hypE;
  op1->connect(op2);
  std::vector<Node *> el1 = {op1, op2, sharedNodes[0]};
  std::vector<Node *> el2 = {op1, op2, sharedNodes[1]};
  std::vector<std::vector<Node *>> ans = {el1, el2};
  return ans;
}

void Triangulation::buildDiagonal() {
  nodes[0]->connect(nodes[3]);
  std::vector<Node *> el1 = {nodes[0], nodes[1], nodes[3]};
  std::vector<Node *> el2 = {nodes[0], nodes[2], nodes[3]};
  elements.push_back(new Element(el1));
  elements.push_back(new Element(el2));
}
