# VideoCoin Proving System

This is initial implementation of VideoCoin proof-of-transcode based on zkSNARKs for end-to-end testing.

## Installation Guide

Install required packages and third party libraries:
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


### Generate proving and verification keys

Usage of `keygen` binary:

```
General options:
  -h [ --help ]                   produce help message

Generator options:
  -m [ --mode ] arg               set algorithm type <ssim16x16 | ssim32x32 | 
                                  ssim64x64>
  -v [ --vkey ] arg               path to verification key
  -p [ --pkey ] arg               path to proving key
  -u [ --uncompressed-vkey ] arg  path to uncompressed verification key
  -j [ --json-vkey ] arg          path to json verification key
```

Key generation:

```
./keygen -m ssim16x16 -v v.key -p p.key -u v.key.uncompressed -j v.key.json
```


### Generate proof for SSIM calculation

Usage of `genproof` binary:

```
General options:
  -h [ --help ]                    produce help message

Prover options:
  -p [ --pkey ] arg                path to proving key
  -f [ --files ] arg               list of %file1, %file2
  -i [ --input-output ] arg        path to input-output file
  -P [ --proof ] arg               path to proof file
  -u [ --uncompressed-proof ] arg  path to uncompressed proof file
  -j [ --json-proof ] arg          path to json proof file
  -w [ --witness ] arg             path to witness file
  -s [ --ssim-level ] arg          threshold ssim level [0-100]
```

Proof generation:

```
./genproof --files ../test_vectors/crowd_run_2160p50_40M.ts ../test_vectors/crowd_run_2160p50_20M_correct.ts -p p.key -s 80 -w witness -i inputs.txt -P ssim.proof -u ssim.proof.uncompressed
```

**Note**: Add more video samples.

### Verify proof using standalone verifier

Usage of `verifier` binary:

```
General options:
  -h [ --help ]              produce help message

Verifier options:
  -m [ --mode ] arg          set algorithm type <ssim16x16 | ssim32x32 | 
                             ssim64x64>
  -v [ --vkey ] arg          path to verification key
  -p [ --proof ] arg         path to proof
  -i [ --input-output ] arg  path to input-output file
```

Proof verification:

```
./verifier -m ssim16x16 -v v.key -p ssim.proof -i inputs.txt 
```

### Verify proof using Truffle Tests

**Pre-requirements**: `node.js`, `truffle` should be installed.

```
cd ../smart-contract
truffle test
```

### Proof generation benchmarking

Google cloud instance, v8CPU:

```
(leave) Call to r1cs_ppzksnark_prover           [4.1696s x5.62] (4.1696s x5.62 from start)
(leave) Call to r1cs_ppzksnark_prover           [4.1477s x5.59] (4.1477s x5.59 from start)
(leave) Call to r1cs_ppzksnark_prover           [4.1645s x5.64] (4.1645s x5.64 from start)
(leave) Call to r1cs_ppzksnark_prover           [4.1482s x5.60] (4.1482s x5.60 from start)
(leave) Call to r1cs_ppzksnark_prover           [4.1860s x5.61] (4.1860s x5.61 from start)
(leave) Call to r1cs_ppzksnark_prover           [4.1550s x5.63] (4.1550s x5.63 from start)
(leave) Call to r1cs_ppzksnark_prover           [4.0892s x5.58] (4.0892s x5.58 from start)
(leave) Call to r1cs_ppzksnark_prover           [4.1394s x5.63] (4.1394s x5.63 from start)
(leave) Call to r1cs_ppzksnark_prover           [4.1795s x5.60] (4.1795s x5.60 from start)
(leave) Call to r1cs_ppzksnark_prover           [4.2008s x5.54] (4.2008s x5.54 from start)
(leave) Call to r1cs_ppzksnark_prover           [4.1483s x5.62] (4.1483s x5.62 from start)
(leave) Call to r1cs_ppzksnark_prover           [4.1841s x5.66] (4.1841s x5.66 from start)
(leave) Call to r1cs_ppzksnark_prover           [4.1767s x5.59] (4.1767s x5.59 from start)
(leave) Call to r1cs_ppzksnark_prover           [4.2650s x5.55] (4.2650s x5.55 from start)
(leave) Call to r1cs_ppzksnark_prover           [3.6660s x5.23] (3.6660s x5.23 from start)
(leave) Call to r1cs_ppzksnark_prover           [4.1090s x5.58] (4.1090s x5.58 from start)
(leave) Call to r1cs_ppzksnark_prover           [4.2088s x5.65] (4.2089s x5.65 from start)
(leave) Call to r1cs_ppzksnark_prover           [4.1530s x5.58] (4.1530s x5.58 from start)
```
