# Test Setup and Procedure for VideoCoin Transcode Verification using zkSnarks (zktransverify)

The purpose of this test is to check if "videocoin proof generation and verifier" is functioning correctly. The test procedure includes running the oprtations at wallet, transcode-worker, storage-worker and verifier manually. After the integration with the videocoin-net, these operations will be embedded in the respective modules. 

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

 - TODO -
