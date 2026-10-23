#include <iomanip>
#include <iostream>
#include <omp.h>
#include <random>
#include <vector>

using namespace std;

// Parallele Version vom Monte Carlo Programm (Folie 21).
// Jeder Thread bekommt seinen eigenen Zufallsgenerator, weil sich die Threads
// sonst einen Generator teilen würden (Data Race und gleiche Zufallszahlen).
double pi_monte_carlo(int n, int num_threads, unsigned int seed) {
  int counter = 0;

  omp_set_num_threads(num_threads);
#pragma omp parallel reduction(+ : counter)
  {
    int id = omp_get_thread_num();
    int anzahl = omp_get_num_threads();

    // Seed aus seed und Thread-ID bauen. Einfach seed + id geht nicht gut,
    // weil bei default_random_engine die Seeds 0 und 1 dieselben Zahlen liefern
    // (siehe seed_test.cpp)
    seed_seq seq{seed, (unsigned int)id};
    default_random_engine re{seq};
    uniform_real_distribution<double> zero_to_one{0.0, 1.0};

    // jeder Thread erzeugt seinen Teil der n Punkte
    int start = (long long)n * id / anzahl;
    int ende = (long long)n * (id + 1) / anzahl;

    for (int i = start; i < ende; ++i) {
      auto x = zero_to_one(re);
      auto y = zero_to_one(re);
      if (x * x + y * y <= 1.0) {
        ++counter; // counter ist durch reduction für jeden Thread eine eigene Kopie
      }
    }
  } // hier werden die Kopien von counter automatisch zusammenaddiert

  return 4 * (double(counter) / n);
}

int main() {
  int n = 100000000; // Anzahl der Punkte
  unsigned int seed = 0;
  int max_threads = omp_get_max_threads();

  vector<int> threads;
  for (int t = 1; t < max_threads; t *= 2) {
    threads.push_back(t);
  }
  threads.push_back(max_threads);

  double zeit_1_thread = 0.0;
  cout << "Threads  pi          Zeit [s]  Speedup\n";
  for (int t : threads) {
    // 3 Wiederholungen und die beste Zeit nehmen
    double pi = 0.0;
    double run_time = 1e9;
    for (int wdh = 0; wdh < 3; wdh++) {
      double start_time = omp_get_wtime();
      pi = pi_monte_carlo(n, t, seed);
      run_time = min(run_time, omp_get_wtime() - start_time);
    }
    if (t == 1) {
      zeit_1_thread = run_time;
    }

    cout << setw(7) << t << "  " << setprecision(8) << fixed << pi << "  "
         << setprecision(4) << run_time << "    " << setprecision(2)
         << zeit_1_thread / run_time << "\n";
  }
}
