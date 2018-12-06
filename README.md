# VideoCoin Proof-of-Transcode using Elliptic Curve Pairing
This repo contains software modules that implements VideoCoin proof-of-transcode based on elliptic curve pairings.
 
Elliptic curve pairings are a recent addition to cryptography and fuelling advancements in the blockchain technologies. There are several blockchain applications of elliptic curve pairings that include Identity Based Encryption, Non-interactive Zero Knowledge proofs, Short Signatures etc.

The proof-of-transcode includes three modules that includes the following:
(A) A library that runs in the context of VideoCoin client that performs the following:
(i) extracts pHash of a frame for each video segment, 
(ii) map pHash to the curve point on G1(p1-1), 
(iii) obtains public, private keys from G2, 
(iv) encrypts the p1-1(i.e. mapped pHash of the frame) with  private key and generate p1-2, 
(v) multiply the p1-2 with generator on G2 to obtain p1 
(vi) generates an escrow record for the video segment that includes a unique id for the video segment, p1 and reward information and registers with smart contract.

(B) A library that runs in the context of Transcoder(or Storage Miner) that performs the following:
(i) extracts pHash of a frame for each video segment, 
(ii) map pHash to the curve point on G1 and generate p2
(iii) Sends the p2 to the smart-contract to claim the transcode mining reward.

(C) An Ethereum smart contract that performs the following:
(i) Maintains escrow records for video segment transcode requests. Each escrow record contains the precomputed challenge supplied by the client library along with mining reward amount,
(ii) Verifies the transcode miner's claim by comparing the elliptic curve point p1*G(supplied by VideoCoin client) with elliptic curve point p2*publickey(supplied by Transcode/Storage Miner).


![Blockdiagram showing VideoCoin Proof-of-transcode verification](./documents/zkproofoftranscode.png)
### References
[1. Exploring Elliptic Curve Pairings,by Vitalik Buterin](https://medium.com/@VitalikButerin/exploring-elliptic-curve-pairings-c73c1864e627)

[2. Having Fun With BN-curves,by Prof Bill Buchanan OBE](https://medium.com/coinmonks/having-fun-with-bn-curves-37fb5b816f67)

[3. Pairings for beginners,by Craig Costello](http://www.craigcostello.com.au/pairings/PairingsForBeginners.pdf)

[4. Precompiled contracts for addition and scalar multiplication on the elliptic curve alt_bn128,by Vitalik Buterin](https://github.com/ethereum/EIPs/blob/master/EIPS/eip-196.md)

[5. Precompiled contracts for optimal ate pairing check on the elliptic curve alt_bn128,by Vitalik Buterin ](https://github.com/ethereum/EIPs/blob/master/EIPS/eip-197.md)

[6. Mathematical Foundations of Elliptic Curve Cryptography,by C Koppensteiner ](http://dmg.tuwien.ac.at/drmota/koppensteinerdiplomarbeit.pdf)

[7. Succinct Non-Interactive Zero Knowledge for a von Neumann Architecture,by Eli Ben-Sasson et al.](https://eprint.iacr.org/2013/879.pdf)

[8. Scalable, transparent, and post-quantum secure computational integrity,by Eli Ben-Sasson et al ](https://eprint.iacr.org/2018/046.pdf)

[9. Bulletproofs: Short Proofs for Confidential Transactions and More, by Benedikt B¨unz et al ](https://eprint.iacr.org/2017/1066.pdf)
