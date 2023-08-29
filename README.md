# Eigen-FFTW (EFFTW)
EFFTW is a modern C++20 wrapper library around [FFTW](http://www.fftw.org) for [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page).

- Supports 1D and 2D FFTs

## Example

```c++
#include "efftw/efftw.hpp"

int main()
{
   // init_threads must be called before the library is used
   efftw::init_threads(4); // set FFTW to use 4 threads

   // build a complex matrix
   Eigen::MatrixXcd mat(N, N);

   std::mt19937_64 g{}; // random number generator
   std::uniform_real_distribution<double> dist{0.0, 1.0};

   for (size_t r = 0; r < N; ++r) {
      for (size_t c = 0; c < N; ++c) {
         mat(r, c) = { dist(g), dist(g) };
      }
   }
   
   efftw::f2 fft{mat}; // FFTW planning on construction (may be reused)
   fft(); // compute the FFT of mat in place
}
```

## API

```c++
using namespace efftw;
// classes
f1{vec} // 1D forward FFT (not normalized)
f2{mat} // 2D forward FFT
i1{vec} // 1D inverse FFT (1/(rows) normalization)
i2{mat} // 2D inverse FFT (1/(rows * cols) normalization)

// functions
shift1(vec) // 1D forward FFT shift
shift2(mat) // 2D forward FFT shift
ishift1(vec) // 1D inverse FFT shift
ishift2(mat) // 2D inverse FFT shift
```

## Important!

EFFTW classes take references to Eigen types. Do not delete the matrix or resize it without rebuilding the EFFTW class.

The EFFTW classes maintain the FFTW plan, which is deleted in the EFFTW class destructors. The classes are used to keep the plan alive and allow the same matrix memory to be used multiple times. It is inefficient to rebuild the plan, but the plan needs to be rebuilt if the size of the matrix or vector changes.

## Alias Type Deduction

```c++
// For clang, alias type deduction is not yet supported, so you will need to write:
efftw::f2<decltype(mat)> fft(mat);
```

