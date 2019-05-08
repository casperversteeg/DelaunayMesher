#include "../include/Body.h"

// Constructors
Body::Body(char *readFile) {
  x_size = 0;
  y_size = 0;
  try {
    preProcess(readFile);
  } catch (std::exception &e) {
    for (unsigned i = 0; i < vertices.size(); i++) {
      delete vertices[i];
    }
    throw;
  }
}

Body::Body(const Body &rhs) {
  x_size = rhs.getXsize();
  y_size = rhs.getYsize();
  vertices = rhs.getBoundingPoints();
}

// Destructor
Body::~Body() {
  // Need to reset these counters every time a body destroys itself.
  Node::nextNodeID = 1;
  Element::nextElementID = 1;
}

// Operators
Body &Body::operator=(const Body &rhs) {
  if (&rhs != this) {
    Body temp = rhs;
    double x = x_size;
    double y = y_size;
    std::vector<Node *> v = vertices;
    x_size = temp.x_size;
    y_size = temp.y_size;
    vertices = temp.vertices;
    temp.x_size = x;
    temp.y_size = y;
    temp.vertices = v;
  }
  return *this;
}

// Public methods
double Body::getXsize() const { return x_size; }

double Body::getYsize() const { return y_size; }

std::vector<Node *> Body::getBoundingPoints() const { return vertices; }

// Protected methods
void Body::preProcess(char *inputFile) {
  std::vector<std::string> fileContents = readFile(inputFile);
  isValid(fileContents);
  std::vector<std::vector<double>> fileData = formatData(fileContents);
  isValid(fileData);
  buildMe(fileData);
}

std::vector<std::string> Body::readFile(char *fileName) {
  // Make input file stream -- check if opening is successful
  std::ifstream f(fileName);
  if (!f.is_open()) {
    throw std::runtime_error("There was an error opening file.\n");
  }
  // Read each line of input file as string
  std::stringstream *buffer = new std::stringstream();
  (*buffer) << f.rdbuf();
  std::string to;
  std::vector<std::string> fileContents;
  while (std::getline((*buffer), to, '\n')) {
    fileContents.push_back(to);
  }
  delete buffer;
  // Close file, return contents
  f.close();
  return fileContents;
}

void Body::isValid(std::vector<std::string> &fileContents) {
  // just based on file contents
  // Check that number of lines is correct (must be 6)
  if (fileContents.size() != 6) {
    throw std::runtime_error(
        "Input file length not expected. Must be 6 lines long.\n");
  }
}

void Body::isValid(std::vector<std::vector<double>> &fileData) {
  // actually have numbers here
  // Check structure is correct lines 1 & 2 must contain only 1 number
  if (fileData[0].size() != 1 || fileData[1].size() != 1) {
    throw std::runtime_error(
        "Must enter only one number for element x-size and y-size\n");
  }
  for (unsigned i = 2; i < fileData.size(); i++) {
    if (fileData[i].size() != 3) {
      throw std::runtime_error(
          "Bounding node definition must take form (int)ID (double)x_position "
          "(double)y_position\n");
    } else {
      if ((int)fileData[i][0] <= 0 || (int)fileData[i][0] > 4) {
        throw std::runtime_error(
            "Bounding node ID must start at 1 and end at 4\n");
      } else if (((int)fileData[i][0]) / fileData[i][0] != 1) {
        throw std::runtime_error("Bounding node must have an integer ID\n");
      }
    }
  }
  // Check that the ordering is correct by index, not order of
  // appearance (including numbering are ints)
  std::vector<std::vector<double>> temp;
  temp.resize(4);
  for (unsigned j = 2; j < fileData.size(); j++) {
    temp[(int)fileData[j][0] - 1] = fileData[j];
  }
  // Check that the nodes actually define a rectangle (not inverted, and not
  // degenerate)
  if ((temp[0][1] != temp[2][1] || temp[1][1] != temp[3][1] ||
       temp[0][2] != temp[1][2] || temp[2][2] != temp[3][2]) ||
      (temp[1][1] - temp[0][1] <= 0) || (temp[2][2] - temp[0][2] <= 0)) {
    throw std::runtime_error("Bounding nodes do not define a rectangle, "
                             "define an inverted or degenerate shape, or are "
                             "not in the correct order.\n");
  }
  // Make the fileData array in sorted order
  for (unsigned j = 2; j < fileData.size(); j++) {
    fileData[j] = temp[j - 2];
  }
}

std::vector<std::vector<double>>
Body::formatData(std::vector<std::string> &fileContents) {
  std::vector<std::vector<double>> fileData(fileContents.size());
  try {
    for (unsigned int i = 0; i < fileContents.size(); i++) {
      std::string::size_type sz = 0; // alias of size_t
      // stod is prone to throwing exceptions if passed non-numbers or
      // invalid argument, so wrapped in try-catch to handle errors.
      while (fileContents[i].length() > 0) {
        fileData[i].push_back(std::stod(fileContents[i], &sz));
        fileContents[i] = fileContents[i].substr(sz);
        sz = 0;
      }
    }
  } catch (std::exception &e) {
    throw std::runtime_error(
        std::string("An error occured in stod(). It returned:\n") + e.what());
  }
  return fileData;
}

void Body::buildMe(std::vector<std::vector<double>> &fileData) {
  x_size = fileData[0][0];
  y_size = fileData[1][0];
  vertices.resize(fileData.size() - 2);
  for (unsigned i = 2; i < fileData.size(); i++) {
    vertices[(int)(fileData[i][0] - 1)] =
        new Node(fileData[i][1], fileData[i][2]);
  }
}
