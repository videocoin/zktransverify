#!/bin/bash

set -e

PACKAGES="build-essential gcc g++ gcc-multilib g++-multilib\
          libboost-all-dev libgmp10 libgmp-dev \
          zlib1g zlib1g-dev libssl-dev cmake"

if [[ $(apt-cache search openjdk-8-jdk) ]]; then
    PACKAGES+=" openjdk-8-jdk"

else
    PACKAGES+=" openjdk-7-jdk"
fi

sudo apt-get install $PACKAGES

VIDEOCOIN_ENV=$HOME/.videocoin

mkdir $VIDEOCOIN_ENV
cp -R apps $VIDEOCOIN_ENV

echo "Setting up bash profile"
echo "" >> $HOME/.bashrc
echo "# Custom variables for Videocoin" >> $HOME/.bashrc
echo "export VIDEOCOIN_ENV=${VIDEOCOIN_ENV}" >> $HOME/.bashrc
echo "" >> $HOME/.bashrc

cd thirdparty
./install_deps.sh
 
