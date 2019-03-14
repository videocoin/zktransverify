#include <libff/common/default_types/ec_pp.hpp>
#include <libff/common/profiling.hpp>
#include <libff/common/utils.hpp>

#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>

using namespace libsnark;

std::vector<unsigned long> bytes_to_words(std::vector<uint8_t> &bytes)
{
    assert(bytes.size() % 4 == 0);
    std::vector<unsigned long> v;
    for (size_t i = 0; i < bytes.size() / 4; ++i)
    {
        v.emplace_back((bytes[i*4 + 3] << 24) + (bytes[i*4+2] << 16) + (bytes[i*4+1] << 8) + bytes[i*4]);
    }

    return v;
}

template<typename FieldT>
void test_original_video_hash_plus_transcoded_video_hash()
{
    protoboard<FieldT> pb;

    digest_variable<FieldT> left(pb, SHA256_digest_size, "original");
    digest_variable<FieldT> right(pb, SHA256_digest_size, "transcoded");
    digest_variable<FieldT> output(pb, SHA256_digest_size, "output");

    sha256_two_to_one_hash_gadget<FieldT> f(pb, left, right, output, "f");
    f.generate_r1cs_constraints();
    printf("Number of constraints for sha256_two_to_one_hash_gadget: %zu\n", pb.num_constraints());

    std::vector<uint8_t> original_video = { 0xd8, 0xc2, 0x6b, 0x42, 0x82, 0x67, 0xc8, 0x4d, 0x7a, 0x95, 0xe8, 0x81, 0x48, 0xc1, 0x9e, 0x40, 0xe8, 0xfb, 0xcf, 0xe6, 0x4f, 0xba, 0xe6, 0xaf, 0x78, 0x19, 0x6f, 0x9c, 0xe9, 0xf7, 0x7a, 0xdd };
    std::vector<uint8_t> transcoded_video = { 0x42, 0xce, 0x8c, 0x03, 0xb8, 0x66, 0xd3, 0xab, 0x00, 0x7e, 0xde, 0x3e, 0x53, 0xde, 0x30, 0x91, 0x3d, 0xf7, 0xcd, 0x72, 0x14, 0x51, 0x82, 0xee, 0x1b, 0x8d, 0xb4, 0x8c, 0xd0, 0x8a, 0xf6, 0x9a };
    std::vector<uint8_t> output_hash = { 0x7f, 0x0b, 0xfd, 0xef, 0x16, 0xa1, 0xcb, 0x1c, 0xf7, 0x16, 0xe8, 0x2e, 0x48, 0x2b, 0xc6, 0x31, 0x41, 0x51, 0x30, 0x59, 0x0a, 0x5c, 0x0e, 0x99, 0x3d, 0xd3, 0x40, 0xce, 0xd1, 0x67, 0x11, 0x0b };

    const libff::bit_vector left_bv = libff::int_vector_to_bits(bytes_to_words(original_video), 32);
    const libff::bit_vector right_bv = libff::int_vector_to_bits(bytes_to_words(transcoded_video), 32);
    const libff::bit_vector hash_bv = libff::int_vector_to_bits(bytes_to_words(output_hash), 32);

    left.generate_r1cs_witness(left_bv);
    right.generate_r1cs_witness(right_bv);

    f.generate_r1cs_witness();
    output.generate_r1cs_witness(hash_bv);

    assert(pb.is_satisfied());

    const r1cs_constraint_system<FieldT> constraint_system = pb.get_constraint_system();
    const r1cs_ppzksnark_keypair<default_r1cs_ppzksnark_pp> keypair = r1cs_ppzksnark_generator<default_r1cs_ppzksnark_pp>(constraint_system);
    const r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof = r1cs_ppzksnark_prover<default_r1cs_ppzksnark_pp>(keypair.pk, pb.primary_input(), pb.auxiliary_input());
    bool verified = r1cs_ppzksnark_verifier_strong_IC<default_r1cs_ppzksnark_pp>(keypair.vk, pb.primary_input(), proof);

    assert(verified);
}

int main(void)
{
    libff::start_profiling();
    libff::default_ec_pp::init_public_params();
    test_original_video_hash_plus_transcoded_video_hash<libff::Fr<libff::default_ec_pp> >();
}
