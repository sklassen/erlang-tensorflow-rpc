#!/bin/sh

g++ -std=c++17 -o main.out -I /home/simon/libtensorflow2/include/ -I /home/simon/src/github.com/serizba/cppflow/include/ main.cpp -ltensorflow
