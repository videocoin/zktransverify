pragma solidity ^0.4.24;

contract Verify {
    
	struct Segment {      
		uint256    reward;
		uint256    public_key;
		uint256    signed_hash_pt_x;
		uint256    signed_hash_pt_y;		
	}
	
    mapping(uint256 => Segment) segments;
    mapping(uint256 => Segment) disputes;
    
    function ecmul(uint256 x, uint256 y, uint256 scalar) public view returns (uint256 ox, uint256 oy) {
        // With a public key (x, y), this computes p = scalar * (x, y).
        uint256[3] memory input;
        uint256[2] memory output;
        input[0] = x;
        input[1] = y;
        input[2] = scalar;
        assembly {
            // call ecmul precompile
            if iszero(call(not(0), 0x07, 0, input, 0x60,output, 0x40)) {
                revert(0, 0)
            }
        }
        ox = output[0];
        oy = output[1];
    }
    
    function addSegmentEscrow(uint256 segmentId, uint256 _signed_hash_pt_x, uint256 _signed_hash_pt_y, uint256 _public_key) public payable {
		uint256 _reward = msg.value;
		require(_reward > 0);
		require(segments[segmentId].public_key != 0);

		
		Segment memory segment = Segment({
				public_key : _public_key,
				signed_hash_pt_x : _signed_hash_pt_x,
				signed_hash_pt_y : _signed_hash_pt_y,
				reward : _reward
			});
		segments[segmentId] = segment;
    }
    
    function claimSegmentEscrow(uint256 segmentId, uint256 hash_pt_x, uint256 hash_pt_y) public {
		require(segments[segmentId].public_key > 0);
		Segment storage segment = segments[segmentId];
		uint256 signed_hash_pt_2_x = 0;
		uint256 signed_hash_pt_2_y = 0;
    	(signed_hash_pt_2_x, signed_hash_pt_2_y) = ecmul(hash_pt_x, hash_pt_y, segment.public_key);
    	if(signed_hash_pt_2_x == segment.signed_hash_pt_x && signed_hash_pt_2_y == segment.signed_hash_pt_y){
    		// Transfer funds
    	} else {
    		// add to disputed
    	}
    }
    
}