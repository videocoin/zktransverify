cmake_minimum_required(VERSION 3.5)
project(videocoin_prooving_system)

# according to libsnark's documentation, we need to use the same conditional defines libsnark was compiled with, as found in
# thirdparty/libsnark/build/libsnark/CMakeFiles/snark.dir/flags.make
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -fopenmp -lstdc++\
    -DCURVE_ALT_BN128 -DBN_SUPPORT_SNARK -DBINARY_OUTPUT\
    -DMONTGOMERY_OUTPUT -DNO_PROCPS -DUSE_ASM -DMULTICORE -ggdb3 -O3 -march=native -mtune=native")

# prover may generate invalid proofs, so to this we have to disable asserts 
add_definitions(-DNDEBUG)

set(THIRDPARTY_DIR ${PROJECT_SOURCE_DIR}/thirdparty)

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${THIRDPARTY_DIR}/dependencies/include
        ${THIRDPARTY_DIR}/libsnark
        ${THIRDPARTY_DIR}/libsnark/depends/libff
        ${THIRDPARTY_DIR}/libsnark/depends/libfqfft
        ${THIRDPARTY_DIR}/ffmpeg
)

link_directories(
        ${PROJECT_BINARY_DIR}
        ${THIRDPARTY_DIR}/dependencies/lib
        ${THIRDPARTY_DIR}/libsnark/build/libsnark
        ${THIRDPARTY_DIR}/libsnark/build/depends
        ${THIRDPARTY_DIR}/libsnark/build/depends/libff/libff
        ${THIRDPARTY_DIR}/ffmpeg/libavcodec
        ${THIRDPARTY_DIR}/ffmpeg/libavfilter
        ${THIRDPARTY_DIR}/ffmpeg/libavformat
        ${THIRDPARTY_DIR}/ffmpeg/libswscale
        ${THIRDPARTY_DIR}/ffmpeg/libpostproc
        ${THIRDPARTY_DIR}/ffmpeg/libswresample
        ${THIRDPARTY_DIR}/ffmpeg/libavutil
)

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

add_executable(
        keygen

        key_generator/main.cpp
        common/utility.cpp
)

add_executable(
        verifier

        verifier/main.cpp
        common/utility.cpp
)

add_executable(
        genproof

        extools/genproof.cpp
        extools/decode-h264-mb.cpp
        extools/sha256-util.cpp
)

add_executable(
        verifyproof

        extools/verifyproof.cpp
        extools/decode-h264-mb.cpp
        common/utility.cpp
)

target_compile_definitions(
        prover
        PRIVATE

        -DDB_NUM_ADDRESSES=1024
        -DRAM_CELL_NUM_BITS=32
        -DDB_HASH_NUM_BITS=1216
        -DSHARED_BSTORE_FN="default_shared_db"
)

target_link_libraries(
        keygen

        -lgmp -lgmpxx -lff -lboost_program_options
)

target_link_libraries(
        verifier

        -lgmp -lgmpxx -lff -lboost_program_options
)

target_link_libraries(
        prover

        -lkyotocabinet -lpapi -lleveldb
)

target_link_libraries(
        genproof

        -lprover -lff -lleveldb -lkyotocabinet -lpapi -lgmp
        -lavfilter -lswscale -lpostproc -lavformat -lavcodec -lswresample -lavutil
        -lcrypto -lm -ldl -lbz2 -lpthread -lX11 -lvpx -lx264 -lx265
        -lz -laom -lvorbis -lvorbisenc -lopus -lmp3lame -lfdk-aac
        -lboost_program_options
)

target_link_libraries(
        verifyproof

        -lgmp -lgmpxx -lff -lboost_program_options
        -lff -lleveldb -lkyotocabinet -lpapi -lgmp
        -lavfilter -lswscale -lpostproc -lavformat -lavcodec -lswresample -lavutil
        -lcrypto -lm -ldl -lbz2 -lpthread -lX11 -lvpx -lx264 -lx265
        -lz -laom -lvorbis -lvorbisenc -lopus -lmp3lame -lfdk-aac
        -lboost_program_options
)

add_dependencies(genproof prover)