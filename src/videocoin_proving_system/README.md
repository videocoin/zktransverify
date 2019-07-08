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
  -m [ --mode ] arg               set algorithm type <ssim | h264 | h264prover>
  -v [ --vkey ] arg               path to verification key
  -p [ --pkey ] arg               path to proving key
  -u [ --uncompressed-vkey ] arg  path to uncompressed verification key
  -j [ --json-vkey ] arg          path to json verification key
```

Key generation:

```
./keygen -m h264prover -v v.key -p p.key -u v.key.uncompressed -j v.key.json
```


### Generate proof for SSIM calculation

Usage of `genproof` binary:

```
General options:
  -h [ --help ]                    produce help message

Prover options:
  -p [ --pkey ] arg                path to proving key
  -f [ --files ] arg               list of %file1, %file2
  -P [ --proof ] arg               path to proof file
  -u [ --uncompressed-proof ] arg  path to uncompressed proof file
  -j [ --json-proof ] arg          path to json proof file
  -w [ --witness ] arg             path to witness file
  -s [ --ssim-level ] arg          threshold ssim level [0-100]
```

Proof generation:

```
./genproof --files ../test_vectors/crowd_run_2160p50_40M.ts ../test_vectors/crowd_run_2160p50_20M_correct.ts -p p.key -s 80 -w witness.txt -P h264.proof -u h264.proof.uncompressed
```

### Verify proof using standalone verifier

Usage of `verifier` binary:

```
General options:
  -h [ --help ]              produce help message

Verifier options:
  -m [ --mode ] arg          set algorithm type <ssim | h264 | h264prover>
  -v [ --vkey ] arg          path to verification key
  -p [ --proof ] arg         path to proof
  -w [ --witness ] arg       path to witness file
```

Proof verification:

```
./verifier -m h264prover -v v.key -p h264.proof -w witness.txt 
```

### Verify proof using Truffle Tests

TODO: Update smart-contract to accept macroblock hash as input parameter.

**Pre-requirements**: `node.js`, `truffle` should be installed.

```
cd ../smart-contract
truffle test
```

### Testing attack vectors

Following proof generations will generate incorrect proofs. Verification using truffle test or standalone verifier will show this.

**Low Quality Attack** - Transcodes using presets which produce low quality streams:

```
./genproof --files ../test_vectors/crowd_run_2160p50_40M.ts ../test_vectors/crowd_run_2160p50_20M_attack_lq.ts -p p.key -s 80 -w witness.txt -P h264.proof -u h264.proof.uncompressed
```

**Wrong Stream Attack** - Submits a stream not produced from source A stream unrelated to source stream is submitted as transcoded stream:

```
./genproof --files ../test_vectors/crowd_run_2160p50_40M.ts ../test_vectors/ducks_take_off_2160p50_40M.ts -p p.key -s 80 -w witness.txt -P h264.proof -u h264.proof.uncompressed
```

**No Transcode Attack** - Submits source stream as transcoded stream The source stream is submitted as transcoded stream:

```
./genproof --files ../test_vectors/crowd_run_2160p50_40M.ts ../test_vectors/crowd_run_2160p50_40M.ts -p p.key -s 80 -w witness.txt -P h264.proof -u h264.proof.uncompressed
```

### `genproof` internals

Generator uses new zkSNARK circuit which consists of three algorithms:

* SHA256 generator
* Macroblock (INTRA16x16 modes) decoder
* SSIM calculation

Circuit has two public parameters:

* Reference SSIM value (threshold)
* SHA265 (32 bytes) digest of source macroblock

Circuit has two auxiliary parameters:
* Source macroblock (256 bytes) 
* Macroblock decoder specific data

Circuit has two out parameter:
* Resulting value (0 or 1)
* Zaatar leftover (always 0)

Circuit computes next:
* Generates SHA256 digest for source macroblock
* Compares calculated digest with supplied one
* Decodes transcoded macroblock from auxiliary data
* Calculates SSIM value for supplied macroblock and decoded one
* Circuit returns `1` result, if supplied digest and calculated are identical and calculated SSIM is in range (ref_SSIM, 100); otherwise - `0`

Circuit implementation [#h264 prover circuit](https://github.com/videocoin/zktransverify/blob/macroblock-circuit/src/pequin/pepper/apps/h264_prover_2.c) 


### Proof generation benchmarking

TODO: update benchmarking data accoring to new circuit.

(SSIM only)Google cloud instance, v8CPU:

```
(leave) Call to r1cs_ppzksnark_prover           [4.2650s x5.55] (4.2650s x5.55 from start)
(leave) Call to r1cs_ppzksnark_prover           [3.6660s x5.23] (3.6660s x5.23 from start)
(leave) Call to r1cs_ppzksnark_prover           [4.1090s x5.58] (4.1090s x5.58 from start)
(leave) Call to r1cs_ppzksnark_prover           [4.2088s x5.65] (4.2089s x5.65 from start)
(leave) Call to r1cs_ppzksnark_prover           [4.1530s x5.58] (4.1530s x5.58 from start)
```

(SSIM only)Google cloud instance, v16CPU:

```
(leave) Call to r1cs_ppzksnark_prover           [3.1922s x9.20] (3.1922s x9.20 from start)
(leave) Call to r1cs_ppzksnark_prover           [3.0785s x8.51] (3.0785s x8.51 from start)
(leave) Call to r1cs_ppzksnark_prover           [3.0543s x8.30] (3.0544s x8.30 from start)
(leave) Call to r1cs_ppzksnark_prover           [3.1612s x9.23] (3.1612s x9.23 from start)
(leave) Call to r1cs_ppzksnark_prover           [3.0549s x8.77] (3.0549s x8.77 from start)
```

(SSIM only)Google cloud instance, v8CPU, Groth16:

```
(leave) Call to r1cs_gg_ppzksnark_prover        [4.0300s x5.29] (4.0300s x5.29 from start)
(leave) Call to r1cs_gg_ppzksnark_prover        [4.6938s x5.80] (4.6938s x5.80 from start)
(leave) Call to r1cs_gg_ppzksnark_prover        [4.6269s x5.65] (4.6269s x5.65 from start)
(leave) Call to r1cs_gg_ppzksnark_prover        [4.6361s x5.79] (4.6361s x5.79 from start)
(leave) Call to r1cs_gg_ppzksnark_prover        [4.6581s x5.75] (4.6581s x5.75 from start)
```
 
(SSIM only)Google cloud instance, v16CPU, Groth16:
 
 ```
(leave) Call to r1cs_gg_ppzksnark_prover        [3.4591s x9.49] (3.4591s x9.49 from start)
(leave) Call to r1cs_gg_ppzksnark_prover        [3.3320s x9.12] (3.3320s x9.12 from start)
(leave) Call to r1cs_gg_ppzksnark_prover        [3.4591s x9.74] (3.4591s x9.74 from start)
(leave) Call to r1cs_gg_ppzksnark_prover        [3.3138s x8.88] (3.3138s x8.88 from start)
(leave) Call to r1cs_gg_ppzksnark_prover        [3.4126s x9.03] (3.4126s x9.03 from start)
 ```

(SSIM only)Google cloud instance, v64CPU, Groth16:

```
(leave) Call to r1cs_gg_ppzksnark_prover        [2.6216s x30.93]        (2.6216s x30.93 from start)
(leave) Call to r1cs_gg_ppzksnark_prover        [2.5062s x30.09]        (2.5062s x30.09 from start)
(leave) Call to r1cs_gg_ppzksnark_prover        [2.4423s x28.24]        (2.4423s x28.24 from start)
(leave) Call to r1cs_gg_ppzksnark_prover        [2.4632s x30.16]        (2.4632s x30.16 from start)
(leave) Call to r1cs_gg_ppzksnark_prover        [2.4493s x30.08]        (2.4493s x30.08 from start)
```
