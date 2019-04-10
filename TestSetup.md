# Test Setup and Procedure for VideoCoin Transcode Verification using zkSnarks (zktransverify)

The purpose of this test is to check if "videocoin proof generation and verifier" is functioning correctly.

## 1. wallet : Encodes a yuv420 UHD test source into 10sec TS segment

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

2) Transcode worker: Produces following streams for the stream submitted by the wallet.
2.1 Correct Transcode   - Transcodes using the correct parameter settings
| Param | Value |
| --- | --- |
| preset | medium |
| bitrate | 20mbps |
| gop length | 50 |
```
ffmpeg -y -benchmark -i ./crowd_run_2160p50_40M.ts  -vframes 500 -vcodec libx264 -b:v 20M  -r 50 -preset medium -force_key_frames "expr:gte(t,n_forced*1)" crowd_run_2160p50_20M_correct.ts
```

2.2 Attack No-transcode - Submits source stream as transcoded stream

2.3 Attack Wrong Stream - Submits a stream not produced from source

2.4 Attack Low quality  - Transcodes using presets which produce low quality streams
| Param | Value |
| --- | --- |
| preset | ultrafast |
| bitrate | 20mbps |
| gop length | 50 |
```
ffmpeg -y -benchmark -i ./crowd_run_2160p50_40M.ts  -vframes 500 -vcodec libx264 -b:v 20M  -r 50 -preset ultrafast -force_key_frames "expr:gte(t,n_forced*1)" crowd_run_2160p50_20M_attack_lq.ts
```

Transcode worker runs the proof generator and submits the proof to verifier.
genproof <input:source stream> <input:transcoded stream> <input:proving key> <output:proof>  

## 3. Storage worker: Receives the transcoded stream and extracts public parameters for submission to the verifier
3.1 Generate random frame/macroblock offset using hash of transcoded stream and hash of input stream.
3.2 Retrieve the macroblock from the previous step and calculate the hash
submit the hash to the verifier

## 4. Vefier: Runs the verificarion using following inputs and outputs whether the proof is accepted or rejected
Run the verification using the (1) proof submitted by Transcode-worker and (2) hash submitted by Stoarage-worker


## Appendix A: Test Source Streams

The following raw video source clips are used to create "source stream" test vectors

website: https://media.xiph.org/video/derf/ 
crowdrun: https://media.xiph.org/video/derf/y4m/crowd_run_2160p50.y4m 
ducks_take_off: https://media.xiph.org/video/derf/y4m/ducks_take_off_2160p50.y4m 
in_to_tree: https://media.xiph.org/video/derf/y4m/in_to_tree_2160p50.y4m 
