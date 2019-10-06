//
// Created by taras on 12.04.19.
//

#ifndef COMP_DEFS_H
#define COMP_DEFS_H

#include <gmp.h>
#include <cstdint>
#include <fstream>
#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
#include <libff/algebra/curves/public_params.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#define INIT_MPZ_BITS 128

namespace pt = boost::property_tree;

struct comp_params {
    int n_constraints;
    int n_inputs;
    int n_outputs;
    int n_vars;
};

comp_params parse_params(const char *params_filename);

void convert_to_z(mpz_t z, const mpq_t q, const mpz_t prime);

void convert_to_z(int size, mpz_t *z, const mpq_t *q, const mpz_t prime);

void alloc_init_scalar(mpz_t s);

void alloc_init_scalar(mpq_t s);

void alloc_init_vec(mpz_t **arr, uint32_t size);

void alloc_init_vec(mpq_t **arr, uint32_t size);

void clear_scalar(mpz_t);

void clear_scalar(mpq_t);

void clear_del_vec(mpz_t *vec, uint32_t n);

void clear_del_vec(mpq_t *vec, uint32_t n);

void assert_zero(int value);

#ifdef CURVE_ALT_BN128
std::string coord_to_string(libff::alt_bn128_Fq2 &coord);
std::string coord_to_string(libff::alt_bn128_Fq &coord);

libff::alt_bn128_Fq string_to_coord(const char *str);
libff::alt_bn128_Fq2 string_to_coord(const char *c0, const char *c1);
#endif

template<typename ppt>
pt::ptree point_to_ptree(libff::G1<ppt> &point) {
    pt::ptree x_node, y_node;
    pt::ptree node;

    auto p = libff::G1<ppt>(point);
    p.to_affine_coordinates();

#ifdef CURVE_BN128
    x_node.put("", p.X.toString(16));
    y_node.put("", p.Y.toString(16));
#elif CURVE_ALT_BN128
    x_node.put("", coord_to_string(p.X));
    y_node.put("", coord_to_string(p.Y));
#endif

    node.push_back(std::make_pair("", x_node));
    node.push_back(std::make_pair("", y_node));

    return node;
}

template<typename ppT>
libff::G1<ppT> ptree_to_point(pt::ptree &root) {
    std::string point_str[2];
    int i = 0;
    for (pt::ptree::value_type &coord : root) {
        point_str[i] = coord.second.data();
        i++;
    }

    libff::G1<ppT> point(string_to_coord(point_str[0].c_str()), string_to_coord(point_str[1].c_str()), libff::alt_bn128_Fq::one());

    return point;
}

template<typename ppT>
libff::G2<ppT> ptree_to_point2(pt::ptree &root) {
    std::string point_str[2][2];
    int i = 0;
    for (pt::ptree::value_type &point : root) {
        int j = 0;
        for (auto &coord: point.second) {
            point_str[i][j] = coord.second.data();
            j++;
        }
        i++;
    }

    libff::G2<ppT> point(string_to_coord(point_str[0][0].c_str(), point_str[0][1].c_str()),
                        string_to_coord(point_str[1][0].c_str(), point_str[1][1].c_str()), libff::alt_bn128_Fq2::one());
    return point;
}

template<typename ppt>
pt::ptree point_to_ptree(libff::G2<ppt> &point) {
    pt::ptree x_node, y_node;
    pt::ptree node1, node2;

    auto p = libff::G2<ppt>(point);
    p.to_affine_coordinates();

#ifdef CURVE_BN1228
    x_node.put("", p.coord[0].a_.toString(16));
    y_node.put("", p.coord[0].b_.toString(16));
#elif CURVE_ALT_BN128
    x_node.put("", coord_to_string(p.X.c0));
    y_node.put("", coord_to_string(p.X.c1));
#endif

    node1.push_back(std::make_pair("", x_node));
    node1.push_back(std::make_pair("", y_node));

    node2.push_back(std::make_pair("", node1));
    x_node.clear();
    y_node.clear();
    node1.clear();

#ifdef CURVE_BN128
    x_node.put("", p.coord[1].a_.toString(16));
    y_node.put("", p.coord[1].b_.toString(16));
#elif CURVE_ALT_BN128
    x_node.put("", coord_to_string(p.Y.c0));
    y_node.put("", coord_to_string(p.Y.c1));
#endif

    node1.push_back(std::make_pair("", x_node));
    node1.push_back(std::make_pair("", y_node));

    node2.push_back(std::make_pair("", node1));

    return node2;
}

template<typename ppT>
void knowledge_commitment_to_ptree(pt::ptree &root, const char *prefix,
                                   libsnark::knowledge_commitment<libff::G1<ppT>, libff::G1<ppT>> &commitment) {

    root.add_child(std::string(prefix) + "_g", point_to_ptree<ppT>(commitment.g));
    root.add_child(std::string(prefix) + "_h", point_to_ptree<ppT>(commitment.h));

}

template<typename ppT>
void knowledge_commitment_to_ptree(pt::ptree &root, const char *prefix,
                                   libsnark::knowledge_commitment<libff::G2<ppT>, libff::G1<ppT>> &commitment) {

    root.add_child(std::string(prefix) + "_g", point_to_ptree<ppT>(commitment.g));
    root.add_child(std::string(prefix) + "_h", point_to_ptree<ppT>(commitment.h));

}

template <typename ppT>
void ptree_to_knowledge_commitment(const char *prefix, pt::ptree &root, libsnark::knowledge_commitment<libff::G1<ppT>, libff::G1<ppT>> &commitment) {
    auto g = root.get_child(std::string(prefix) + "_g");
    auto h = root.get_child(std::string(prefix) + "_h");

    auto point_g = ptree_to_point<ppT>(g);
    auto point_h = ptree_to_point<ppT>(h);

    commitment.g = point_g;
    commitment.h = point_h;
}

template <typename ppT>
void ptree_to_knowledge_commitment(const char *prefix, pt::ptree &root, libsnark::knowledge_commitment<libff::G2<ppT>, libff::G1<ppT>> &commitment) {
    auto g = root.get_child(std::string(prefix) + "_g");
    auto h = root.get_child(std::string(prefix) + "_h");

    auto point_g = ptree_to_point2<ppT>(g);
    auto point_h = ptree_to_point<ppT>(h);

    commitment.g = point_g;
    commitment.h = point_h;
}

template<typename ppT>
pt::ptree proof_to_ptree(libsnark::r1cs_ppzksnark_proof<ppT> &proof) {

    pt::ptree node;

    knowledge_commitment_to_ptree<ppT>(node, "A", proof.g_A);
    knowledge_commitment_to_ptree<ppT>(node, "B", proof.g_B);
    knowledge_commitment_to_ptree<ppT>(node, "C", proof.g_C);

    node.add_child("H", point_to_ptree<ppT>(proof.g_H));
    node.add_child("K", point_to_ptree<ppT>(proof.g_K));

    return node;
}

template <typename ppT>
void print_proof_to_json(libsnark::r1cs_ppzksnark_proof<ppT> &proof, const std::string &file_path) {
    std::ofstream proof_data(file_path);
    if (!proof_data.is_open()) {
        std::cerr << "WARNING: unable to open file at path: " << file_path << std::endl;
        return;
    }

    pt::ptree node = proof_to_ptree<libsnark::default_r1cs_ppzksnark_pp>(proof);

    pt::write_json(proof_data, node);
    proof_data.close();
}

template <typename ppT>
void read_proof_from_json(const std::string &file_path, libsnark::r1cs_ppzksnark_proof<ppT> &proof) {
    pt::ptree root;
    pt::read_json(file_path, root);

    ptree_to_knowledge_commitment<ppT>("A", root, proof.g_A);
    ptree_to_knowledge_commitment<ppT>("B", root, proof.g_B);
    ptree_to_knowledge_commitment<ppT>("C", root, proof.g_C);

    auto g_h = root.get_child("H");
    proof.g_H = ptree_to_point<ppT>(g_h);

    auto g_K = root.get_child("K");
    proof.g_K = ptree_to_point<ppT>(g_K);
}

template<typename ppT>
void print_vk_to_json(libsnark::r1cs_ppzksnark_verification_key<ppT> vk, const std::string &file_path)
{
    std::ofstream vk_file(file_path);
    if (!vk_file.is_open()) {
        std::cerr << "WARNING: unable to open file at path: " << file_path << std::endl;
        return;
    }

    pt::ptree node;

    libff::G2<ppT> A(vk.alphaA_g2);
    A.to_affine_coordinates();
    libff::G1<ppT> B(vk.alphaB_g1);
    B.to_affine_coordinates();
    libff::G2<ppT> C(vk.alphaC_g2);
    C.to_affine_coordinates();

    libff::G2<ppT> gamma(vk.gamma_g2);
    gamma.to_affine_coordinates();
    libff::G1<ppT> gamma_beta_1(vk.gamma_beta_g1);
    gamma_beta_1.to_affine_coordinates();
    libff::G2<ppT> gamma_beta_2(vk.gamma_beta_g2);
    gamma_beta_2.to_affine_coordinates();

    libff::G2<ppT> Z(vk.rC_Z_g2);
    Z.to_affine_coordinates();

    libsnark::accumulation_vector<libff::G1<ppT>> IC(vk.encoded_IC_query);
    libff::G1<ppT> IC_0(IC.first);
    IC_0.to_affine_coordinates();

    node.add_child("A", point_to_ptree<ppT>(A));
    node.add_child("B", point_to_ptree<ppT>(B));
    node.add_child("C", point_to_ptree<ppT>(C));
    node.add_child("gamma", point_to_ptree<ppT>(gamma));
    node.add_child("gamma_beta_1", point_to_ptree<ppT>(gamma_beta_1));
    node.add_child("gamma_beta_2", point_to_ptree<ppT>(gamma_beta_2));
    node.add_child("Z", point_to_ptree<ppT>(Z));
    node.add_child("IC_0", point_to_ptree<ppT>(IC_0));

    for(size_t i=0; i<IC.size(); i++) {
        char buf[10] = {0};
        libff::G1<ppT> IC_N(IC.rest[i]);
        IC_N.to_affine_coordinates();

        sprintf(buf, "IC_%zu", i+1);
        node.add_child(buf, point_to_ptree<ppT>(IC_N));
    }

    pt::write_json(vk_file, node);
    vk_file.close();
}

template<typename ppT>
void print_vk_to_file(libsnark::r1cs_ppzksnark_verification_key<ppT> vk, const std::string &file_path)
{
    std::ofstream vk_data(file_path);
    if (!vk_data.is_open()) {
        std::cerr << "WARNING: unable to open file at path: " << file_path << std::endl;
        return;
    }

    libff::G2<ppT> A(vk.alphaA_g2);
    A.to_affine_coordinates();
    libff::G1<ppT> B(vk.alphaB_g1);
    B.to_affine_coordinates();
    libff::G2<ppT> C(vk.alphaC_g2);
    C.to_affine_coordinates();

    libff::G2<ppT> gamma(vk.gamma_g2);
    gamma.to_affine_coordinates();
    libff::G1<ppT> gamma_beta_1(vk.gamma_beta_g1);
    gamma_beta_1.to_affine_coordinates();
    libff::G2<ppT> gamma_beta_2(vk.gamma_beta_g2);
    gamma_beta_2.to_affine_coordinates();

    libff::G2<ppT> Z(vk.rC_Z_g2);
    Z.to_affine_coordinates();

    libsnark::accumulation_vector<libff::G1<ppT>> IC(vk.encoded_IC_query);
    libff::G1<ppT> IC_0(IC.first);
    IC_0.to_affine_coordinates();

    vk_data << coord_to_string(A.X) << std::endl;
    vk_data << coord_to_string(A.Y) << std::endl;

    vk_data << coord_to_string(B.X) << std::endl;
    vk_data << coord_to_string(B.Y) << std::endl;

    vk_data << coord_to_string(C.X) << std::endl;
    vk_data << coord_to_string(C.Y) << std::endl;

    vk_data << coord_to_string(gamma.X) << std::endl;
    vk_data << coord_to_string(gamma.Y) << std::endl;

    vk_data << coord_to_string(gamma_beta_1.X) << std::endl;
    vk_data << coord_to_string(gamma_beta_1.Y) << std::endl;

    vk_data << coord_to_string(gamma_beta_2.X) << std::endl;
    vk_data << coord_to_string(gamma_beta_2.Y) << std::endl;

    vk_data << coord_to_string(Z.X) << std::endl;
    vk_data << coord_to_string(Z.Y) << std::endl;

    vk_data << coord_to_string(IC_0.X) << std::endl;
    vk_data << coord_to_string(IC_0.Y) << std::endl;

    for(size_t i=0; i<IC.size(); i++) {
        libff::G1<ppT> IC_N(IC.rest[i]);
        IC_N.to_affine_coordinates();
        vk_data << coord_to_string(IC_N.X) << std::endl;
        vk_data << coord_to_string(IC_N.Y) << std::endl;
    }

    vk_data.close();
}

template<typename ppT>
void print_proof_to_file(libsnark::r1cs_ppzksnark_proof<ppT> proof, const std::string &file_path)
{
    std::ofstream proof_data(file_path);
    if (!proof_data.is_open()) {
        std::cerr << "WARNING: unable to open file at path: " << file_path << std::endl;
        return;
    }

    libff::G1<ppT> A_g(proof.g_A.g);
    A_g.to_affine_coordinates();
    libff::G1<ppT> A_h(proof.g_A.h);
    A_h.to_affine_coordinates();

    libff::G2<ppT> B_g(proof.g_B.g);
    B_g.to_affine_coordinates();
    libff::G1<ppT> B_h(proof.g_B.h);
    B_h.to_affine_coordinates();

    libff::G1<ppT> C_g(proof.g_C.g);
    C_g.to_affine_coordinates();
    libff::G1<ppT> C_h(proof.g_C.h);
    C_h.to_affine_coordinates();

    libff::G1<ppT> H(proof.g_H);
    H.to_affine_coordinates();
    libff::G1<ppT> K(proof.g_K);
    K.to_affine_coordinates();

    proof_data << coord_to_string(A_g.X) << std::endl;
    proof_data << coord_to_string(A_g.Y) << std::endl;

    proof_data << coord_to_string(A_h.X) << std::endl;
    proof_data << coord_to_string(A_h.Y) << std::endl;

    proof_data << coord_to_string(B_g.X) << std::endl;
    proof_data << coord_to_string(B_g.Y) << std::endl;

    proof_data << coord_to_string(B_h.X) << std::endl;
    proof_data << coord_to_string(B_h.Y) << std::endl;

    proof_data << coord_to_string(C_g.X) << std::endl;
    proof_data << coord_to_string(C_g.Y) << std::endl;

    proof_data << coord_to_string(C_h.X) << std::endl;
    proof_data << coord_to_string(C_h.Y) << std::endl;

    proof_data << coord_to_string(H.X) << std::endl;
    proof_data << coord_to_string(H.Y) << std::endl;

    proof_data << coord_to_string(K.X) << std::endl;
    proof_data << coord_to_string(K.Y) << std::endl;

    proof_data.close();
}

#endif //COMP_DEFS_H
