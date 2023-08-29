cmake_minimum_required(VERSION 3.24)

include(cmake/prelude.cmake)

project(efftw
    VERSION 0.0.1
    LANGUAGES CXX)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

add_library(efftw_efftw INTERFACE)
add_library(efftw::efftw ALIAS efftw_efftw)

set_property(TARGET efftw_efftw PROPERTY EXPORT_NAME efftw)

target_compile_features(efftw_efftw INTERFACE cxx_std_20)
target_include_directories(
   efftw_efftw ${warning_guard}
   INTERFACE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

if (PROJECT_IS_TOP_LEVEL)
  include(cmake/dev-mode.cmake)
endif()