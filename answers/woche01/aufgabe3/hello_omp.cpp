// Folie 14, OpenMP Version
#include <iostream>

int main() {
#pragma omp parallel num_threads(4)
  { std::cout << "Hello World!" << std::endl; }
}
