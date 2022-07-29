# CMakeLists files in this project can
# refer to the root source directory of the project as ${TFLITE_SOURCE_DIR} and
# to the root binary directory of the project as ${TFLITE_BINARY_DIR}.
cmake_minimum_required(VERSION 3.16)
project (tflite C CXX)

add_subdirectory(
  "./tensorflow/lite"
  EXCLUDE_FROM_ALL
)

set(CMAKE_CXX_STANDARD 14)

# Add executable called "tflite" that is built from the source files
# "tflite.cxx". The extensions are automatically found.
add_executable (tflite tflite.cxx)


add_library(tensorflow-lite STATIC IMPORTED)
set_target_properties(tensorflow-lite PROPERTIES IMPORTED_LOCATION ../tflite_build/libtensorflow-lite.a)
set_target_properties(tensorflow-lite PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ../tensorflow_src/tensorflow/lite/)


install(FILES tflite DESTINATION ${TFLITE_SOURCE_DIR}/../priv 
       PERMISSIONS
          OWNER_READ OWNER_EXECUTE OWNER_WRITE
          GROUP_READ GROUP_EXECUTE
          WORLD_READ WORLD_EXECUTE
    )

