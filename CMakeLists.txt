cmake_minimum_required(VERSION 3.1)
project(testmatch_backend)
include(CTest)
include(GenerateExportHeader)

# Ensure C++20 is used
set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED 20)


# Build options
option(BUILD_AS_PYTHON "Option to compile library as a Python extension module" OFF)
option(BUILD_TESTS "Option to also compile testing executables (requires Boost.UnitTestFramework" OFF)
option(BUILD_DEMOS "Option to compile demos found in examples/demos" OFF)

# Compiler flags
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

# Sources
set(sources 
  src/cpp/cards.cpp 
  src/cpp/team.cpp 
  src/cpp/models.cpp 
  src/cpp/simulation.cpp 
  src/cpp/helpers.cpp 
  src/cpp/enums.cpp 
  src/cpp/matchtime.cpp 
  src/cpp/pregame.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/include)


# If compiling as Python extension
if (BUILD_AS_PYTHON)
  if(SKBUILD)
    # Scikit-Build does not add your site-packages to the search path automatically,
    # so we need to add it _or_ the pybind11 specific directory here.
    execute_process(
      COMMAND
        "${PYTHON_EXECUTABLE}" -c
        "import pybind11; print(pybind11.get_cmake_dir())"
      OUTPUT_VARIABLE _tmp_dir
      OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ECHO STDOUT)
    list(APPEND CMAKE_PREFIX_PATH "${_tmp_dir}")
  endif()

  FIND_PACKAGE(pybind11 CONFIG REQUIRED)
  pybind11_add_module(_testmatch MODULE ${sources} src/cpp/python.cpp)

  target_link_libraries(_testmatch PRIVATE
          ${PYTHON_LIBRARIES}
  )

  target_include_directories(_testmatch PUBLIC
      ${CMAKE_SOURCE_DIR}/include/testmatch
  )

  install(TARGETS _testmatch DESTINATION .)

else()
  # Add data to target
  add_library(TestMatch STATIC ${sources})
  target_include_directories(TestMatch PUBLIC ${CMAKE_SOURCE_DIR}/include/testmatch)
 
  #install(TARGETS TestMatch DESTINATION .)

  # OPTIONAL: Build tests
  if (BUILD_TESTS)
    add_subdirectory(test/cpp)
    enable_testing()
  endif()

  
  # OPTIONAL: Build demos in examples/cpp
  if (BUILD_DEMOS)
    add_subdirectory(examples/cpp)
  endif()

endif()
