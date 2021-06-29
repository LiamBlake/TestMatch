# Boost dependency
find_package(Boost 1.70.0 COMPONENTS unit_test_framework REQUIRED)
include_directories(
  ${Boost_INCLUDE_DIR} 
  ${CMAKE_SOURCE_DIR}/include
)

# Register as test
#add_test(NAME demo COMMAND demo)

# Unit tests
# Automatically add all tests
file(GLOB sources_unittests unit/*.cpp)
file(GLOB headers_unittests unit/*.hpp)

add_executable(unit_tests ${sources_unittests})
#set_target_properties(unit_tests PROPERTIES 
#  IMPORTED_LOCATION ${Boost_LIBRARYDIR}/liblept168.dll
#)

target_compile_definitions(unit_tests PUBLIC UNIT_TESTS)

target_include_directories(unit_tests PUBLIC
    ${CMAKE_SOURCE_DIR}/include 
    ${Boost_INCLUDE_DIR}
    headers_unittests
)

target_link_libraries(unit_tests PUBLIC
  ${Boost_LIBRARIES}
  TestMatch
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

add_test(NAME unit COMMAND unit_tests)
