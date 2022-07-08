# CMakeLists files in this project can
# refer to the root source directory of the project as ${TFLITE_SOURCE_DIR} and
# to the root binary directory of the project as ${TFLITE_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)
project (TFLITE)

# Add executable called "tflite" that is built from the source files
# "tflite.cxx". The extensions are automatically found.
add_executable (tflite tflite.cxx)

# Link the executable to the Hello library. Since the Hello library has
# public include directories we will use those link directories when building
# helloDemo
#target_link_libraries (helloDemo LINK_PUBLIC Hello)


install(FILES tflite DESTINATION ${TFLITE_SOURCE_DIR}/../priv)

