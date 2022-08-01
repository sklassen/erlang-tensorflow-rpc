
export INCLUDE=/opt/google/tensorflow/2.8.0-gpu/include
export LIBRARY_PATH=/opt/google/tensorflow/2.8.0-gpu/lib
export LD_LIBRARY_PATH=/opt/google/tensorflow/2.8.0-gpu/lib

TARGET=../priv/ccpflow

${TARGET}: cppflow.cxx
	g++ -std=c++17 -o ${TARGET} -I /home/simon/src/github.com/serizba/cppflow/include/ -I /opt/google/tensorflow/2.8.0-gpu/include cppflow.cxx -ltensorflow

