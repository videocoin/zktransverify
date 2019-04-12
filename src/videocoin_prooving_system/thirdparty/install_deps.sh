#!/bin/bash

set -e

UP=`pwd`
DEPS_DIR=$UP/dependencies
mkdir -p $DEPS_DIR/lib
ln -sf $DEPS_DIR/lib $DEPS_DIR/lib64

#libsnark
echo "installing libsnark"
[ ! -d libsnark ] && git clone https://github.com/scipr-lab/libsnark.git
cp libsnark_compilerflag.patch libsnark
cd libsnark
git checkout dc78fdae02b437bb6c838a82f9261c49bbd7723e
git submodule init && git submodule update
git apply libsnark_compilerflag.patch
mkdir build && cd build
echo $DEPS_DIR
cmake -DCMAKE_INSTALL_PREFIX=$DEPS_DIR -DWITH_PROCPS=OFF -DMULTICORE=ON ..
DESTDIR=$DEPS_DIR make install
cd $UP
