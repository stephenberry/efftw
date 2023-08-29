cmake_minimum_required(VERSION 3.24)

project(eigen_fft
    VERSION 0.0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE srcs src/*.cpp include/efftw/*.hpp)

# Organize files into directories
foreach(FILE ${srcs})
   # Get the directory of the source file
   get_filename_component(PARENT_DIR "${FILE}" PATH)

   # Remove common directory prefix to make the group
   string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

if(MSVC)
   # Make sure we are using windows slashes
   string(REPLACE "/" "\\" GROUP "${GROUP}")
endif()

   # Group into "Source Files" and "Header Files"
   if ("${FILE}" MATCHES ".*\\.cpp")
      set(GROUP "Source Files${GROUP}")
   elseif("${FILE}" MATCHES ".*\\.h")
      set(GROUP "Header Files${GROUP}")
   endif()

   source_group("${GROUP}" FILES "${FILE}")
endforeach()

include_directories(include ../eigen "/opt/homebrew/Cellar/fftw/3.3.10_1/include")

add_executable(${PROJECT_NAME} ${srcs})

# Link the FFTW library to your project
target_link_libraries(${PROJECT_NAME} "/opt/homebrew/Cellar/fftw/3.3.10_1/lib/libfftw3.a"
"/opt/homebrew/Cellar/fftw/3.3.10_1/lib/libfftw3_threads.a"
    m           # Link against math library
    pthread     # Link against pthread library
    )