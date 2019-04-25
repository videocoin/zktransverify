#!/bin/bash

set -e

UP=`pwd`
DEPS_DIR=$UP/dependencies
mkdir -p $DEPS_DIR/lib
ln -sf $DEPS_DIR/lib $DEPS_DIR/lib64

TAR="tar xvzf"

# papi
echo "installing PAPI"
$TAR papi-5.4.1.tar.gz
cd papi-5.4.1/src
./configure --prefix=$DEPS_DIR
make
make install
cd $UP

#Kyoto Cabinet
echo "installing Kyoto Cabinet"
$TAR kyotocabinet-1.2.76.tar.gz
cp gcc6-workaround.patch kyotocabinet-1.2.76
cd kyotocabinet-1.2.76
patch -p1 < gcc6-workaround.patch
./configure --prefix=$DEPS_DIR
make
make install
cd $UP

#leveldb
echo "installing leveldb"
$TAR leveldb-1.10.0.tar.gz
cd leveldb-1.10.0
make
cp --preserve=links libleveldb.* $DEPS_DIR/lib
cp -r include/leveldb $DEPS_DIR/include
cd $UP

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
cmake -DCMAKE_INSTALL_PREFIX=$DEPS_DIR -DWITH_PROCPS=OFF -DMULTICORE=ON -DCURVE=ALT_BN128 ..
DESTDIR=$DEPS_DIR make install
cd $UP

# TODO: add ffmpeg?