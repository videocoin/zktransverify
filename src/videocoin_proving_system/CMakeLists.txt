cmake_minimum_required(VERSION 3.13)
project(videocoin_prooving_system)

# according to libsnark's documentation, we need to use the same conditional defines libsnark was compiled with, as found in
# thirdparty/libsnark/build/libsnark/CMakeFiles/snark.dir/flags.make
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -fopenmp\
    -DCURVE_BN128 -DBN_SUPPORT_SNARK -DBINARY_OUTPUT\
    -DMONTGOMERY_OUTPUT -DNO_PROCPS -DUSE_ASM -DMULTICORE")

set(THIRDPARTY_DIR ${PROJECT_SOURCE_DIR}/thirdparty)

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${THIRDPARTY_DIR}/dependencies/include
        ${THIRDPARTY_DIR}/libsnark
        ${THIRDPARTY_DIR}/libsnark/depends/libff
        ${THIRDPARTY_DIR}/libsnark/depends/libfqfft
)

link_directories(
        ${THIRDPARTY_DIR}/dependencies/lib
        ${THIRDPARTY_DIR}/libsnark/build/libsnark
        ${THIRDPARTY_DIR}/libsnark/build/depends
        ${THIRDPARTY_DIR}/libsnark/build/depends/libff/libff
)

link_libraries(-lsnark -lff -lzm -lgmp -lgmpxx)

add_definitions(-DDB_NUM_ADDRESSES=1024)
add_definitions(-DRAM_CELL_NUM_BITS=32)
add_definitions(-DDB_HASH_NUM_BITS=1216)
add_definitions(-DSHARED_BSTORE_FN="default_shared_db")

add_executable(keygen key_generator/main.cpp common/utility.cpp)
add_executable(verifier verifier/main.cpp common/utility.cpp)
add_library(
        prover
        STATIC

        prover/prover.cpp
        prover/computation_prover.cpp
        common/utility.cpp
        common/waksman_router.cpp
        common/measurement.cpp
        storage/configurable_block_store.cpp
        storage/ram_impl.cpp
        storage/leveldb_block_store.cpp
        storage/ggh_hash.cpp
        storage/exo.cpp
        storage/hasher.cpp
)
target_link_libraries(prover  -lkyotocabinet -lpapi -lleveldb)

add_executable(
        prover_example

        prover_example/ssim_example.cpp
        prover_example/h264_ssim_from_prover.cpp
        prover_example/h264_ssim_original.cpp prover_example/png.cpp)

target_link_libraries(prover_example prover -lpng)