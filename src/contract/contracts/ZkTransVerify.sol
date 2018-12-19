pragma solidity ^0.4.14;
library Pairing {
    struct G1Point {
        uint X;
        uint Y;
    }
    // Encoding of field elements is: X[0] * z + X[1]
    struct G2Point {
        uint[2] X;
        uint[2] Y;
    }
    /// @return the generator of G1
    function P1() pure internal returns (G1Point) {
        return G1Point(1, 2);
    }
    /// @return the generator of G2
    function P2() pure internal returns (G2Point) {
        return G2Point(
            [11559732032986387107991004021392285783925812861821192530917403151452391805634,
             10857046999023057135944570762232829481370756359578518086990519993285655852781],
            [4082367875863433681332203403145435568316851327593401208105741076214120093531,
             8495653923123431417604973247489272438418190587263600148770280649306958101930]
        );
    }
    /// @return the negation of p, i.e. p.addition(p.negate()) should be zero.
    function negate(G1Point p) pure internal returns (G1Point) {
        // The prime q in the base field F_q for G1
        uint q = 21888242871839275222246405745257275088696311157297823662689037894645226208583;
        if (p.X == 0 && p.Y == 0)
            return G1Point(0, 0);
        return G1Point(p.X, q - (p.Y % q));
    }
    /// @return the sum of two points of G1
    function addition(G1Point p1, G1Point p2) internal returns (G1Point r) {
        uint[4] memory input;
        input[0] = p1.X;
        input[1] = p1.Y;
        input[2] = p2.X;
        input[3] = p2.Y;
        bool success;
        assembly {
            success := call(sub(gas, 2000), 6, 0, input, 0xc0, r, 0x60)
            // Use "invalid" to make gas estimation work
            switch success case 0 { invalid() }
        }
        require(success);
    }
    /// @return the product of a point on G1 and a scalar, i.e.
    /// p == p.scalar_mul(1) and p.addition(p) == p.scalar_mul(2) for all points p.
    function scalar_mul(G1Point p, uint s) internal returns (G1Point r) {
        uint[3] memory input;
        input[0] = p.X;
        input[1] = p.Y;
        input[2] = s;
        bool success;
        assembly {
            success := call(sub(gas, 2000), 7, 0, input, 0x80, r, 0x60)
            // Use "invalid" to make gas estimation work
            switch success case 0 { invalid() }
        }
        require (success);
    }
    /// @return the result of computing the pairing check
    /// e(p1[0], p2[0]) *  .... * e(p1[n], p2[n]) == 1
    /// For example pairing([P1(), P1().negate()], [P2(), P2()]) should
    /// return true.
    function pairing(G1Point[] p1, G2Point[] p2) internal returns (bool) {
        require(p1.length == p2.length);
        uint elements = p1.length;
        uint inputSize = elements * 6;
        uint[] memory input = new uint[](inputSize);
        for (uint i = 0; i < elements; i++)
        {
            input[i * 6 + 0] = p1[i].X;
            input[i * 6 + 1] = p1[i].Y;
            input[i * 6 + 2] = p2[i].X[0];
            input[i * 6 + 3] = p2[i].X[1];
            input[i * 6 + 4] = p2[i].Y[0];
            input[i * 6 + 5] = p2[i].Y[1];
        }
        uint[1] memory out;
        bool success;
        assembly {
            success := call(sub(gas, 2000), 8, 0, add(input, 0x20), mul(inputSize, 0x20), out, 0x20)
            // Use "invalid" to make gas estimation work
            switch success case 0 { invalid() }
        }
        require(success);
        return out[0] != 0;
    }
    /// Convenience method for a pairing check for two pairs.
    function pairingProd2(G1Point a1, G2Point a2, G1Point b1, G2Point b2) internal returns (bool) {
        G1Point[] memory p1 = new G1Point[](2);
        G2Point[] memory p2 = new G2Point[](2);
        p1[0] = a1;
        p1[1] = b1;
        p2[0] = a2;
        p2[1] = b2;
        return pairing(p1, p2);
    }
    /// Convenience method for a pairing check for three pairs.
    function pairingProd3(
            G1Point a1, G2Point a2,
            G1Point b1, G2Point b2,
            G1Point c1, G2Point c2
    ) internal returns (bool) {
        G1Point[] memory p1 = new G1Point[](3);
        G2Point[] memory p2 = new G2Point[](3);
        p1[0] = a1;
        p1[1] = b1;
        p1[2] = c1;
        p2[0] = a2;
        p2[1] = b2;
        p2[2] = c2;
        return pairing(p1, p2);
    }
    /// Convenience method for a pairing check for four pairs.
    function pairingProd4(
            G1Point a1, G2Point a2,
            G1Point b1, G2Point b2,
            G1Point c1, G2Point c2,
            G1Point d1, G2Point d2
    ) internal returns (bool) {
        G1Point[] memory p1 = new G1Point[](4);
        G2Point[] memory p2 = new G2Point[](4);
        p1[0] = a1;
        p1[1] = b1;
        p1[2] = c1;
        p1[3] = d1;
        p2[0] = a2;
        p2[1] = b2;
        p2[2] = c2;
        p2[3] = d2;
        return pairing(p1, p2);
    }
}
contract Verifier {
    using Pairing for *;
    struct VerifyingKey {
        Pairing.G2Point A;
        Pairing.G1Point B;
        Pairing.G2Point C;
        Pairing.G2Point gamma;
        Pairing.G1Point gammaBeta1;
        Pairing.G2Point gammaBeta2;
        Pairing.G2Point Z;
        Pairing.G1Point[] IC;
    }
    struct Proof {
        Pairing.G1Point A;
        Pairing.G1Point A_p;
        Pairing.G2Point B;
        Pairing.G1Point B_p;
        Pairing.G1Point C;
        Pairing.G1Point C_p;
        Pairing.G1Point K;
        Pairing.G1Point H;
    }
    function verifyingKey() pure internal returns (VerifyingKey vk) {
        vk.A = Pairing.G2Point([0x216a10343567060d9c5aa19062dd6f6a98f647b6b87e16ddcf2e74d64ef5649a, 0x2fb2ebe7b66bf458dcd1042cea2addeaee339a6286b9fc2b8fc814ca96fa1ad2], [0x1f4cf4394e243f1736fba7c2479b9c0c5f2ac3ed8b942c6c31fb3f6c9f687a86, 0x14f54230e513deb14344b46b2e39224a94bacf77822990ecc014b2cf9609dc1f]);
        vk.B = Pairing.G1Point(0x18214eb18afad905b3a425d44628fbbb75d8d6a9ac32de9e62965ad4bc765c4e, 0x2bd71a090dfee0d8657da3be7ea7d393f2e3f16ea3ae9e4b7dfefd12af4a6527);
        vk.C = Pairing.G2Point([0xcf878b68586fd778646618cb7ce2a0bcf09dbdd05e93b0265574d5e224157bc, 0x13ba26801a1f9e81c83fd77731520de6f4fa218f78c417551db6bb8c0df2623c], [0x2c98889f5c9cf43fd131d3d8419aefae94632fa062cce583697df22e4f7b8d9b, 0x10b7d8022f83abb4ee5d1d9048e6d93df6912dba046adcbd4828efecdec8d152]);
        vk.gamma = Pairing.G2Point([0x147cfa4953ab56271e0f3ec8f851f749eb053a83e931cc1b6e5d586060dd50b8, 0x4d8b4c8134de376548cdb2c7aad869096c507f79e96fff991bec8b4b8173acd], [0x2c054fcabe18482d2a2437131c9d8973c1a88af9b6f9f427ecb6035611b38b82, 0x144529a7510bf7f5e96b8b30ef5967e0c96b626df264d7fc86f43f3eef5b68a9]);
        vk.gammaBeta1 = Pairing.G1Point(0x87bcdbbb3f2dea08bfef662e7539e8a7433195412fa80d4294acd432d9ae5ac, 0x817c356aa2767854dd680810971bbbc9d19c642126de20da0e3591a47f42af5);
        vk.gammaBeta2 = Pairing.G2Point([0x210e9d7a9098a956c638b72c4f9b1735541226f043be2e99444ee63327682b6b, 0x24ec3fceb9ae51e5170b52709af6aff275d37e5e8caa8f2e3df37ae702514e91], [0x9c58fb2d6804e094ec2b5a7bb6ed757d50437fce9cee12f4f2d8c59dde78380, 0x71e1db60b2ecc13f5c765f6f69ad98181c41ba7f85d99a788166652c3a00830]);
        vk.Z = Pairing.G2Point([0xe3c8e2eae4458929783a61db513f30dccafe6ffe6c6a892b0c3c35f37c9626f, 0xcaa960d603d6096a97622543b3e5ddc20e0420155b390c1d9821dd8e5cdb901], [0x2d73a0a77c8b7ad665dfec443e6d4baa3c36d11b9de5cc95c86a8c404b70d281, 0xa1bcf44cf0271d969e58c8aa8ccef16c304af4d3706e911408bc0386e6b724c]);
        vk.IC = new Pairing.G1Point[](34);
        vk.IC[0] = Pairing.G1Point(0x15ea8ae23b0d3b847585277a8efdc945078cd8738403ecdb71214aed707fbce2, 0x2efec4d2c136bdd8461229092168755f5ae3b3106ea41b4b8d23a19ab1ebb632);
        vk.IC[1] = Pairing.G1Point(0x1221e462191f41ac1ec7aad93725846d5955707866a90000ae727a0c092720ab, 0x299c64e1da472e371f653cfacdc3fc66bee78a97f35a1a38175c89dc586b4998);
        vk.IC[2] = Pairing.G1Point(0x28e59acdff97546234b9e154d3fa8b47ef19a406fc1f2e120b84f014834ac9fa, 0x11a0f3d822ec11748b5f4732cceeff4035f228c53718adddc99b6b91623222db);
        vk.IC[3] = Pairing.G1Point(0xc8c8a472e98b93992f1f67a11862ffe40079ea3f49834182ff4c244dee85649, 0x2f63dc37a55205d82ebe9b073997e1dea032bb9d4963068da322bea2ff7b1ef4);
        vk.IC[4] = Pairing.G1Point(0x161b669228785d8c2558fa8fc4b449cbc8c444240ffcce1ed53592600139eedc, 0x207a9271b5ca15c4c7396ceac40119f8a4f271d36f68f16e7642714a95b00017);
        vk.IC[5] = Pairing.G1Point(0x3530a6261e6c650eadc9096323eb747710d4124612d484247efe74d535296fb, 0x1b4e40eb65444adc8b597b8a594e6f19fce49d4a371dbd221b0ea3d0920eb53f);
        vk.IC[6] = Pairing.G1Point(0x1ca11ed14232201820a47f998fc3d20b03640a289ebf8c691680f9bfab4a5b86, 0x1026fdede7bc5ca3c82dd37826a0bbde81b7fbcd861abc077ab9c820781363b5);
        vk.IC[7] = Pairing.G1Point(0x2bca43276c20224ff7eab109a24b4180447630d5191585f7916d878779396865, 0x16ee5c5399f4c1a97e15991a8c11956843ff8efcbd64faeb2489a9ad5cdcbfa2);
        vk.IC[8] = Pairing.G1Point(0x2db97fc7afbded0736d04a90277b4a09dd7291df5453b708990a794106de0bf6, 0x2fa3745284ceb24d3c72da893ea1bb0829ad15166bd564fa7e32c2d8d6498c70);
        vk.IC[9] = Pairing.G1Point(0x1ab479e3f0cd5bbcc566049c47c1344f828e13c90150aaec33abee90b439c1c1, 0x1ffcf393f3aeef134cd076dbec577c3281e53825921c7e61c3e82a2af0dca2e6);
        vk.IC[10] = Pairing.G1Point(0x2c91b5ddc69c158743547cd2cbeef6b4a7de7312243604f02786692e66ab0234, 0xa838388a5423559d9b1c70650ce1b6555f10875a23f8caaa8c0ae0c6897151);
        vk.IC[11] = Pairing.G1Point(0x107d11d3a181b7c70f1fdd16d760a4c128910c1278165327245e450d950c66a3, 0x871ff0e31965ae6a6909a44669fa6c97c1003ceec73fe46ee9aa9c61427f939);
        vk.IC[12] = Pairing.G1Point(0xb9cbec937529223ded3e585391892fabbd226ab2f40d6e2de111d7c669aaaa3, 0x301961b86c155ef422992200353440fb3bbaf5ca68e42486e9a38d3272a5b641);
        vk.IC[13] = Pairing.G1Point(0x281256b822ff4451deda4df078aa19ebfdd1e2424cb9a1dea643fb3067e2bd73, 0x7d031fdc899aea055fed3fd489fcd95b41b7565caef12f25a3c4170372d3e37);
        vk.IC[14] = Pairing.G1Point(0x1bf984b043b40c11b3eea36ce8e679163f0a87ed0436bf6a70c40ba5acc2b4a7, 0x21d37470743c6521de091d136d6faee4b40a9da3ee393db1e81b61f4ff6f60df);
        vk.IC[15] = Pairing.G1Point(0x1a08f64c49f4c92eb7b1d13abdb6bd6686504395166fa2ad7fa0f4b03e8f5659, 0x20ebb9767268a2280bc184f2ac99a5d21821e0d8a7ab2e4cec68cf24f2be3233);
        vk.IC[16] = Pairing.G1Point(0x20034aaa05c197769d0c54c16bd025e2c0e6f5f0bfc3d43b21299e529e4e7423, 0xcb9cbd13d7b7bbdf45819837f2ec8d3c62245d407ebaa7859023aca2db419ce);
        vk.IC[17] = Pairing.G1Point(0x16781e886ac08d18304e9e2d85eab23d3120eb27366b115b482790dd40cf859c, 0x1ea411afb043337e789e7f43f07190e8f8b37e3c12d94bdbb07deb0253f2c895);
        vk.IC[18] = Pairing.G1Point(0x1bb282dfca95b99c25022576057b7f7037e5aeb325fdc6fc38dc4017ed3b2e52, 0x28be6f6fe5ba76c1780e6fb7b9df12c7ff20a3f08840557369d249c55019551c);
        vk.IC[19] = Pairing.G1Point(0x11eeadbfd6a7d9e06aa2c48914e64c3b862bdb23c4cb421439d950f3ba831756, 0x24324451b4a1ade10ee6c40d98a9edf82b39cd300484336ee879fb729e62c8fb);
        vk.IC[20] = Pairing.G1Point(0x2427496e43b1aa0253ec50af8c1fb1e6135e218199b3fd5dbed6001c8b659636, 0x10751ba3d5e068eed7e1617d859ffab438d48918c922cd369f0775e8a18154be);
        vk.IC[21] = Pairing.G1Point(0xca7c15998c3af54c73d64d76356d1464b6b013d8c15a5bbce217a824b2daeba, 0x1a2af7810811ad398e062f163957029090ad509d2c23beea91ecd5a9596b510e);
        vk.IC[22] = Pairing.G1Point(0x2df4ee35c38a8a5b9d847643ef5258230bd78e30e4b09960ebd9f30afebfa9e2, 0x2848d1e9535ceaa11039f5e73ccf9e4d03ff36ccbd6488f08678fd28673af521);
        vk.IC[23] = Pairing.G1Point(0xb7c8f0e18544f04dc8531264ba8c864db208d6a25549e9b48d083764de8ab8e, 0x2a351a2fb3271a0e0af6c285fade937c140a40a55de991007bc1e0b30971ad65);
        vk.IC[24] = Pairing.G1Point(0x1e7ef6afae98b05f16d1aa62a1aa37d755404a13c65b09eae24d9ed08d8adb48, 0x6702eddbd49634467948254b043f9b5af2e1729fcb24028e4c235d82da66eb0);
        vk.IC[25] = Pairing.G1Point(0x123f2d106831b8822de4bb1ed02f794a48ad3db2db2ec88e1b9019746f3be2e1, 0x4cfc7d59522b23f6818cd7997a18bf62fadcebee34a89ce21747638c58b9a34);
        vk.IC[26] = Pairing.G1Point(0x25a271e6222429dbbd2f0ba842150003fff5b000ed8d500015d89ef895f8ebe0, 0x22a23ae5ab272d35d4e68b175a91b06a7081f0358a8d45d9107f61469a5500f9);
        vk.IC[27] = Pairing.G1Point(0x164bc1f97cea9fadeff6549d6bbae9289dbdbb4b5b3a869373bef2f8382bae89, 0x11fbddbb860c7754cd5dae870f9f7c548421f69a106a8240ad7c0e69d059bbd8);
        vk.IC[28] = Pairing.G1Point(0x6d12383035f5f3c3a5af549a8fb7f8a24796ca5b89f9a9a6e51bbb6f7c9ce18, 0x1753171de3103a5bd09edf276b023fa390a03fa164e5c583afe9e52e5778c8f6);
        vk.IC[29] = Pairing.G1Point(0x219391a94268d619f4a26026ff28a7fab581e6b12c70840c75834f84325de6c4, 0x2ff76af8aae75116630e2cdda38488d999adfa9dd867877a11a2a3e1f957cc8d);
        vk.IC[30] = Pairing.G1Point(0x27606477f51de070246811d0f4ed0b6c187b2787df6b660b431e3040cef5d30c, 0x17b5e02929d408ad3884bc6004ad81690f4809d34a8498c0d6716f03f7714225);
        vk.IC[31] = Pairing.G1Point(0x9ef8a9b88799b7cbd6b9a314d126424737faade4e3962436ea4eed998961f2d, 0x144a922f8389181c3e188d65824ca79189756659c1863f062c1ae4b56e0ec887);
        vk.IC[32] = Pairing.G1Point(0xc77c4460484c516393f58d1f33a22218e66e9fb6cdffefb78ff227e812b6440, 0xc87b787ee1512a2ad2a3e6da4e96a986a06993ec0d815767d518486b6119a32);
        vk.IC[33] = Pairing.G1Point(0x23dce1bdbce6b4cb3e56b38b04419f0d494446798faad4116c01a483a81f0ffb, 0x24af7550a27750c653c9592707014c58dae9174ac5833bca607f1fc01719f75f);
    }
    function verify(uint[] input, Proof proof) internal returns (uint) {
        VerifyingKey memory vk = verifyingKey();
        require(input.length + 1 == vk.IC.length);
        // Compute the linear combination vk_x
        Pairing.G1Point memory vk_x = Pairing.G1Point(0, 0);
        for (uint i = 0; i < input.length; i++)
            vk_x = Pairing.addition(vk_x, Pairing.scalar_mul(vk.IC[i + 1], input[i]));
        vk_x = Pairing.addition(vk_x, vk.IC[0]);
        if (!Pairing.pairingProd2(proof.A, vk.A, Pairing.negate(proof.A_p), Pairing.P2())) return 1;
        if (!Pairing.pairingProd2(vk.B, proof.B, Pairing.negate(proof.B_p), Pairing.P2())) return 2;
        if (!Pairing.pairingProd2(proof.C, vk.C, Pairing.negate(proof.C_p), Pairing.P2())) return 3;
        if (!Pairing.pairingProd3(
            proof.K, vk.gamma,
            Pairing.negate(Pairing.addition(vk_x, Pairing.addition(proof.A, proof.C))), vk.gammaBeta2,
            Pairing.negate(vk.gammaBeta1), proof.B
        )) return 4;
        if (!Pairing.pairingProd3(
                Pairing.addition(vk_x, proof.A), proof.B,
                Pairing.negate(proof.H), vk.Z,
                Pairing.negate(proof.C), Pairing.P2()
        )) return 5;
        return 0;
    }
    event Verified(string s);
    function verifyTx(
            uint256 segmentId, 
            uint[2] a,
            uint[2] a_p,
            uint[2][2] b,
            uint[2] b_p,
            uint[2] c,
            uint[2] c_p,
            uint[2] h,
            uint[2] k,
            uint[17] input
        ) public returns (bool r) {
        Proof memory proof;
        proof.A = Pairing.G1Point(a[0], a[1]);
        proof.A_p = Pairing.G1Point(a_p[0], a_p[1]);
        proof.B = Pairing.G2Point([b[0][0], b[0][1]], [b[1][0], b[1][1]]);
        proof.B_p = Pairing.G1Point(b_p[0], b_p[1]);
        proof.C = Pairing.G1Point(c[0], c[1]);
        proof.C_p = Pairing.G1Point(c_p[0], c_p[1]);
        proof.H = Pairing.G1Point(h[0], h[1]);
        proof.K = Pairing.G1Point(k[0], k[1]);
        uint[] memory inputValues = new uint[](input.length + 16);
        for(uint i = 0; i < input.length; i++){
            inputValues[i] = input[i];
        }
  
        appendChallengeHash(segmentId, inputValues);

        if (verify(inputValues, proof) == 0) {
            emit Verified("Transaction successfully verified.");
            transferReward(segmentId, msg.sender);
            return true;
        } else {
            return false;
        }
    }

// Manually added
   struct Segment {      
	uint256     reward;
	uint[16] signed_hash;
   }
	
    mapping(uint256 => Segment) segments;
    mapping(uint256 => Segment) disputes;

    function addSegmentEscrow(uint256 segmentId, uint256[16] _signed_hash) public payable {
	uint256 _reward = msg.value;
	require(_reward > 0);

	Segment memory segment;
       for (uint i = 0; i < _signed_hash.length; i++){
	  segment.signed_hash[i] = _signed_hash[i]; 
       }
       segment.reward = _reward;
       segments[segmentId] = segment;
    }
   
   function appendChallengeHash(uint256 segmentId,uint[] inputValues) public {
     Segment storage segment = segments[segmentId]; 
     require(segment.signed_hash.length > 0);
     for(uint i = 0; i < 16; i++){
            inputValues[17+i] = segment.signed_hash[i];
      }
  }
  
  function transferReward(uint256 segmentId, address miner) {
     Segment storage segment = segments[segmentId]; 
     require(segment.signed_hash.length > 0);
     require(segment.reward > 0);
     miner.transfer(segment.reward);
     delete segments[segmentId];
  }
}

