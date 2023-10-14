#pragma once

#include <fftw3.h>

#include <Eigen/Dense>
#include <random>
#include <stdexcept>

namespace efftw
{
   inline void init_threads(const int n_threads)
   {
      if (fftw_init_threads() == 0) {
         throw std::runtime_error("fftw_init_threads() failed");
      }
      fftw_plan_with_nthreads(n_threads);
   }

   template <class T>
   concept is_vector = requires {
                          {
                             T::ColsAtCompileTime == 1
                          };
                       };

   template <class ComplexType>
   inline auto fftw_planner_1d(auto&&... args)
   {
      if constexpr (sizeof(ComplexType) == 16) {
         return fftw_plan_dft_1d(args...);
      }
      else {
         return fftwf_plan_dft_1d(args...);
      }
   }

   template <class ComplexType>
   inline auto fftw_planner_2d(auto&&... args)
   {
      if constexpr (sizeof(ComplexType) == 16) {
         return fftw_plan_dft_2d(args...);
      }
      else {
         return fftwf_plan_dft_2d(args...);
      }
   }

   enum class direction : int32_t { forward = FFTW_FORWARD, backward = FFTW_BACKWARD };

   template <is_vector T, direction Direction>
   struct gen1
   {
      Eigen::MatrixBase<T>& data;

      using value_type = typename Eigen::MatrixBase<T>::Scalar;
      using fftw_complex_t = std::conditional_t<sizeof(value_type) == 16, fftw_complex, fftwf_complex>;
      using fftw_plan_t = std::conditional_t<sizeof(value_type) == 16, fftw_plan, fftwf_plan>;

      gen1(Eigen::MatrixBase<T>& data) : data(data) {}
      gen1() = default;
      gen1(const gen1&) = default;
      gen1(gen1&&) = default;

      ~gen1()
      {
         if constexpr (sizeof(value_type) == 16) {
            fftw_destroy_plan(plan);
         }
         else {
            fftwf_destroy_plan(plan);
         }
      }

      void operator()()
      {
         if constexpr (sizeof(value_type) == 16) {
            fftw_execute(plan);
         }
         else {
            fftwf_execute(plan);
         }
         if constexpr (Direction == direction::backward) {
            data /= double(data.rows()); // normalize
         }
      }

     private:
      fftw_plan_t plan = fftw_planner_1d<value_type>(
         int(data.rows()), reinterpret_cast<fftw_complex_t*>(data.derived().data()),
         reinterpret_cast<fftw_complex_t*>(data.derived().data()), int(Direction), FFTW_ESTIMATE);
   };

   template <class T, direction Direction>
   struct gen2
   {
      Eigen::MatrixBase<T>& data;

      using value_type = typename Eigen::MatrixBase<T>::Scalar;
      using fftw_complex_t = std::conditional_t<sizeof(value_type) == 16, fftw_complex, fftwf_complex>;
      using fftw_plan_t = std::conditional_t<sizeof(value_type) == 16, fftw_plan, fftwf_plan>;

      gen2(Eigen::MatrixBase<T>& data) : data(data) {}
      gen2() = default;
      gen2(const gen2&) = default;
      gen2(gen2&&) = default;

      ~gen2()
      {
         if constexpr (sizeof(value_type) == 16) {
            fftw_destroy_plan(plan);
         }
         else {
            fftwf_destroy_plan(plan);
         }
      }

      void operator()()
      {
         if constexpr (sizeof(value_type) == 16) {
            fftw_execute(plan);
         }
         else {
            fftwf_execute(plan);
         }
         if constexpr (Direction == direction::backward) {
            data /= double(data.rows()) * data.cols(); // normalize
         }
      }

     private:
      fftw_plan_t plan = fftw_planner_2d<value_type>(
         int(data.rows()), int(data.cols()), reinterpret_cast<fftw_complex_t*>(data.derived().data()),
         reinterpret_cast<fftw_complex_t*>(data.derived().data()), int(Direction), FFTW_ESTIMATE);
   };

   template <class T>
   using f1 = gen1<T, direction::forward>;

   template <class T>
   using i1 = gen1<T, direction::backward>;

   template <class T>
   using f2 = gen2<T, direction::forward>;

   template <class T>
   using i2 = gen2<T, direction::backward>;

   template <is_vector T>
   inline void shift1(Eigen::MatrixBase<T>& data)
   {
      const auto size = data.size();
      const auto shift = (size + 1) / 2; // +1 to handle odd sizes
      data.segment(0, shift).swap(data.segment(shift, size - shift));
   }

   template <is_vector T>
   inline void inv_shift1(Eigen::MatrixBase<T>& data)
   {
      const auto size = data.size();
      const auto shift = (size + 1) / 2; // +1 to handle odd sizes
      data.segment(0, shift).swap(data.segment(shift, size - shift));
   }

   template <class T>
   inline void shift2(Eigen::MatrixBase<T>& data)
   {
      if (data.rows() % 2 != 0 || data.cols() % 2 != 0) {
         throw std::runtime_error("efftw::shift2 requires even sized inputs");
      }
      const auto shift_rows = data.rows() / 2;
      const auto shift_cols = data.cols() / 2;
      data.topLeftCorner(shift_rows, shift_cols).swap(data.bottomRightCorner(shift_rows, shift_cols));
      data.topRightCorner(shift_rows, shift_cols).swap(data.bottomLeftCorner(shift_rows, shift_cols));
   }

   template <class T>
   inline void inv_shift2(Eigen::MatrixBase<T>& data)
   {
      if (data.rows() % 2 != 0 || data.cols() % 2 != 0) {
         throw std::runtime_error("efftw::shift2 requires even sized inputs");
      }
      const auto shift_rows = data.rows() / 2;
      const auto shift_cols = data.cols() / 2;
      data.bottomRightCorner(shift_rows, shift_cols).swap(data.topLeftCorner(shift_rows, shift_cols));
      data.bottomLeftCorner(shift_rows, shift_cols).swap(data.topRightCorner(shift_rows, shift_cols));
   }
}
