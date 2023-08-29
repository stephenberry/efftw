
#include "efftw/efftw.hpp"

#include <chrono>
#include <iostream>

int main() {
   constexpr auto N = 4096;
   
   efftw::init_threads(4);
   
   Eigen::MatrixXcd mat;
   mat.resize(N, N);
   
   std::mt19937_64 g{};
   
   std::uniform_real_distribution<double> dist{0.0, 1.0};
   
   for (size_t r = 0; r < N; ++r) {
      for (size_t c = 0; c < N; ++c) {
         mat(r, c) = { dist(g), dist(g) };
      }
   }
   
   efftw::f2 fft{mat};
   
   auto t0 = std::chrono::steady_clock::now();
   fft();
   auto t1 = std::chrono::steady_clock::now();
   auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() * 1e-6;
   
   std::cout << duration << '\n';
   
   return 0;
}
