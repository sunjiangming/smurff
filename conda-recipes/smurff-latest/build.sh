#!/bin/bash

set -x

if [ $(uname) == "Darwin" ]
then
    CMAKE_OPENMP_FLAGS="-DOpenMP_CXX_FLAGS=-fopenmp=libiomp5 -DOpenMP_C_FLAGS=-fopenmp=libiomp5"
    PYTHON_OPENMP_FLAGS="--with-openmp=libiomp5"
else
    CMAKE_OPENMP_FLAGS=""
    PYTHON_OPENMP_FLAGS="--with-openmp"
fi
 
pushd lib/smurff-cpp/cmake
rm -rf build 
mkdir build
cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$PREFIX \
    -DLAPACK_LIBRARIES=${PREFIX}/lib/libmkl_rt${SHLIB_EXT} ${CMAKE_OPENMP_FLAGS}
make -j$CPU_COUNT
make install
popd

pushd python/smurff
$PYTHON setup.py install $PYTHON_OPENMP_FLAGS --single-version-externally-managed --record=record.txt
popd
