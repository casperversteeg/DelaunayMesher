#include "../include/Body.h"
#include "../include/Mesh.h"
//#include "../lib/matplotlib-cpp-master/matplotlibcpp.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

/* TODO:
1. Check all copy constructors and assignment constructors are done to std
2. Add references to unusual code snippets
*/

void printErrorToFile(const char *fileName, const std::exception &e) {
  std::string root = fileName;
  root = root + ".log";
  std::ofstream w(root.c_str());
  if (!(w.is_open())) {
    fprintf(stderr, "Error opening %s", root.c_str());
    exit(EXIT_FAILURE);
  }
  w << "There was an error with file `" << fileName
    << "`. The following exception was thrown: \n"
    << e.what() << "No mesh was created.\n";
  w.close();
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: mesh-generator <input file>\n");
    return EXIT_FAILURE;
  }
  for (int j = 0; j < 2; j++) { // run it twice, randomize second time
    for (int i = 1; i < argc; i++) {
      try {
        Body inputBody(argv[i]);
        Mesh meshedBody(inputBody);
        if (j == 0) {
          meshedBody.mesh();
          meshedBody.printMesh(argv[i]); // Print the mesh
        } else {
          meshedBody.randomize();
          meshedBody.printMesh(argv[i]);
        }
        meshedBody.Delaunay();
        meshedBody.printMesh(argv[i]);
      } catch (const std::exception &e) {
        printErrorToFile(argv[i], e);
        std::cout << "There was an error with file `" << argv[i]
                  << "`. The following exception was thrown: \n"
                  << e.what() << "No mesh was created.\n"
                  << std::endl;
        continue;
      }
    }
  }
  return EXIT_SUCCESS;
}
