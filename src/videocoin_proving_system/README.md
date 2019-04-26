# Videcocoin Proving System

## Installation Guide

Install required packages and thirdparty libraries:
```
./install_debian_ubuntu.sh
```

Build binaries:

```
mkdir build
cd build
cmake ..
make
```

## Running binaries

Go to root directory

```
cd ..
```

### Create temporary folder

```
mkdir temp
```

**Note**: Currently tests and binaries are tied to temp folder. Should be removed during code cleanup.

### Generate proving and verification keys

```
./build/keygen ssim16x16 temp/v.key temp/p.key temp/vk_data
```

`v.key` is a verification key which is required for verification and used by standalone verifier.

`p.key` is a proving key which is required got proof generation. 

`vk_data` is verification key in uncompressed format which is required for truffle tests.

### Calculate SSIM for two video samples and generate proof

```
./build/genproof extools/SampleVideo_1280x720_1mb.mp4 extools/SampleVideo_1280x720_1mb.mp4 temp/p.key
```

`genproof` will generate `ssim.proof` and `input.txt` and store them into `temp` directory.

**Note**: Add more video samples.

### Running Truffle Tests

**Pre-requirements**: `node.js`, `truffle` packages should be installed.

```
cd smart-contract
truffle test
```
