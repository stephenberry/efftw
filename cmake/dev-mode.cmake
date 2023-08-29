enable_language(CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS YES)

include(CTest)

include(FetchContent)

FetchContent_Declare(
   ut
   GIT_REPOSITORY https://github.com/boost-ext/ut.git
   GIT_TAG master
   GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(ut)

# Done in developer mode only, so users won't be bothered by this :)
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/efftw/*.hpp")
source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX headers FILES ${headers})

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
source_group(TREE "${PROJECT_SOURCE_DIR}/src" PREFIX sources FILES ${sources})

include_directories(include ../eigen "/opt/homebrew/Cellar/fftw/3.3.10_1/include")

add_executable(efftw_test ${sources} ${headers})

target_link_libraries(efftw_test PRIVATE efftw::efftw ut
   "/opt/homebrew/Cellar/fftw/3.3.10_1/lib/libfftw3.a"
   "/opt/homebrew/Cellar/fftw/3.3.10_1/lib/libfftw3_threads.a"
    m           # Link against math library
    pthread     # Link against pthread library
)

add_test(NAME efftw_test COMMAND efftw_test)
