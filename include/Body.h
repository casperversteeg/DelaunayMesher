#ifndef BODY_H
#define BODY_H
#include "Element.h"
#include "Node.h"
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <utility>
#include <vector>

class Node; // forward declaration

class Body {
private:
  double x_size, y_size;
  std::vector<Node *> vertices;

protected:
  void preProcess(char *);
  std::vector<std::string> readFile(char *);
  void isValid(std::vector<std::string> &);
  void isValid(std::vector<std::vector<double>> &);
  std::vector<std::vector<double>> formatData(std::vector<std::string> &);
  void buildMe(std::vector<std::vector<double>> &);

public:
  // Constructors
  Body() : x_size(0), y_size(0), vertices(0){};
  Body(char *);
  Body(const Body &); // Copy
  // Destructor
  ~Body();
  // Operators
  Body &operator=(const Body &); // Assignment
  // Public methods
  void buildBody();                              // Make body from points
  double getXsize() const;                       // returns x_size
  double getYsize() const;                       // returns y_size
  std::vector<Node *> getBoundingPoints() const; // returns vertices
};

#endif /*__BODY_H__*/
