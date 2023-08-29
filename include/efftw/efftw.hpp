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

   template <is_vector T>
   struct f1 final
   {
      Eigen::MatrixBase<T>& data;
      
      f1(Eigen::MatrixBase<T>& data) : data(data) {}
      ~f1() {
         fftw_destroy_plan(plan);
      }
      
      void operator()() {
          fftw_execute(plan);
      }
      
      using value_type = typename Eigen::MatrixBase<T>::Scalar;
      
   private:
      fftw_plan plan = fftw_planner_1d<value_type>(int(data.rows()), int(data.cols()),
                                                                  reinterpret_cast<fftw_complex*>(data.derived().data()), reinterpret_cast<fftw_complex*>(data.derived().data()),
                                                                  FFTW_FORWARD, FFTW_ESTIMATE);
   };
   
   enum class direction : int32_t
   {
      forward = FFTW_FORWARD,
      backward = FFTW_BACKWARD
   };

   template <class T>
   struct f2 final
   {
      Eigen::MatrixBase<T>& data;
      
      f2(Eigen::MatrixBase<T>& data) : data(data) {}
      ~f2() {
         fftw_destroy_plan(plan);
      }
      
      void operator()() {
          fftw_execute(plan);
      }
      
      using value_type = typename Eigen::MatrixBase<T>::Scalar;
      
   private:
      fftw_plan plan = fftw_planner_2d<value_type>(int(data.rows()), int(data.cols()),
          reinterpret_cast<fftw_complex*>(data.derived().data()), reinterpret_cast<fftw_complex*>(data.derived().data()),
          FFTW_FORWARD, FFTW_ESTIMATE);
   };
   
   template <is_vector T>
   struct i1 final
   {
      Eigen::MatrixBase<T>& data;
      
      i1(Eigen::MatrixBase<T>& data) : data(data) {}
      ~i1() {
         fftw_destroy_plan(plan);
      }
      
      void operator()() {
          fftw_execute(plan);
      }
      
      using value_type = typename Eigen::MatrixBase<T>::Scalar;
      
   private:
      fftw_plan plan = fftw_planner_1d<value_type>(int(data.rows()), int(data.cols()),
                                                                  reinterpret_cast<fftw_complex*>(data.derived().data()), reinterpret_cast<fftw_complex*>(data.derived().data()),
                                                   FFTW_BACKWARD, FFTW_ESTIMATE);
   };

   template <class T>
   struct i2 final
   {
      Eigen::MatrixBase<T>& data;
      
      i2(Eigen::MatrixBase<T>& data) : data(data) {}
      ~i2() {
         fftw_destroy_plan(plan);
      }
      
      void operator()() {
          fftw_execute(plan);
         data /= double(data.rows()) * data.cols(); // normalize
      }
      
      using value_type = typename Eigen::MatrixBase<T>::Scalar;
      
   private:
      fftw_plan plan = fftw_planner_2d<value_type>(int(data.rows()), int(data.cols()),
          reinterpret_cast<fftw_complex*>(data.derived().data()), reinterpret_cast<fftw_complex*>(data.derived().data()),
          FFTW_BACKWARD, FFTW_ESTIMATE);
   };
   
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
