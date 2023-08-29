#pragma once

#include <fftw3.h>
#include <Eigen/Dense>

#include <random>

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
       { T::ColsAtCompileTime == 1 } -> std::same_as<std::true_type>;
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
   
   enum class direction : int32_t
   {
      forward = FFTW_FORWARD,
      backward = FFTW_BACKWARD
   };
   
   template <class T, direction Direction>
   struct gen1
   {
      Eigen::MatrixBase<T>& data;
      
      gen1(Eigen::MatrixBase<T>& data) : data(data) {}
      ~gen1() {
         fftw_destroy_plan(plan);
      }
      
      void operator()() {
         fftw_execute(plan);
         if constexpr (Direction == direction::backward) {
            data /= double(data.rows()); // normalize
         }
      }
      
      using value_type = typename Eigen::MatrixBase<T>::Scalar;
      
   private:
      fftw_plan plan = fftw_planner_1d<value_type>(int(data.rows()), int(data.cols()),
                                                                  reinterpret_cast<fftw_complex*>(data.derived().data()), reinterpret_cast<fftw_complex*>(data.derived().data()),
                                                   int(Direction), FFTW_ESTIMATE);
   };
   
   template <class T, direction Direction>
   struct gen2
   {
      Eigen::MatrixBase<T>& data;
      
      gen2(Eigen::MatrixBase<T>& data) : data(data) {}
      ~gen2() {
         fftw_destroy_plan(plan);
      }
      
      void operator()() {
         fftw_execute(plan);
         if constexpr (Direction == direction::backward) {
            data /= double(data.rows()) * data.cols(); // normalize
         }
      }
      
      using value_type = typename Eigen::MatrixBase<T>::Scalar;
      
   private:
      fftw_plan plan = fftw_planner_2d<value_type>(int(data.rows()), int(data.cols()),
          reinterpret_cast<fftw_complex*>(data.derived().data()), reinterpret_cast<fftw_complex*>(data.derived().data()),
          int(Direction), FFTW_ESTIMATE);
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
      const auto shift = size / 2;
      data.segment(0, shift).swap(data.segment(shift, size - shift));
   }
   
   template <is_vector T>
   inline void inv_shift1(Eigen::MatrixBase<T>& data)
   {
      const auto size = data.size();
      const auto shift = size / 2;
      data.segment(0, shift).swap(data.segment(shift, size - shift));
   }
   
   template <class T>
   inline void shift2(Eigen::MatrixBase<T>& data)
   {
      const auto shift_rows = data.rows() / 2;
      const auto shift_cols = data.cols() / 2;
      data.topLeftCorner(shift_rows, shift_cols).swap(data.bottomRightCorner(shift_rows, shift_cols));
      data.topRightCorner(shift_rows, shift_cols).swap(data.bottomLeftCorner(shift_rows, shift_cols));
   }
   
   template <class T>
   inline void inv_shift2(Eigen::MatrixBase<T>& data)
   {
      const auto shift_rows = data.rows() / 2;
      const auto shift_cols = data.cols() / 2;
      data.bottomRightCorner(shift_rows, shift_cols).swap(data.topLeftCorner(shift_rows, shift_cols));
      data.bottomLeftCorner(shift_rows, shift_cols).swap(data.topRightCorner(shift_rows, shift_cols));
   }
}
