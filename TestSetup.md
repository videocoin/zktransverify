# Test Setup and Procedure for VideoCoin Transcode Verification using zkSnarks (zktransverify)

The purpose of this test is to check if "videocoin proof generation and verifier" is functioning correctly. The test procedure includes running the oprtations at wallet, transcode-worker, storage-worker and verifier manually. After the integration with the videocoin-net, these operations will be embedded in the respective modules. 

## Overview
The transcode verification proof system uses the following logic:

* Receives two macroblocks (one from input stream and the second from transcoded stream) as private input/witness
* Receives sha256 hash of transcode-macroblock as public input/witness
* Receives SSIM threshold value as public input/witness
* The constraint system ensures the verifcation succeeds only when SSIM of the two macroblocks is below the SSIM supplied as public witness and sha256 hash of the transcode-macroblock is equal to the sha256 has supplied as public input.
* A proving-key and verification key are generated

The operation of the wallet-node is emulated by encoding a stream to generate 10sec TS segment. 

The operation of the transcode-worker is emulated by generating correctly transcoded stream as well attcak streams as described in the following sections. The proof-generation at the transcode-worker includes customized ffmpeg application that retrieves macroblocks from source and transcoded-streams and supplied to the proof-generator. These macroblocks are selected based on the offsets generated from sha256 hashes of source and transcode streams.

The storage-worker after receiving the transcoded stream retrieves the sha256 hash of the macroblock and submits to the vefifier. The storage-worker includes customized ffmpeg application that retrieves macroblock based on frame/macroblock offsets generated similar to storage-worker.

The verifier uses the following inputs and performs using zkSNARK ec pairing functions.
* proof supplied by the transcode-worker
* SSIM supplied by the Wallet
* sha256 hash of transcoded stream submitted by the storage-worker

The commands at each node are described below.

## 1. Wallet : Generate test source stream

Encodes streams from test sequences listed in Appendix-A using following settings: 

| Param | Value |
| --- | --- |
| preset | medium |
| bitrate | 40mbps |
| gop length | 50 |

Example:
```
ffmpeg -y -benchmark -i ./crowd_run_2160p50.y4m  -vframes 500 -vcodec libx264 -b:v 40M  -r 50 -preset medium -force_key_frames "expr:gte(t,n_forced*1)" crowd_run_2160p50_40M.ts
```

## 2. Transcode-worker:  Generates attack streams as wells as correct transcoded stream.
2.1 Correct Transcode   - Transcodes using the correct parameter settings

| Param | Value |
| --- | --- |
| preset | medium |
| bitrate | 20mbps |
| gop length | 50 |
```
ffmpeg -y -benchmark -i ./crowd_run_2160p50_40M.ts  -vframes 500 -vcodec libx264 -b:v 20M  -r 50 -preset medium -force_key_frames "expr:gte(t,n_forced*1)" crowd_run_2160p50_20M_correct.ts
```

2.2 Attack  - No-transcode - Submits source stream as transcoded stream
The source stream is submitted as transcoded stream

2.3 Attack - Wrong Stream - Submits a stream not produced from source
A stream unrelated to source stream is submitted as transcoded stream.

2.4 Attack - Low quality  - Transcodes using presets which produce low quality streams
| Param | Value |
| --- | --- |
| preset | ultrafast |
| bitrate | 20mbps |
| gop length | 50 |
```
ffmpeg -y -benchmark -i ./crowd_run_2160p50_40M.ts  -vframes 500 -vcodec libx264 -b:v 20M  -r 50 -preset ultrafast -force_key_frames "expr:gte(t,n_forced*1)" crowd_run_2160p50_20M_attack_lq.ts
```

Transcode worker runs the proof generator and submits the proof to verifier.
```
genproof <input:source stream> <input:transcoded stream> <input:proving key> <output:proof>  
```

## 3. Storage worker: Extracts zkSNARK witness from transcoded stream
3.1 Generate random frame/macroblock offset using hash of transcoded stream and hash of input stream.
3.2 Retrieve the macroblock from transcode stream corresponding to the random frame/macroblock offsets from the previous step. Calculate the hash wich will act as public-input(witness) to the proof-verifier.

```
./genwitness <input:source stream file> <input:transcode stream file> <zkSNARK witness> 
```
The above command outputs hash of the macroblock corresponding to the macroblock used by the  proof gneration.

## 4. Vefier: Accept or reject proof submitted by storage-worker
Run the verification using the (1) proof submitted by Transcode-worker and (2) public-witness submitted by Stoarage-worker
Currently, it will be a stand alone verifier, but it will be integrated in to an Ethereum Smart-Contract.

## Appendix A: Test Source Streams

The following raw video source clips are used to create "source stream" test vectors

website: https://media.xiph.org/video/derf/  
crowdrun: https://media.xiph.org/video/derf/y4m/crowd_run_2160p50.y4m  
ducks_take_off: https://media.xiph.org/video/derf/y4m/ducks_take_off_2160p50.y4m  
in_to_tree: https://media.xiph.org/video/derf/y4m/in_to_tree_2160p50.y4m  

## Appendix B:
A one time zkSNARK setup operation will be performed that generates proving and verification keys as described below:

 Follow this [link](./src/videocoin_proving_system/README.md) to run test setup.
