// Misst die Zeit vom Programm auf Folie 9/10 für verschiedene Abstände (PAD)
// zwischen den Zählern der Threads. Aufruf: ./false_sharing_test PAD
#include <cstdlib>
#include <iostream>
#include <omp.h>

using namespace std;

int main(int argc, char *argv[]) {
  const int threads = 4;
  int pad = 1;
  if (argc > 1) {
    pad = atoi(argv[1]);
  }

  // Array am Anfang einer Cache Line ausrichten (128 Byte beim M3)
  alignas(128) static volatile long arr[threads * 64] = {};

  double start_time = omp_get_wtime();
#pragma omp parallel num_threads(threads)
  {
    const int thread_id = omp_get_thread_num();
    for (long i = 0; i < 1000000000; ++i) {
      arr[thread_id * pad] = arr[thread_id * pad] + 1;
    }
  }
  double zeit = omp_get_wtime() - start_time;

  // Abstand zwischen den Zählern in Byte
  cout << "PAD " << pad << " (" << pad * sizeof(long) << " Byte Abstand): " << zeit
       << " Sekunden\n";
}
