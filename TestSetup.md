# Test Setup and Procedure for VideoCoin Transcode Verification using zkSnarks (zktransverify)

The purpose of this test is to check if "videocoin proof generation and verifier" are functioning correctly.

1) wallet : Encodes a stream(test source) using one of the encode setting
ffmpeg -i <test source> -b <bit rate> testsrc.ts

2) Transcode worker: Produces following streams for the stream submitted by the wallet.
2.1 Correct Transcode   - Transcodes using the correct parameter settings
ffmpeg

2.2 Attack No-transcode - Submits source stream as transcoded stream
2.3 Attack Wrong Stream - Submits a stream not produced from source
2.4 Attack Low quality  - Transcodes using presets which produce low quality streams
ffmpeg

Transcode worker runs the proof generator and submits the proof to verifier.
genproof <input:source stream> <input:transcoded stream> <input:proving key> <output:proof>  

3) Storage worker: Receives the transcoded stream and extracts public parameters for submission to the verifier
3.1 Generate random frame/macroblock offset using hash of transcoded stream and hash of input stream.
3.2 Retrieve the macroblock from the previous step and calculate the hash
submit the hash to the verifier

4) Vefier: Runs the verificarion using following inputs and outputs whether the proof is accepted or rejected
Run the verification using the (1) proof submitted by Transcode-worker and (2) hash submitted by Stoarage-worker


## Test Source Streams

The following raw video source clips are used to create "source stream" test vectors

website: https://media.xiph.org/video/derf/
crowdrun: https://media.xiph.org/video/derf/y4m/crowd_run_2160p50.y4m
ducks_take_off: https://media.xiph.org/video/derf/y4m/ducks_take_off_2160p50.y4m
old_town_cross: https://media.xiph.org/video/derf/y4m/old_town_cross_2160p50.y4m
