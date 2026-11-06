#include <iomanip>
#include <iostream>
#include <mutex>
#include <omp.h>

using namespace std;

int main() {
  int num_steps = 100000000;
  double width = 1.0 / double(num_steps);
  double pi = 0.0;
  mutex pi_mutex;

#pragma omp parallel num_threads(4)
  {
    int num_threads = omp_get_num_threads();
    int thread_id = omp_get_thread_num();
    double sum_local = 0.0;
    for (int i = thread_id; i < num_steps; i += num_threads) {
      double x = (i + 0.5) * width;
      sum_local = sum_local + (1.0 / (1.0 + x * x));
    }
    lock_guard<mutex> lock(pi_mutex);
    pi += sum_local * 4 * width;
  }

  cout << setprecision(17) << pi << endl;
}
