#!/bin/bash

set -e

PACKAGES="build-essential gcc g++ gcc-multilib g++-multilib\
          libboost-all-dev libgmp10 libgmp-dev rar unrar-free libbz2-dev\
          zlib1g zlib1g-dev libssl-dev cmake libx11-dev\
	  yasm libass-dev libfdk-aac-dev libmp3lame-dev libopus-dev libvorbis-dev libvpx-dev libx264-dev libx265-dev"

sudo apt-get install $PACKAGES

VIDEOCOIN_ENV=$HOME/.videocoin

tar xf apps.tar.xz

mkdir $VIDEOCOIN_ENV
cp -R apps $VIDEOCOIN_ENV

echo "Setting up bash profile"
echo "" >> $HOME/.bashrc
echo "# Custom variables for Videocoin" >> $HOME/.bashrc
echo "export VIDEOCOIN_ENV=${VIDEOCOIN_ENV}" >> $HOME/.bashrc
echo "" >> $HOME/.bashrc
source $HOME/.bashrc

cd thirdparty
./install_deps.sh
 
