#include <stdlib.h>
#include <iostream>

#include "libff/algebra/fields/field_utils.hpp"
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>
#include "libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp"
#include "libsnark/common/default_types/r1cs_ppzksnark_pp.hpp"
#include "libsnark/gadgetlib1/pb_variable.hpp"

//#include "gadget.hpp"
#include "libsnark/gadgetlib1/gadgets/pairing/pairing_checks.hpp"
#include <libsnark/gadgetlib1/gadgets/pairing/pairing_params.hpp>
#include <libsnark/gadgetlib1/gadgets/curves/weierstrass_g1_gadget.hpp>
#include <libsnark/gadgetlib1/gadgets/curves/weierstrass_g2_gadget.hpp>

#include "util.hpp"

// Test
#include <libsnark/relations/variable.hpp>

typedef libff::mnt4_pp ppT;
typedef libff::Fr<ppT> FieldT;

//
// gadget for video coin transcode verification.
// P1,P2 represents public input (source stream pHash x encoded as P2 = x*P1)
// Q1, Q2 represents private input (Trnascode stream pHash y encoded as Q1=y*Q2
// The gadget generates proving and verification keys.
// The class also includes methods for proof generation and verification( for test purpose).
template<typename ppT>
class vid_check_e_equals_e_gadget: public gadget<libff::Fr<ppT> > {
public:
	// Create protoboard

	typedef libff::Fr<ppT> FieldT;

	G1_variable<ppT> P1;
	G2_variable<ppT> Q1;
	G1_variable<ppT> P2;
	G2_variable<ppT> Q2;

	G1_precomputation<ppT> prec_P1;
	G2_precomputation<ppT> prec_Q1;
	G1_precomputation<ppT> prec_P2;
	G2_precomputation<ppT> prec_Q2;

	precompute_G1_gadget<ppT> compute_prec_P1;
	precompute_G2_gadget<ppT> compute_prec_Q1;
	precompute_G1_gadget<ppT> compute_prec_P2;
	precompute_G2_gadget<ppT> compute_prec_Q2;

	Fqk_variable<ppT> miller_result;
	mnt_e_over_e_miller_loop_gadget<ppT> miller;
	pb_variable<FieldT> result_is_one;
	final_exp_gadget<ppT> finexp;

	vid_check_e_equals_e_gadget(protoboard<FieldT> &pb,
			const std::string &annotation_prefix);
	r1cs_ppzksnark_keypair<ppT> generate_keys(protoboard<FieldT> &pb);
	r1cs_ppzksnark_proof<ppT> generate_proof(protoboard<FieldT> &pb,
			r1cs_ppzksnark_keypair<ppT> keypair,
			libff::G1<other_curve<ppT> > lhs_G1_val,
			libff::G2<other_curve<ppT> > lhs_G2_val,
			libff::G1<other_curve<ppT> > rhs_G1_val,
			libff::G2<other_curve<ppT> > rhs_G2_val);

	int verify(protoboard<FieldT> &pb, r1cs_ppzksnark_keypair<ppT> keypair,
			const r1cs_ppzksnark_proof<ppT> proof);
};

// Setup
template<typename ppT>
vid_check_e_equals_e_gadget<ppT>::vid_check_e_equals_e_gadget(
		protoboard<FieldT> &pb, const std::string &annotation_prefix) :
		gadget<FieldT>(pb, annotation_prefix), P1(pb,
				FMT(annotation_prefix, "P1")), Q1(pb,
				FMT(annotation_prefix, "Q1")), P2(pb,
				FMT(annotation_prefix, "P2")), Q2(pb,
				FMT(annotation_prefix, "Q2")), compute_prec_P1(
				precompute_G1_gadget<ppT>(pb, P1, prec_P1, "compute_prec_P1")), compute_prec_Q1(
				precompute_G2_gadget<ppT>(pb, Q1, prec_Q1, "compute_prec_Q1")), compute_prec_P2(
				precompute_G1_gadget<ppT>(pb, P2, prec_P2, "compute_prec_P2")), compute_prec_Q2(
				precompute_G2_gadget<ppT>(pb, Q2, prec_Q2, "compute_prec_Q2")), miller_result(
				pb, "miller_result"), miller(
				mnt_e_over_e_miller_loop_gadget<ppT>(pb, prec_P1, prec_Q1,
						prec_P2, prec_Q2, miller_result, "miller")), finexp(
				final_exp_gadget<ppT>(pb, miller_result, result_is_one,
						"finexp")) {
	result_is_one.allocate(pb, "result_is_one");
	compute_prec_P1.generate_r1cs_constraints();
	compute_prec_Q1.generate_r1cs_constraints();
	compute_prec_P2.generate_r1cs_constraints();
	compute_prec_Q2.generate_r1cs_constraints();
	miller.generate_r1cs_constraints();
	finexp.generate_r1cs_constraints();

}

// Generate keys
template<typename ppT>
r1cs_ppzksnark_keypair<ppT> vid_check_e_equals_e_gadget<ppT>::generate_keys(
		protoboard<FieldT> &pb) {
	result_is_one.allocate(pb, "result_is_one");
	compute_prec_P1.generate_r1cs_constraints();
	compute_prec_Q1.generate_r1cs_constraints();
	compute_prec_P2.generate_r1cs_constraints();
	compute_prec_Q2.generate_r1cs_constraints();
	miller.generate_r1cs_constraints();
	finexp.generate_r1cs_constraints();

	const r1cs_constraint_system<FieldT> cs = pb.get_constraint_system();
	const r1cs_ppzksnark_keypair<ppT> keypair = r1cs_ppzksnark_generator<ppT>(
			cs);

	cout << "Number of R1CS constraints: " << cs.num_constraints() << endl;

	return keypair;
}

// generate proof
template<typename ppT>
r1cs_ppzksnark_proof<ppT> vid_check_e_equals_e_gadget<ppT>::generate_proof(
		protoboard<FieldT> &pb, r1cs_ppzksnark_keypair<ppT> keypair,
		libff::G1<other_curve<ppT> > lhs_G1_val,
		libff::G2<other_curve<ppT> > lhs_G2_val,
		libff::G1<other_curve<ppT> > rhs_G1_val,
		libff::G2<other_curve<ppT> > rhs_G2_val) {
	P1.generate_r1cs_witness(lhs_G1_val);
	compute_prec_P1.generate_r1cs_witness();
	Q1.generate_r1cs_witness(lhs_G2_val);
	compute_prec_Q1.generate_r1cs_witness();
	P2.generate_r1cs_witness(rhs_G1_val);
	compute_prec_P2.generate_r1cs_witness();
	Q2.generate_r1cs_witness(rhs_G2_val);
	compute_prec_Q2.generate_r1cs_witness();

	miller.generate_r1cs_witness();
	finexp.generate_r1cs_witness();
	assert(pb.is_satisfied());

	const r1cs_ppzksnark_proof<ppT> proof = r1cs_ppzksnark_prover<ppT>(
			keypair.pk, pb.primary_input(), pb.auxiliary_input());

	cout << "Primary (public) input: " << pb.primary_input() << endl;
	cout << "Auxiliary (private) input: " << pb.auxiliary_input() << endl;
	const r1cs_ppzksnark_verification_key<ppT> vk = keypair.vk;
	return proof;
}

// verify proof
template<typename ppT>
int vid_check_e_equals_e_gadget<ppT>::verify(protoboard<FieldT> &pb,
		r1cs_ppzksnark_keypair<ppT> keypair,
		const r1cs_ppzksnark_proof<ppT> proof) {
	bool verified = r1cs_ppzksnark_verifier_strong_IC<ppT>(keypair.vk,
			pb.primary_input(), proof);

	cout << "Primary (public) input: " << pb.primary_input() << endl;
	cout << "Verification status: " << verified << endl;
	const r1cs_ppzksnark_verification_key<ppT> vk = keypair.vk;
	return verified;
}

int main() {
	// Initialize the curve parameters
	libff::mnt4_pp::init_public_params();
	libff::mnt6_pp::init_public_params();
	default_r1cs_ppzksnark_pp::init_public_params();
	protoboard<FieldT> pb;

	vid_check_e_equals_e_gadget<ppT> g(pb, "test");
	pb.set_input_sizes(1);
	r1cs_ppzksnark_keypair<ppT> keypair = g.generate_keys(pb);

	cout << "num_inputs: " << pb.num_inputs() << endl;
	cout << "num_variables: " << pb.num_variables() << endl;
	cout << "num_constraints: " << pb.num_constraints() << endl;
	cout << "Primary (public) input: " << pb.primary_input() << endl;
	cout << "Auxiliary (private) input: " << pb.auxiliary_input() << endl;
	/*
	 cout << "Verification status: " << verified << endl;
	 */
	const r1cs_ppzksnark_verification_key<ppT> vk = keypair.vk;

	//print_vk_to_file<ppT>(vk, "./vk_data");
	/*
	 print_proof_to_file<ppT>(proof, "./proof_data");
	 */
	printf("number of constraints for full pairing (Fr is)  = %zu\n",
			pb.num_constraints());

	unsigned long a = 2;
	unsigned long b = 2;
	libff::Fr<other_curve<ppT> > x = libff::Fr<other_curve<ppT> >::one() * a;
	libff::Fr<other_curve<ppT> > y = libff::Fr<other_curve<ppT> >::one() * b;

	libff::G1<other_curve<ppT> > lhs_G1_val =
			libff::Fr<other_curve<ppT> >::one()
					* libff::G1<other_curve<ppT> >::one();
	libff::G2<other_curve<ppT> > lhs_G2_val = y
			* libff::G2<other_curve<ppT> >::one();

	libff::G1<other_curve<ppT> > rhs_G1_val = x
			* libff::G1<other_curve<ppT> >::one();
	libff::G2<other_curve<ppT> > rhs_G2_val =
			libff::Fr<other_curve<ppT> >::one()
					* libff::G2<other_curve<ppT> >::one();

	r1cs_ppzksnark_proof<ppT> proof = g.generate_proof(pb, keypair, lhs_G1_val,
			lhs_G2_val, rhs_G1_val, rhs_G2_val);

	g.verify(pb, keypair, proof);
}
