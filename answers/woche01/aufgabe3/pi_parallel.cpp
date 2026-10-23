#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>
#include <omp.h>

using namespace std;

double pi_parallel(int num_steps, int num_threads) {
  double width = 1.0 / double(num_steps);
  vector<double> teilsummen(num_threads, 0.0); // eine Teilsumme pro Thread

  omp_set_num_threads(num_threads);
#pragma omp parallel
  {
    int id = omp_get_thread_num();
    int anzahl = omp_get_num_threads();

    // Bereich [start, ende) für diesen Thread
    int start = (long long)num_steps * id / anzahl;
    int ende = (long long)num_steps * (id + 1) / anzahl;

    double sum = 0.0;
    for (int i = start; i < ende; i++) {
      double x = (i + 0.5) * width;
      sum += 1.0 / (1.0 + x * x);
    }
    teilsummen[id] = sum; // jeder Thread schreibt nur einmal in sein eigenes Feld
  }

  // Teilsummen nach der parallelen Region der Reihe nach addieren,
  // dadurch braucht man kein atomic oder critical
  double sum = 0.0;
  for (double s : teilsummen) {
    sum += s;
  }
  return sum * 4 * width;
}

int main() {
  int num_steps = 100000000;
  int max_threads = omp_get_max_threads();

  // Threadanzahlen, die getestet werden: 1, 2, 4, ... und am Ende die maximale Anzahl
  vector<int> threads;
  for (int t = 1; t < max_threads; t *= 2) {
    threads.push_back(t);
  }
  threads.push_back(max_threads);

  double zeit_1_thread = 0.0;
  cout << "Threads  pi                   Abweichung  Zeit [s]  Speedup\n";
  for (int t : threads) {
    // jede Messung 5 mal machen und die beste Zeit nehmen, weil einzelne Messungen schwanken
    double pi = 0.0;
    double run_time = 1e9;
    for (int wdh = 0; wdh < 5; wdh++) {
      double start_time = omp_get_wtime();
      pi = pi_parallel(num_steps, t);
      run_time = min(run_time, omp_get_wtime() - start_time);
    }
    if (t == 1) {
      zeit_1_thread = run_time;
    }

    cout << setw(7) << t << "  " << setprecision(17) << fixed << pi << "  "
         << setprecision(1) << scientific << abs(pi - M_PI) << "     "
         << setprecision(4) << fixed << run_time << "    "
         << setprecision(2) << zeit_1_thread / run_time << "\n";
  }
}
