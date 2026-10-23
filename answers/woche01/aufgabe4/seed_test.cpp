// Test, ob verschiedene Seeds bei default_random_engine verschiedene Zahlen liefern
#include <iostream>
#include <random>

int main() {
  for (unsigned int seed = 0; seed < 3; seed++) {
    std::default_random_engine re{seed};
    std::cout << "seed " << seed << ":";
    for (int i = 0; i < 4; i++) {
      std::cout << " " << re();
    }
    std::cout << "\n";
  }
}
