# Compile example simulation
add_executable(demo demo.cpp)
target_include_directories(demo PUBLIC ${CMAKE_SOURCE_DIR}/include )
target_link_libraries(demo PUBLIC
  TestMatch
)