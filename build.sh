#!/bin/bash

CXX=g++
CXXFLAGS="-std=c++17 -Wall -g"
SRC_DIR="."
BUILD_DIR="."
EXEC_NAME="Main"
TMP_FILES=".o .obj"

if [ "$1" == "--online-judge" ]; then
    CXXFLAGS="$CXXFLAGS -D ONLINE_JUDGE -D DEBUG"
fi

for src_file in $SRC_DIR/*.cpp; do
    obj_file="$BUILD_DIR/$(basename "$src_file" .cpp).o"
    $CXX $CXXFLAGS -c $src_file -o $obj_file
done

$CXX $CXXFLAGS $BUILD_DIR/*.o -o $BUILD_DIR/$EXEC_NAME

for ext in $TMP_FILES; do
    rm -f $BUILD_DIR/*$ext
done