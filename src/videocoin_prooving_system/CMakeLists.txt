cmake_minimum_required(VERSION 3.13)
project(videocoin_proof_keygen)

# according to libsnark's documentation, we need to use the same conditional defines libsnark was compiled with, as found in
# thirdparty/libsnark/build/libsnark/CMakeFiles/snark.dir/flags.make
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -fopenmp\
    -DCURVE_BN128 -DBN_SUPPORT_SNARK -DBINARY_OUTPUT\
    -DMONTGOMERY_OUTPUT -DNO_PROCPS -DUSE_ASM -DMULTICORE")

set(THIRDPARTY_DIR ${PROJECT_SOURCE_DIR}/thirdparty)

include_directories(
        ${THIRDPARTY_DIR}/libsnark
        ${THIRDPARTY_DIR}/libsnark/depends/libff
        ${THIRDPARTY_DIR}/libsnark/depends/libfqfft
)

link_directories(
        ${THIRDPARTY_DIR}/libsnark/build/libsnark
        ${THIRDPARTY_DIR}/libsnark/build/depends
        ${THIRDPARTY_DIR}/libsnark/build/depends/libff/libff
)

link_libraries(-lsnark -lff -lzm -lgmp -lgmpxx)

add_executable(proof_keygen key_generator/key_generator.cpp)
add_executable(proof_verifier verifier/verifier.cpp)