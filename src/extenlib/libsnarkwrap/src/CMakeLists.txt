include_directories(.)

add_executable(
  videocoin

  videocoin.cpp
)
target_link_libraries(
  videocoin

  snark
)
target_include_directories(
  videocoin

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
  ${DEPENDS_DIR}/libsnark/depends/libff
  ${DEPENDS_DIR}/libzokrates
)
