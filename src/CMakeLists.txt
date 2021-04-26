
cmake_policy(SET CMP0076 NEW)

add_library(poly_factory INTERFACE)
target_include_directories(poly_factory INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}) 
target_sources(poly_factory INTERFACE pofa/pofa.hpp)

