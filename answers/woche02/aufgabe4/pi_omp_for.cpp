#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <omp.h>

using namespace std;

int main(int argc, char *argv[]) {
  int num_threads = omp_get_max_threads();
  if (argc > 1) {
    num_threads = atoi(argv[1]);
  }

  int num_steps = 100000000;
  double width = 1.0 / double(num_steps);
  double sum = 0.0;

  double start_time = omp_get_wtime();
#pragma omp parallel num_threads(num_threads)
  {
    double sum_local = 0.0;
#pragma omp for schedule(runtime)
    for (int i = 0; i < num_steps; i++) {
      double x = (i + 0.5) * width;
      sum_local = sum_local + (1.0 / (1.0 + x * x));
    }
#pragma omp atomic
    sum += sum_local;
  }
  double pi = sum * 4 * width;
  double run_time = omp_get_wtime() - start_time;

  cout << "pi with " << num_steps << " steps is " << setprecision(17) << pi << " in "
       << setprecision(6) << run_time << " seconds (" << num_threads << " threads)\n";
}
